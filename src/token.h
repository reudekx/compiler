#ifndef TOKEN_H
#define TOKEN_H

#include <cstdio>
#include <string>

#define BASICS \
    TOK(PART, "#part") \
    TOK(ERROR, "#error") \
    TOK(ENDOFFILE, "#eof") \
    TOK(IDENTIFIER, "#identifier") \

#define LITERALS \
    TOK(CHAR, "#char") \
    TOK(STRING, "#string") \
    TOK(INTEGER, "#integer") \
    TOK(FLOAT, "#float") \

#define SYMBOLS \
    TOK(symbol_beg, "#symbol_beg") \
    TOK(EQ, "=") \
    TOK(PLUS, "+") \
    TOK(MINUS, "-") \
    TOK(STAR, "*") \
    TOK(SLASH, "/") \
    TOK(EQ_EQ, "==") \
    TOK(LPAREN, "(") \
    TOK(RPAREN, ")") \
    TOK(LBRACE, "{") \
    TOK(RBRACE, "}") \
    TOK(LBRACKET, "[") \
    TOK(RBRACKET, "]") \
    TOK(COMMA, ",") \
    TOK(DOT, ".") \
    TOK(SEMICOLON, ";") \
    TOK(symbol_end, "#symbol_end") \

#define KEYWORDS \
    TOK(keyword_beg, "#keyword_beg") \
    TOK(IMPORT, "import") \
    TOK(NEW, "new") \
    TOK(DEL, "del") \
    TOK(FOR, "for") \
    TOK(WHILE, "while") \
    TOK(IF, "if") \
    TOK(ELIF, "elif") \
    TOK(ELSE, "else") \
    TOK(VAR, "var") \
    TOK(CONST, "const") \
    TOK(FUN, "fun") \
    TOK(STRUCT, "struct") \
    TOK(TRAIT, "trait") \
    TOK(IMPL, "impl") \
    TOK(AND, "and") \
    TOK(OR, "or") \
    TOK(NOT, "not") \
    TOK(TRUE, "true") \
    TOK(FALSE, "false") \
    TOK(NUL, "null") \
    TOK(keyword_end, "#keyword_end") \

#define TOKENS \
    BASICS \
    LITERALS \
    SYMBOLS \
    KEYWORDS \

class Token {
public:
    enum class Type : int {
        #define TOK(name, string) name,
        TOKENS
        #undef TOK
        Count
    };

    inline static const char* const String[] {
        #define TOK(name, string) string,
        TOKENS
        #undef TOK
    };

    static void print_trie();

    inline static const char* to_string(Type token_type) {
        return String[(int)token_type];
    }

    static Type check_type(char ch, bool init = false);

    inline void print() {
        printf("[%02d:%02d] Token: %s, Literal: %s\n", line, column, Token::to_string(type), literal.c_str());
    }

    // 필드
    Type type = Type::ERROR;
    int line = 0;
    int column = 0;
    std::string literal;
    int length = 0;
    const char* message = nullptr;

};

#undef TOKENS
#undef BASICS
#undef LITERALS
#undef SYMBOLS
#undef KEYWORDS

#endif