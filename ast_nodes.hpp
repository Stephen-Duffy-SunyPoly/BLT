#pragma once
#include <utility>

#include "ast_base.hpp"

class AstNumberLiteral : public AstExpression {
    std::shared_ptr<DataType> dataType;
    const TokenLocationInfo location;
    int intValue = 0;
    float fixedValue = 0;
public:
    explicit AstNumberLiteral(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, const int value)
        : AstExpression(location), dataType(std::move(dataType)), location(location), intValue(value) {
    }
    explicit AstNumberLiteral(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, const float value)
        : AstExpression(location), dataType(std::move(dataType)), location(location), fixedValue(value) {
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;

    std::string toString() override;

    std::shared_ptr<AstNode> deepCopy() override;

    std::shared_ptr<DataType> getExpressionType() override;

    std::shared_ptr<AstExpression> resolve() override;

    bool isCompileTimeValue() override;

    std::shared_ptr<void> getValue() override;
};