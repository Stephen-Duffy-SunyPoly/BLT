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
