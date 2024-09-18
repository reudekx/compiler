#include "tokenizer.h"
#include "token.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

void Tokenizer::init_lexeme() {
    lexeme.length = 0;
}

void Tokenizer::build_lexeme() {
    lexeme.data[lexeme.length] = scanner.peek();
    lexeme.length++;
}

char* Tokenizer::get_literal() {
    char* literal = (char*)malloc(sizeof(char) * lexeme.length);
    memcpy(literal, lexeme.data, lexeme.length);
    return literal;
}

// 공백임이 앞서 식별됨.
void Tokenizer::consume_space() {
    do {
        scanner.consume();
    } while(isspace(scanner.peek()));
}

// # 문자가 앞서 식별됨.
void Tokenizer::consume_comment() {
    do {
        scanner.consume();
    } while(scanner.peek() != '\n');
}

// 숫자가 앞서 식별됨.
void Tokenizer::tokenize_number() {
    cur_token.line = scanner.line;
    cur_token.column = scanner.column;

    init_lexeme();
    do {
        build_lexeme();
        scanner.consume();
    } while(isdigit(scanner.peek()));

    cur_token.type = Token::Type::INTEGER;
    cur_token.literal = get_literal();
    cur_token.length = lexeme.length;
}

// " 문자가 앞서 식별됨.
void Tokenizer::tokenize_string() {
    cur_token.line = scanner.line;
    cur_token.column = scanner.column;

    scanner.consume();
    init_lexeme();
    while (scanner.peek() != '"') {
        build_lexeme();
        scanner.consume();
    }

    scanner.consume();

    cur_token.type = Token::Type::STRING;
    cur_token.literal = get_literal();
    cur_token.length = lexeme.length;
}

// 알파벳 혹은 _ 문자가 앞서 식별됨.
void Tokenizer::tokenize_keyword() {
    Token::Type token_type = Token::check_type(scanner.peek(), true);

    cur_token.line = scanner.line;
    cur_token.column = scanner.column;

    init_lexeme();
    build_lexeme();
    scanner.consume();

    while (isalnum(scanner.peek()) || scanner.peek() == '_') {
        token_type = Token::check_type(scanner.peek());
        build_lexeme();
        scanner.consume();
    }

    if (token_type == Token::Type::IDENTIFIER) {
        cur_token.literal = get_literal();
        cur_token.length = lexeme.length;
    }
    else {
        cur_token.literal = nullptr;
        cur_token.length = 0;
    }

    cur_token.type = token_type;
}

// EOF가 앞서 식별됨.
void Tokenizer::tokenize_eof() {
    cur_token.type = Token::Type::ENDOFFILE;
    cur_token.line = scanner.line;
    cur_token.column = scanner.column;
    cur_token.literal = nullptr;
    cur_token.length = 0;
}

// 나머지 경우 (기호 문자이거나, 유효하지 않은 문자임.)
void Tokenizer::tokenize_symbol() {
    Token::Type token_type = Token::Type::ERROR;
    Token::Type cur_token_type = Token::check_type(scanner.peek(), true);

    cur_token.line = scanner.line;
    cur_token.column = scanner.column;

    while (true) {
        if (cur_token_type == Token::Type::IDENTIFIER) {
            break;
        }
        token_type = cur_token_type;
        scanner.consume();
        cur_token_type = Token::check_type(scanner.peek());
    }
    
    cur_token.type = token_type;
    cur_token.literal = nullptr;
    cur_token.length = 0;
}

const Token& Tokenizer::tokenize() {
    while(true) {
        if (isspace(scanner.peek())) {
            consume_space();
            continue;
        }
        
        if (scanner.peek() == '#') {
            consume_comment();
            continue;
        }

        if (isdigit(scanner.peek())) {
            tokenize_number();
            break;
        }

        if (scanner.peek() == '"') {
            tokenize_string();
            break;
        }

        if (scanner.peek() == EOF) {
            tokenize_eof();
            break;
        }

        if (isalpha(scanner.peek()) || scanner.peek() == '_') {
            tokenize_keyword();
            break;
        }

        tokenize_symbol();
        break;
    }
    return cur_token;
}
