#ifndef TOKEN_H
#define TOKEN_H

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
    TOK(TRUE, "true") \
    TOK(FALSE, "false") \
    TOK(NUL, "null") \

// #define TYPES \
//     TOK(I8, "i8") \
//     TOK(I16, "i16") \
//     TOK(I32, "i32") \
//     TOK(I64, "i64") \
//     TOK(I128, "i128") \
//     TOK(F32, "f32") \
//     TOK(F64, "f64") \
//     TOK(BOOL, "bool") \
//     TOK(CHAR, "char") \
//     TOK(VOID, "void") \

#define SYMBOLS \
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

#define KEYWORDS \
    TOK(NEW, "new") \
    TOK(DEL, "del") \
    TOK(FOR, "for") \
    TOK(WHILE, "while") \
    TOK(FUN, "fun") \
    TOK(STRUCT, "struct") \
    TOK(IF, "if") \
    TOK(ELIF, "elif") \
    TOK(ELSE, "else") \
    TOK(TRAIT, "trait") \
    TOK(IMPL, "impl")

#define TOKENS \
    BASICS \
    LITERALS \
    SYMBOLS \
    KEYWORDS \

class Token {
private:

public:
    enum class Type : int {
        #define TOK(name, string) name,
        TOKENS
        #undef TOK
        Count
    };

    inline static const char * const String[] {
        #define TOK(name, string) string,
        TOKENS
        #undef TOK
    };

    static void print_trie();

    inline static const char *to_string(Type token_type) {
        return String[(int)token_type];
    }

    Type to_type(char ch, bool init = false);


};

#endif