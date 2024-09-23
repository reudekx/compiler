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

*/

namespace AST {

struct Base {};

struct File : Base {
    std::unique_ptr<Node> import_stmt;
    std::vector<std::unique_ptr<Node>> global_decls;
};

struct ImportStmt : Base {
    std::vector<std::unique_ptr<Token>> modules;
};

struct ConstDecl : Base {
    std::unique_ptr<Token> name;
    std::unique_ptr<Node> type;
    std::unique_ptr<Token> value;
};

struct VarDecl : Base {
    std::unique_ptr<Token> name;
    std::unique_ptr<Node> type;
    std::unique_ptr<Node> value;
};

struct StructDef : Base {
    std::unique_ptr<Token> name;
    std::vector<std::unique_ptr<Token>> member_names;
    std::vector<std::unique_ptr<Node>> member_types;
};

struct FunDef : Base {
    std::unique_ptr<Token> name;
    std::vector<std::unique_ptr<Token>> param_names;
    std::vector<std::unique_ptr<Node>> param_types;
    std::unique_ptr<Node> return_type;
    std::vector<std::unique_ptr<Node>> scope; 
};

struct NamedType : Base {
    std::unique_ptr<Token> name;
};

struct ArrayType : Base {
    std::unique_ptr<Node> type;
    std::unique_ptr<Token> length;
};

struct FunType : Base {
    std::vector<std::unique_ptr<Node>> param_types;
    std::unique_ptr<Node> return_type;
};

struct Identifier : Base {
    std::unique_ptr<Token> id;
};

struct Literal : Base {
    std::unique_ptr<Token> value;
};

struct StructLiteral : Base {
    std::unique_ptr<Node> values;
};

struct ArrayLiteral : Base {
    std::unique_ptr<Node> values;
};

struct Indexing : Base {
    std::unique_ptr<Node> index;
};

struct ParenExpr : Base {
    std::unique_ptr<Node> expr;
};

struct Call : Base {
    std::unique_ptr<Node> args;
};

struct AtomicExpr : Base {
    std::vector<std::unique_ptr<Token>> unarys; // Unary operator
    std::unique_ptr<Node> primary_expr; // literal, identifier, paren expr
    std::unique_ptr<Node> suffixes; // Indxing or Call
};

struct Expr : Base {
    Token binary;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

struct ExprStmt : Base {
    std::unique_ptr<Node> expr;
};

struct Scope : Base {
    std::vector<std::unique_ptr<Node>> stmts;
};

struct IfStmt : Base {
    std::vector<std::unique_ptr<Node>> conds;
    std::vector<std::unique_ptr<Node>> scopes;
};

struct WhileStmt : Base {
    std::unique_ptr<Node> cond;
    std::unique_ptr<Node> scope;
};

using Data = std::variant<
    File, ImportStmt, ConstDecl, VarDecl, StructDef, FunDef,
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