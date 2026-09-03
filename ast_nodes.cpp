#include "ast_nodes.hpp"

std::vector<std::shared_ptr<AstNode>> AstNumberLiteral::getNodes() {
    return {};
}

std::string AstNumberLiteral::toString() {
    if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
        return "Constant fixed value: " + std::to_string(fixedValue);
    }
    return "Constant value: "+std::to_string(intValue);
}

std::shared_ptr<AstNode> AstNumberLiteral::deepCopy() {
    if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
        return std::make_shared<AstNumberLiteral>(getLocation(), dataType, fixedValue);
    }
    return std::make_shared<AstNumberLiteral>(getLocation(), dataType, intValue);
}

std::shared_ptr<DataType> AstNumberLiteral::getExpressionType() {
    return dataType;
}

std::shared_ptr<AstExpression> AstNumberLiteral::resolve() {
    //return a deep copy to ensure memory safety (dont make a new shared pointer of this because it could result in a double free later)
    return std::static_pointer_cast<AstExpression>(deepCopy());
}

bool AstNumberLiteral::isCompileTimeValue() {
    return true;
}

std::shared_ptr<void> AstNumberLiteral::getValue() {
    if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
        return std::shared_ptr<void>(new float(fixedValue));
    }
    return std::shared_ptr<void>(new int(intValue));
}

std::vector<std::shared_ptr<AstNode>> AstVariable::getNodes() {
    return {};
}

std::string AstVariable::toString() {
    return "variable: "+name;
}

std::shared_ptr<AstNode> AstVariable::deepCopy() {
    //if it is a regular variable
    if (!immediate) {
        return std::make_shared<AstVariable>(getLocation(), dataType, name,false);
    }
    //if it is a compile time immediate
    if (immediate && assembleImmediate) {
        return std::make_shared<AstVariable>(getLocation(), dataType, name,true);
    }
    //it is now certainly a regular immediate, but of what type?
    if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
        return std::make_shared<AstVariable>(getLocation(), dataType, name,fixedValue);
    }
    return std::make_shared<AstVariable>(getLocation(), dataType, name,intValue);

}

std::shared_ptr<DataType> AstVariable::getExpressionType() {
    return dataType;
}

std::shared_ptr<AstExpression> AstVariable::resolve() {
    if (immediate && !assembleImmediate) {
        //return a number
        if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
            return std::make_shared<AstNumberLiteral>(getLocation(), dataType, fixedValue);
        }
        return std::make_shared<AstNumberLiteral>(getLocation(), dataType, intValue);
    }
    return nullptr;
}

bool AstVariable::isCompileTimeValue() {
    return immediate && !assembleImmediate;
}

std::shared_ptr<void> AstVariable::getValue() {
    if (immediate && !assembleImmediate) {
        //return a number
        if (dataType->getTypeId() == FIXED_TYPE_VALUE) {
            return std::shared_ptr<void>(new float(fixedValue));
        }
        return std::shared_ptr<void>(new int(intValue));
    }
    return nullptr;
}

std::vector<std::shared_ptr<AstNode>> AstAddExpression::getNodes() {
    return {left,right};
}

std::string AstAddExpression::toString() {
    return "AddExpression";
}

std::shared_ptr<AstNode> AstAddExpression::deepCopy() {
    return std::make_shared<AstAddExpression>(getLocation(),
        std::static_pointer_cast<AstExpression>(left->deepCopy()),
        std::static_pointer_cast<AstExpression>(right->deepCopy()
    ));
}

std::shared_ptr<DataType> AstAddExpression::getExpressionType() {
    return left->getExpressionType();
}

std::shared_ptr<AstExpression> AstAddExpression::resolve() {
    //attempt resolution of both branches
    std::shared_ptr<AstExpression> tmp = left->resolve();
    if (tmp != nullptr) {
        left = tmp;
    }
    tmp = right->resolve();
    if (tmp != nullptr) {
        right = tmp;
    }
    //check if both sides values are currently known
    if (left->isCompileTimeValue() && right->isCompileTimeValue()) {
        //combine them
        bool fixed = left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE;
        if (fixed) {
            const float leftValue = *std::static_pointer_cast<float>(left->getValue());
            const float rightValue = *std::static_pointer_cast<float>(right->getValue());
            const float result = leftValue + rightValue;
            if (result > 255.99609375f) {
                //TODO check if warnings should be printed
                std::cerr << "WARNING: Compile time fixed point value overflow at: " + getLocation().toString()<<std::endl;
            }
            return std::make_shared<AstNumberLiteral>(getLocation(), left->getExpressionType(), result);
        }
        //if not a fixed type expression
        const int leftValue = *std::static_pointer_cast<int>(left->getValue());
        const int rightValue = *std::static_pointer_cast<int>(right->getValue());
        int result = leftValue + rightValue;
        if (result > 65535) {
            //TODO check if warnings should be printed
            std::cerr << "WARNING: Compile time value overflow at: " + getLocation().toString()<<std::endl;
        }
        return std::make_shared<AstNumberLiteral>(getLocation(), left->getExpressionType(), result);
    }
    return nullptr;
}

