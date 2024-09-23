#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <variant>

#include "token.h"

/*

중간에 메타 구조체를 정의할 필요성이 있는지 생각해보자..
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

struct Base {};

struct File : Base {
    NODE import_stmt;
    NODE global;
};

struct Global {
    NODE_LIST stmts;
};

struct ImportStmt : Base {
    TOKEN_LIST modules;
};

struct ConstDecl : Base {
    TOKEN name;
    NODE type;
    TOKEN value;
};

struct VarDecl : Base {
    TOKEN name;
    NODE type;
    NODE value;
};

struct StructDef : Base {
    TOKEN name;
    TOKEN_LIST member_names;
    NODE_LIST member_types;
};

struct FunDef : Base {
    TOKEN name;
    TOKEN_LIST param_names;
    NODE_LIST param_types;
    NODE return_type;
    NODE_LIST scope; 
};

struct NamedType : Base {
    TOKEN name;
};

struct ArrayType : Base {
    NODE type;
    TOKEN length;
};

struct FunType : Base {
    NODE_LIST param_types;
    NODE return_type;
};

struct Identifier : Base {
    TOKEN id;
};

struct Literal : Base {
    TOKEN value;
};

struct StructLiteral : Base {
    NODE_LIST values;
};

struct ArrayLiteral : Base {
    NODE_LIST values;
};

struct Indexing : Base {
    NODE index;
};

struct ParenExpr : Base {
    NODE expr;
};

struct Call : Base {
    NODE_LIST args;
};

struct AtomicExpr : Base {
    TOKEN_LIST unarys; // Unary operator
    NODE primary_expr; // literal, identifier, paren expr
    NODE_LIST suffixes; // Indxing or Call
};

struct Expr : Base {
    Token binary;
    NODE left;
    NODE right;
};

struct ExprStmt : Base {
    NODE expr;
};

struct Scope : Base {
    NODE_LIST stmts;
};

struct IfStmt : Base {
    NODE_LIST conds;
    NODE_LIST scopes;
};

struct WhileStmt : Base {
    NODE cond;
    NODE scope;
};

using Data = std::variant<
    File, Global, ImportStmt, ConstDecl, VarDecl, StructDef, FunDef,
    NamedType, ArrayType, FunType, Identifier, Literal,
    StructLiteral, ArrayLiteral, Indexing, ParenExpr, Call,
    AtomicExpr, Expr, ExprStmt, Scope, IfStmt, WhileStmt
>;

class Node {
public:
    Data data;
};

};

#endif