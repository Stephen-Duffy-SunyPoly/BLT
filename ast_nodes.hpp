#pragma once
#include <stdexcept>
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

class AstAddExpression: public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstAddExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        //validate the param are of valid types
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("AddExpression: Attempted to create add expression with incompatible types at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstSubtractExpression: public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstSubtractExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        //validate the param are of valid types
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("SubtractExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
    }
    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstMultiplyExpression: public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstMultiplyExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
       AstExpression(location), left(std::move(left)), right(std::move(right)) {
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("MultiplyExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
        if (left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE) {
            throw std::logic_error("Attempted to use regular multiply expression for fixed point multiplication! at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};