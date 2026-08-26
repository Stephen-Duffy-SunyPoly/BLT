#pragma once
#include <utility>
#include <vector>
#include <memory>
#include "DataTypes.hpp"
#include "commonStructures.hpp"

class AstNode {
    TokenLocationInfo location;
public:
    explicit AstNode(TokenLocationInfo location) : location(std::move(location)) {}
    virtual ~AstNode() = default;
    virtual std::vector<std::shared_ptr<AstNode>> getNodes() = 0;
    virtual std::string toString() = 0;
    //TODO Convert to IR function
    TokenLocationInfo getLocation();
};

class AstExpression : public AstNode {
    public:
    explicit AstExpression(const TokenLocationInfo &location): AstNode(location) {}

    virtual std::shared_ptr<DataType> getExpressionType() = 0;
};