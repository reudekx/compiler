#include "parser.h"

namespace {

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

template<typename T>
std::pair<AST::NODE, T*> make_node() {
    auto node = std::make_unique<AST::Node>();
    node->data = (AST::Data*)malloc(sizeof(T));
    return { std::move(node), (T*)node->data };
}

};

Parser::Parser(Lexer *lexer) : lexer(lexer) {

}

AST::TOKEN Parser::take_token() {
    return std::make_unique<Token>(lexer->peek());
}

// 토큰의 소모는 따로 처리한다.
void Parser::expect(Token::Type type, const char * const message) {
    if (!lexer->match(type)) {
        throw ParseError(message);
    }
}

/*

각 parse 함수는 peek() 토큰에 의해 식별되었다.
식별된 parse 함수가 해당 토큰을 소모해야 하는 것을 기본 원칙으로..
    가령 parse_import_stmt의 경우 호출 시 먼저 IMPORT 토큰을 소모시켜야 한다.

물론 괄호 등 여러 요소들을 누가 소모해야 할지 애매한 면이 있으므로 잘 구현해보자.

*/

void Parser::parse() {
    try {
        result_message = "Parse completed.";
        ast = parse_file();
    }
    catch(const ParseError &e) {
        result_message = e.what();
    }
}


// 코드 완성 정 IDE 오류 표시 방지용 임시 함수
AST::NODE parse_empty() {
    return std::make_unique<AST::Node>();
}

// 항상 최초에 호출됨
AST::NODE Parser::parse_file() {
    auto [node, file] = make_node<AST::File>();
    if (lexer->match(Token::Type::IMPORT)) {
        file->import_stmt = parse_empty();
    }
    file->global = parse_empty();
    return node;
}

AST::NODE Parser::parse_import_stmt() {
    auto [node, import_stmt] = make_node<AST::ImportStmt>();
    lexer->advance();
    expect(Token::Type::LBRACE);
    return node;
}

AST::NODE Parser::parse_global() {
    auto [node, global] = make_node<AST::Global>();
    return node;
}