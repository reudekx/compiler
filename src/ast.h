#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <variant>
#include <iostream>

#include "token.h"

#define PARENS ()

#define APPLY(...) APPLY2(APPLY2(APPLY2(__VA_ARGS__)))
#define APPLY2(...) APPLY3(APPLY3(APPLY3(__VA_ARGS__)))
#define APPLY3(...) __VA_ARGS__

#define REPEAT(macro, ...) \
    __VA_OPT__(APPLY(PAIR(macro, __VA_ARGS__)))

#define PAIR(macro, a, b, ...) \
    macro(a, b) \
    __VA_OPT__(AGAIN PARENS (macro, __VA_ARGS__))

#define AGAIN() PAIR

#define DEF_MEMBER(type, name) type name;

#define DEF_PRINT(type, name) \
    print_space(space + 1); \
    std::cout << "% " << #name << std::endl; \
    print_child(name, space); \


#define AST_DATA(name, ...) \
    struct name : Data { \
        REPEAT(DEF_MEMBER, __VA_ARGS__) \
        const char* const label() const override { \
            return #name; \
        } \
        void print(int space) const override { \
            print_space(space); \
            std::cout << "<" << #name << ">" << std::endl; \
            REPEAT(DEF_PRINT, __VA_ARGS__) \
        } \
    }; \
    

/*

중간에 메타 구조체를 정의할 필요가 있는지 생각해보자..
가령 GlobalDecl 구조체는 필요할까?
GlobalDecl에는 전역 const, var, struct, funcion에 대한 정의가 뒤따른다.
어차피 File 노드에서 parse_global_decls 함수를 호출해서 파싱하면 되긴 한다.

결론: GlobalDecl의 경우에는 어차피 File에서 무조건 처리가 가능하므로 정의하지 않아도 됨.
(같은 논리로 GlobalConstDecl, GlobalVarDecl 등도 필요없다.)

물론 상위 구조만으로 존재가 식별되지 않는 경우에는 구조체를 분리할 필요가 있음.

ParenExpr 같은 경우에는?
단순히 Expr을 캡슐화하는 용도긴 한데.. 일단 따로 정의한 뒤 나중에 필요 없으면 제거하자.

======

즉 AST 구성 단계에서 노드의 의미를 구체화할지, 이후 의미 분석 단계에서 구조를 보고 노드의 의미를 파악할지 적절히 정해서 구현해야 함.

*/

namespace AST {

class Node;

using NODE = std::unique_ptr<Node>;
using TOKEN = std::unique_ptr<Token>;
using NODE_LIST = std::vector<std::unique_ptr<Node>>;
using TOKEN_LIST = std::vector<std::unique_ptr<Token>>;

struct Data {
    virtual const char* const label() const {
        return "Data";
    }

    virtual void print(int space = 0) const {
        std::cout << std::string(space, ' ') << "Data" << std::endl;
    }

    virtual ~Data() = default;
};

class Node {
public:
    Data* data;

    ~Node() {
        free(data);
    }
};

static void print_space(int space) {
    for (int i = 0; i < space; i++) {
        std::cout << " ";
    }
}

static void print_child(const NODE& node, int space) {
    if (!node) return;
    node->data->print(space + 4);
}

static void print_child(const TOKEN& token, int space) {
    if (!token) return;
    print_space(space + 4);
    token->print();
}

static void print_child(const NODE_LIST& nodes, int space) {
    for (const auto& node : nodes) {
        node->data->print(space + 4);
    }
}

static void print_child(const TOKEN_LIST& tokens, int space) {
    for (const auto& token : tokens) {
        print_space(space + 4);
        token->print();
    }
}

AST_DATA(File, NODE, import_stmt, NODE, global)
AST_DATA(Global, NODE_LIST, stmts)
AST_DATA(ImportStmt, TOKEN_LIST, modules)
AST_DATA(ConstDecl, TOKEN, name, NODE, type, TOKEN, value)
AST_DATA(VarDecl, TOKEN, name, NODE, type, NODE, value)
AST_DATA(StructDef, TOKEN, name, TOKEN_LIST, member_names, NODE_LIST, member_types)
AST_DATA(FunDef, TOKEN, name, TOKEN_LIST, param_names, NODE_LIST, param_types, NODE, return_type, NODE_LIST, scope)
AST_DATA(NamedType, TOKEN, name)
AST_DATA(ArrayType, NODE, type, TOKEN, length)
AST_DATA(FunType, NODE_LIST, param_types, NODE, return_type)
AST_DATA(Identifier, TOKEN, id)
AST_DATA(Literal, TOKEN, value)
AST_DATA(StructLiteral, NODE_LIST, values)
AST_DATA(ArrayLiteral, NODE_LIST, values)
AST_DATA(Indexing, NODE, index)
AST_DATA(ParenExpr, NODE, expr)
AST_DATA(Call, NODE_LIST, args)
AST_DATA(AtomicExpr, TOKEN_LIST, unarys, NODE, primary_expr, NODE_LIST, suffixes)
AST_DATA(Expr, TOKEN, binary, NODE, left, NODE, right)
AST_DATA(ExprStmt, NODE, expr)
AST_DATA(Scope, NODE_LIST, stmts)
AST_DATA(IfStmt, NODE_LIST, conds, NODE_LIST, scopes)
AST_DATA(WhileStmt, NODE, cond, NODE, scope)

};

#endif