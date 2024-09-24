#include "parser.h"

#include <format>

namespace {

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

template<typename T>
std::pair<AST::NODE, T*> make_node() {
    auto node = std::make_unique<AST::Node>();
    node->data = new T();
    return { std::move(node), static_cast<T*>(node->data) };
}

};

Parser::Parser(Lexer *lexer) : lexer(lexer) {

}

// 토큰의 소모는 따로 처리한다.
void Parser::expect(Token::Type type) {
    if (!match(type)) {
        const std::string message = std::format("Expected type: {} / but got: {}", Token::to_string(type), Token::to_string(lexer->peek().type));
        throw ParseError(message);
    }
}

void Parser::unexpect(Token::Type type) {
    const std::string message = std::format("Got unexpected type: {}", Token::to_string(type));
    throw ParseError(message);
}

void Parser::consume(Token::Type type) {
    expect(type);
    advance();
}

AST::TOKEN Parser::fetch_token() {
    return std::make_unique<Token>(lexer->peek());
}

AST::TOKEN Parser::take() {
    auto token = fetch_token();
    advance();
    return token;
}

AST::TOKEN Parser::take(Token::Type type) {
    expect(type);
    auto token = fetch_token();
    advance();
    return token;
}

/*

각 parse 함수는 peek() 토큰에 의해 식별되었다.
식별된 parse 함수가 해당 토큰을 소모해야 하는 것을 기본 원칙으로..
    가령 parse_import_stmt의 경우 호출 시 먼저 IMPORT 토큰을 소모시켜야 한다.

물론 괄호 등 여러 요소들을 누가 소모해야 할지 애매한 면이 있으므로 잘 구현해보자.

*/

void Parser::parse() {
    try {
        m_result_message = "Parse completed.";
        m_ast = parse_file();
    }
    catch(const ParseError &e) {
        m_result_message = e.what();
    }
}


// 코드 완성 정 IDE 오류 표시 방지용 임시 함수
AST::NODE parse_empty() {
    auto [node, data] = make_node<AST::Data>();
    return node;
}

// 항상 최초에 호출됨
AST::NODE Parser::parse_file() {
    auto [node, file] = make_node<AST::File>();
    if (match(Token::Type::IMPORT)) {
        file->import_stmt = parse_import_stmt();
    }
    file->global = parse_global();
    return node;
}

AST::NODE Parser::parse_import_stmt() {
    auto [node, import_stmt] = make_node<AST::ImportStmt>();
    advance(); // "import"
    consume(Token::Type::LBRACE);
    while(!match(Token::Type::RBRACE)) {
        import_stmt->modules.emplace_back(take(Token::Type::IDENTIFIER));
    }
    advance(); // "}"
    return node;
}

AST::NODE Parser::parse_global() {
    auto [node, global] = make_node<AST::Global>();
    bool end = false;
    while(true) {
        switch(type()) {
            case Token::Type::CONST:
                global->stmts.emplace_back(parse_const_decl());
                break;
            case Token::Type::VAR:
                global->stmts.emplace_back(parse_var_decl());
                break;
            case Token::Type::STRUCT:
                global->stmts.emplace_back(parse_struct_def());
                break;
            case Token::Type::FUN:
                global->stmts.emplace_back(parse_fun_def());
                break;
            case Token::Type::ENDOFFILE:
                end = true;
                break;
            default:
                unexpect(type());
                break;
        }
        if (end) break;
    }
    return node;
}

AST::NODE Parser::parse_const_decl() {
    auto [node, const_decl] = make_node<AST::ConstDecl>();
    advance(); // "const"
    const_decl->name = take(Token::Type::IDENTIFIER);
    const_decl->type = parse_type();
    if (match(Token::Type::EQ)) {
        advance();
        // parse_expr
    }
    return node;
}

AST::NODE Parser::parse_var_decl() {
    auto [node, var_decl] = make_node<AST::VarDecl>();
    advance(); // "var"
    var_decl->name = take(Token::Type::IDENTIFIER);
    var_decl->type = parse_type();
    if (match(Token::Type::EQ)) {
        advance();
        // parse_expr
    }
    return node;
}

AST::NODE Parser::parse_struct_def() {
    auto [node, struct_def] = make_node<AST::StructDef>();
    advance(); // "struct"
    struct_def->name = take(Token::Type::IDENTIFIER);
    return node;
}

AST::NODE Parser::parse_fun_def() {
    auto [node, fun_def] = make_node<AST::FunDef>();
    advance(); // "fun"
    fun_def->name = take(Token::Type::IDENTIFIER);
    return node;
}

AST::NODE Parser::parse_type() {
    switch(type()) {
        case Token::Type::IDENTIFIER:
            return parse_named_type();
            break;
        case Token::Type::LBRACKET:
            return parse_array_type();
            break;
        case Token::Type::FUN:
            return parse_fun_type();
            break;
        default:
            unexpect(type());
            break;
    }
    return parse_empty();
}

AST::NODE Parser::parse_named_type() {
    auto [node, named_type] = make_node<AST::NamedType>();
    named_type->name = take(); // identifier
    return node;
}

AST::NODE Parser::parse_array_type() {
    auto [node, array_type] = make_node<AST::ArrayType>();
    advance();  // "["
    array_type->type = parse_type();
    consume(Token::Type::SEMICOLON);
    array_type->length = take(Token::Type::INTEGER);
    return node;
}

AST::NODE Parser::parse_fun_type() {
    auto [node, fun_type] = make_node<AST::FunType>();
    advance(); // "fun"
    consume(Token::Type::LPAREN);
    if (match(Token::Type::RPAREN)) {
        advance();
    }
    else {
        while (true) {
            fun_type->param_types.emplace_back(parse_type());
            if (match(Token::Type::RPAREN)) {
                advance();
                break;
            }
            consume(Token::Type::COMMA);
        }
    }
    fun_type->return_type = parse_type();
    return node;
}