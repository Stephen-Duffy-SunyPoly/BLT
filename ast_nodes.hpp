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

class AstDivideExpression: public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstDivideExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("DivideExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
        if (left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE) {
            throw std::logic_error("Attempted to use regular divide expression for fixed point division! at: "+location.toString());
        }
    }
    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstModulusExpression: public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstModulusExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("ModulusExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
        if (left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE) {
            throw std::logic_error("Attempted to use modulus operation on fixed type value! at: "+location.toString());
        }
    }
    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};


class AstLeftShiftExpression : public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstLeftShiftExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("LeftShiftExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
        if (left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE) {
            throw std::logic_error("Attempted to use left shift operation on fixed type value! at: "+location.toString());
        }
        if (right->getExpressionType()->getTypeId() == POINTER_TYPE_VALUE) {
            throw std::logic_error("Attempted to bit shift a value by a pinter at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstRightShiftExpression : public AstExpression {
    std::shared_ptr<AstExpression> left;
    std::shared_ptr<AstExpression> right;
public:
    explicit AstRightShiftExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> left, std::shared_ptr<AstExpression> right):
        AstExpression(location), left(std::move(left)), right(std::move(right)) {
        if (!dataTypeCompatible(left->getExpressionType().get(), right->getExpressionType().get())) {
            throw std::logic_error("RightShiftExpression: Attempted to create subtract expression with incompatible types at: "+location.toString());
        }
        if (left->getExpressionType()->getTypeId() == FIXED_TYPE_VALUE) {
            throw std::logic_error("Attempted to use right shift operation on fixed type value! at: "+location.toString());
        }
        if (right->getExpressionType()->getTypeId() == POINTER_TYPE_VALUE) {
            throw std::logic_error("Attempted to bit shift a value by a pinter at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstPointerDerefExpression : public AstExpression {
    std::shared_ptr<AstExpression> pointer;
public:
    explicit AstPointerDerefExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> pointer):
    AstExpression(location), pointer(std::move(pointer)) {
        if (pointer->getExpressionType()->getTypeId() != POINTER_TYPE_VALUE) {
            throw std::logic_error("Attempted to dereference a value that is not a pointer! at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};

class AstNegateExpression : public AstExpression {
    std::shared_ptr<AstExpression> value;
public:
    explicit AstNegateExpression(const TokenLocationInfo &location, std::shared_ptr<AstExpression> value):
        AstExpression(location), value(std::move(value)) {
        if (value->getExpressionType()->getTypeId() != INT_TYPE_VALUE) {
            throw std::logic_error("only int type data can be negated at: "+location.toString());
        }
    }

    std::vector<std::shared_ptr<AstNode>> getNodes() override;
    std::string toString() override;
    std::shared_ptr<AstNode> deepCopy() override;
    std::shared_ptr<DataType> getExpressionType() override;
    std::shared_ptr<AstExpression> resolve() override;
};