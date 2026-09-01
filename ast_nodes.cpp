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
        return std::make_shared<AstNumberLiteral>(location, dataType, fixedValue);
    }
    return std::make_shared<AstNumberLiteral>(location, dataType, intValue);
}

std::shared_ptr<DataType> AstNumberLiteral::getExpressionType() {
    return dataType;
}

std::shared_ptr<AstExpression> AstNumberLiteral::resolve() {
    return std::shared_ptr<AstExpression>(this);
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
