#pragma once
#include <vector>
#include <memory>
#include "lexer.hpp"
#include "DataTypes.hpp"

class AstNode {
    public:
    virtual ~AstNode() = default;
    virtual std::vector<std::shared_ptr<AstNode>> getNodes() = 0;
    virtual std::string toString() = 0;
    //TODO Convert to IR function
};

class AstExpression : public AstNode {
    public:
    virtual std::shared_ptr<DataType> getExpressionType() = 0;
};