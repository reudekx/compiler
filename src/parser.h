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
    const char* m_result_message;

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

public:
    Parser(Lexer *lexer);

    void parse();

    inline const AST::Node* ast() const {
        return m_ast.get();
    };

    inline const char* const result_message() const {
        return m_result_message;
    }

};

#endif