#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <variant>

#include "lexer.h"
#include "ast.h"

class Parser {
private:
    Lexer* lexer;
    std::unique_ptr<AST::Node> m_ast;
    bool m_has_error = false;
    const char* m_error_message;
    Token m_error_token;

    inline const bool match(Token::Type type) const {
        return lexer->peek().type == type;
    }

    inline const bool match_ahead(Token::Type type) const {
        return lexer->ahead().type == type;
    }

    inline void advance() {
        lexer->advance();
    }

    inline Token::Type type() {
        return lexer->peek().type;
    }

    void expect(Token::Type type);
    void unexpect(Token::Type type);
    void consume(Token::Type type);

    AST::TOKEN fetch_token();
    AST::TOKEN take();
    AST::TOKEN take(Token::Type type);

    AST::NODE parse_file();
    AST::NODE parse_import_stmt();
    AST::NODE parse_global();
    AST::NODE parse_const_decl();
    AST::NODE parse_var_decl();
    AST::NODE parse_struct_def();
    AST::NODE parse_fun_def();
    AST::NODE parse_type();
    AST::NODE parse_named_type();
    AST::NODE parse_array_type();
    AST::NODE parse_fun_type();

    AST::NODE parse_identifier();
    AST::NODE parse_literal();
    AST::NODE parse_struct_literal();
    AST::NODE parse_array_literal();
    AST::NODE parse_indexing();
    AST::NODE parse_call();
    AST::NODE parse_paren_expr();

    AST::NODE parse_atomic_expr();
    AST::NODE parse_expr();
    AST::NODE parse_expr_stmt();
    AST::NODE parse_if_stmt();
    AST::NODE parse_while_stmt();
    AST::NODE parse_scope();

public:
    Parser(Lexer *lexer);

    void parse();

    inline const AST::Node* ast() const {
        return m_ast.get();
    };

    inline const bool has_error() const {
        return m_has_error;
    }

    inline const char* const error_message() const {
        return m_error_message;
    }

    inline const Token& error_token() {
        return m_error_token;
    }

};

#endif