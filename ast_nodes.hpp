#pragma once
#include <utility>

#include "ast_base.hpp"

class AstNumberLiteral : public AstExpression {
    std::shared_ptr<DataType> dataType;
    int intValue = 0;
    float fixedValue = 0;
public:
    explicit AstNumberLiteral(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, const int value)
        : AstExpression(location), dataType(std::move(dataType)), intValue(value) {
    }
    explicit AstNumberLiteral(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, const float value)
        : AstExpression(location), dataType(std::move(dataType)), fixedValue(value) {
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;

    std::string toString() override;

    std::shared_ptr<AstNode> deepCopy() override;

    std::shared_ptr<DataType> getExpressionType() override;

    std::shared_ptr<AstExpression> resolve() override;

    bool isCompileTimeValue() override;

    std::shared_ptr<void> getValue() override;
};

class AstVariable: public AstExpression {
    std::shared_ptr<DataType> dataType;
    std::string name;
    bool immediate = false;
    bool assembleImmediate = false;
    int intValue = 0;
    float fixedValue = 0;
public:
    explicit AstVariable(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, std::string name, const bool immediate):
        AstExpression(location), dataType(std::move(dataType)), name(std::move(name)), immediate(immediate) {
        if (immediate) {
            assembleImmediate = true;
        }
    }
    explicit AstVariable(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, std::string name, const int value):
        AstExpression(location), dataType(std::move(dataType)), name(std::move(name)), immediate(true),intValue(value) {}

    explicit AstVariable(const TokenLocationInfo &location, std::shared_ptr<DataType> dataType, std::string name, const float value):
        AstExpression(location), dataType(std::move(dataType)), name(std::move(name)), immediate(true),fixedValue(value) {}

    std::vector<std::shared_ptr<AstNode>> getNodes() override;

    std::string toString() override;

    std::shared_ptr<AstNode> deepCopy() override;

    std::shared_ptr<DataType> getExpressionType() override;

    std::shared_ptr<AstExpression> resolve() override;

    bool isCompileTimeValue() override;

    std::shared_ptr<void> getValue() override;
};