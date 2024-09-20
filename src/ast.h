#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>

#include "token.h"

namespace AST {

enum class Type {

};

class Node {
public:
    Type type;
};

class Literal : Node {

};

class Expr : Node {
public:
    std::unique_ptr<AtomicExpr> left;
    std::unique_ptr<AtomicExpr> right;
    Token op;
};

class AtomicExpr : Node {
public:
    std::vector<Token> unarys;
    std::unique_ptr<PrimaryExpr> primary_expr;
};

class PrimaryExpr {

};

class Identifier : PrimaryExpr {
public:
    Token token;
};

class Literal : PrimaryExpr {
public:
    Token token;
};

class ParenExpr : PrimaryExpr {
public:
    std::unique_ptr<Expr> expr;
}

};

#endif