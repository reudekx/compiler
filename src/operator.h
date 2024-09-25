#ifndef OPERATOR_H
#define OPERATOR_H

#include "token.h"

namespace Operator {

enum class Type {
    NONE,
    UNARY,
    BINARY,
    HYBRID,
};

struct Info {
    Type type;
    int precedence;
};

namespace {

constexpr auto build_info() {
    Info info[(int)Token::Type::Count] = { {Type::NONE, 0} };


    return info;
}

};


const Info* const info = build_info();

};

#endif