std::vector<std::shared_ptr<AstNode>> AstSubtractExpression::getNodes() {
    return {left,right};
}

std::string AstSubtractExpression::toString() {
    return "SubtractExpression";
}

std::shared_ptr<AstNode> AstSubtractExpression::deepCopy() {
    return std::make_shared<AstSubtractExpression>(getLocation(),
        std::static_pointer_cast<AstExpression>(left->deepCopy()),
        std::static_pointer_cast<AstExpression>(right->deepCopy()
    ));
}

std::shared_ptr<DataType> AstSubtractExpression::getExpressionType() {
    return left->getExpressionType();
}

std::shared_ptr<AstExpression> AstSubtractExpression::resolve() {
    //attempt resolution of both branches
    std::shared_ptr<AstExpression> tmp = left->resolve();
    if (tmp != nullptr) {
        left = tmp;
    }
    tmp = right->resolve();
    if (tmp != nullptr) {
        right = tmp;
    }
    //check if both sides values are currently known
    if (left->isCompileTimeValue() && right->isCompileTimeValue()) {
        //combine them
        bool fixed = left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE;
        if (fixed) {
            const float leftValue = *std::static_pointer_cast<float>(left->getValue());
            const float rightValue = *std::static_pointer_cast<float>(right->getValue());
            const float result = leftValue - rightValue;
            if (result > 255.99609375f) {
                //TODO check if warnings should be printed
                std::cerr << "WARNING: Compile time fixed point value overflow at: " + getLocation().toString()<<std::endl;
            }
            return std::make_shared<AstNumberLiteral>(getLocation(), left->getExpressionType(), result);
        }
        //if not a fixed type expression
        const int leftValue = *std::static_pointer_cast<int>(left->getValue());
        const int rightValue = *std::static_pointer_cast<int>(right->getValue());
        int result = leftValue - rightValue;
        if (result > 65535) {
            //TODO check if warnings should be printed
            std::cerr << "WARNING: Compile time value overflow at: " + getLocation().toString()<<std::endl;
        }
        return std::make_shared<AstNumberLiteral>(getLocation(), left->getExpressionType(), result);
    }
    return nullptr;
}

std::vector<std::shared_ptr<AstNode>> AstMultiplyExpression::getNodes() {
    return {left,right};
}

std::string AstMultiplyExpression::toString() {
    return "MultiplyExpression";
}

std::shared_ptr<AstNode> AstMultiplyExpression::deepCopy() {
    return std::make_shared<AstMultiplyExpression>(getLocation(),
        std::static_pointer_cast<AstExpression>(left->deepCopy()),
        std::static_pointer_cast<AstExpression>(right->deepCopy())
    );
}

std::shared_ptr<DataType> AstMultiplyExpression::getExpressionType() {
    return left->getExpressionType();
}

std::shared_ptr<AstExpression> AstMultiplyExpression::resolve() {
    //attempt resolution of both branches
    std::shared_ptr<AstExpression> tmp = left->resolve();
    if (tmp != nullptr) {
        left = tmp;
    }
    tmp = right->resolve();
    if (tmp != nullptr) {
        right = tmp;
    }
    //check if both sides values are currently known
    if (left->isCompileTimeValue() && right->isCompileTimeValue()) {
        //combine them
        const int leftValue = *std::static_pointer_cast<int>(left->getValue());
        const int rightValue = *std::static_pointer_cast<int>(right->getValue());
        int result = leftValue * rightValue;
        if (result > 65535) {
            //TODO check if warnings should be printed
            std::cerr << "WARNING: Compile time value overflow at: " + getLocation().toString()<<std::endl;
        }
        return std::make_shared<AstNumberLiteral>(getLocation(), left->getExpressionType(), result);
    }
    return nullptr;
}
