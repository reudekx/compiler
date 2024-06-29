#include "lexer.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <unordered_map>

#define CHUNK_SIZE 64

namespace Lexer {

FILE* SourceCode;

int Bc = 0, Fc = -1;

char Chunk[2][CHUNK_SIZE];
int CurChunkIdx = 0;
int ChunkLength = 0;

bool BcChunkIdx = 0;

int LexemeLength = 0;

char* Lexeme = nullptr;

char Char = ' ';

/*
    수정 필요 사항

    1. std::string 대신 더 효율적인 자료형 이용하여 문자열 처리하기 (해결)

    2. 현재 주석 skip 시 재귀 호출 (해결)
        -> 호출 스택이 깊어지는 문제 해결 必
            -> 전체를 while문으로 만들자.
    3. 문자열 리터럴 식별시 큰/작은 따옴표 모두 고려하기
        -> 현재는 일단 큰 따옴표만 처리 함.

    4. Lexeme 조립 시 memcpy 등을 이용해 더욱 효율적으로 처리하기 (해결)

    논의 사항

    1. 토큰으로 정의되지 않은 문자가 등장할 경우 처리 방안
        1. 그대로 반환한다. (가령, 소괄호나 쉼표를 파서에서 처리)
        2. 처리가 필요한 모든 문자를 토큰으로 정의하고, 정의되지 않은 문자는 그냥 무시하기

        -> 일단 2번째 방법으로 하자.

    2. string literal 처리 방안
        lexing 과정에서 문자열 객체를 생성해야 하는가?
            -> 고민 必

        IR 생성 시에는 배열로 처리하면 될 듯?

*/

/*
    문자를 사용하고난 뒤 comsume을 호출 하여 다음 문자를 Char에 담을 수 있다.
    또한 Fc는 현재 Char의 index를 가리킨다.


*/


/* ===================================================== LEXER ========================================================*/


int consume()
{
    Fc++;

    if (Fc == Bc && CurChunkIdx != BcChunkIdx)
    {
        Char = tok_err;
        return Char;
    }

    if (Fc == ChunkLength)
    {
        if (ChunkLength == CHUNK_SIZE)
        {
            Fc = 0;
            CurChunkIdx = 1 - CurChunkIdx;
            ChunkLength = (int)fread(Chunk[CurChunkIdx], 1, CHUNK_SIZE, SourceCode);
            if (ChunkLength < CHUNK_SIZE)
            {
                Chunk[CurChunkIdx][ChunkLength] = EOF;
            }
        }
        else
        {
            Char = EOF;
            return Char;
        }
    }

    Char = Chunk[CurChunkIdx][Fc];

    return Char;
}

// Lexeme의 길이만큼만 해당 함수를 호출해야 함. 이는 호출하는 부분의 몫
// Bc부터 Fc - 1까지가 Lexeme의 범위이다.
char getLexemeChar()
{
    char CurChar = Chunk[BcChunkIdx][Bc];
    Bc++;
    if (Bc == CHUNK_SIZE)
    {
        Bc = 0;
        BcChunkIdx = CurChunkIdx;
    }

    return CurChar;
}

void buildLexeme()
{
    LexemeLength = (Fc - Bc + CHUNK_SIZE) % CHUNK_SIZE;
    Lexeme = (char *)malloc(sizeof(char) * LexemeLength + 1);
    if (CurChunkIdx != BcChunkIdx)
    {
        memcpy(Lexeme, Chunk[BcChunkIdx] + Bc, CHUNK_SIZE - Bc);
        memcpy(Lexeme + CHUNK_SIZE - Bc, Chunk[CurChunkIdx], Fc);
    }
    else
    {
        memcpy(Lexeme, Chunk[CurChunkIdx] + Bc, LexemeLength);
    }
    Lexeme[LexemeLength] = '\0';
}

bool isLexemeKeyword(const char* Keyword, int KeywordLength)
{
    if (LexemeLength != KeywordLength)
        return false;
    return strcmp(Lexeme, Keyword) == 0;
}

/*
    기본 원칙:
        사용한 문자는 토큰 반환 전에 소모시켜야 한다.
*/

int getToken()
{

    while (true)
    {
        while (isspace(Char)) // 공백 제거
            consume();

        if (isalpha(Char))
        {
            Bc = Fc;
            BcChunkIdx = CurChunkIdx;
            while (isalnum(consume()));

            buildLexeme();

            switch (Lexeme[0])
            {
            case 'a':
                if (isLexemeKeyword("and", 3))
                    return tok_and;
                break;
            case 'b':
                if (isLexemeKeyword("break", 5))
                    return tok_break;
                break;
            case 'd':
                if (isLexemeKeyword("do", 2))
                    return tok_do;
                break;
            case 'f':
                if (isLexemeKeyword("for", 3))
                    return tok_for;
                if (isLexemeKeyword("fun", 3))
                    return tok_fun;
                break;
            case 'e':
                if (isLexemeKeyword("elif", 4))
                    return tok_elif;
                if (isLexemeKeyword("else", 4))
                    return tok_else;
                break;
            case 'i':
                if (isLexemeKeyword("if", 2))
                    return tok_if;
                if (isLexemeKeyword("is", 2))
                    return tok_is;
                if (isLexemeKeyword("in", 2))
                    return tok_in;
                break;
            case 'l':
                if (isLexemeKeyword("let", 3))
                    return tok_let;
                break;
            case 'n':
                if (isLexemeKeyword("not", 3))
                    return tok_not;
                if (isLexemeKeyword("nil", 3))
                    return tok_nil;
                break;
            case 'o':
                if (isLexemeKeyword("or", 2))
                    return tok_or;
                if (isLexemeKeyword("ok", 2))
                    return tok_ok;
                break;
            case 'r':
                if (isLexemeKeyword("return", 6))
                    return tok_return;
            case 'w':
                if (isLexemeKeyword("while", 5))
                    return tok_while;
                break;
            }

            return tok_id;
        }

        if (isdigit(Char)) // 숫자
        {
            Bc = Fc;
            BcChunkIdx = CurChunkIdx;
            do
            {
                consume();
            } while (isdigit(Char) || Char == '.');

            buildLexeme();

            return tok_num;
        }

        if (Char == '"') // 문자열
        {
            consume();

            Bc = Fc;
            BcChunkIdx = CurChunkIdx;

            while (Char != '"' && Char != EOF)
            {
                consume();
            }

            buildLexeme();

            if (Char == '"')
                consume();

            return tok_str;
        }

        if (Char == '#') // 주석
        {
            do
                consume();
            while (Char != '#' && Char != EOF);

            if (Char == '#')
                consume();

            continue;
            
        }

        if (Char == EOF) // EOF
            return tok_eof;


        // 연산자

        char Op = Char;
        consume();

        switch (Op)
        {
        case '@':
            return tok_at;
        case ';':
            return tok_semicolon;
        case '*':
            if (Char == '=')
            {
                consume();
                return tok_star_equal;
            }
            if (Char == '*')
            {
                consume();
                if (Char == '=')
                {
                    consume();
                    return tok_star_star_equal;
                }
                return tok_star_star;
            }
            return tok_star;
        case '/':
            if (Char == '=')
            {
                consume();
                return tok_slash_equal;
            }
            if (Char =='/')
            {
                consume();
                if (Char == '=')
                {
                    consume();
                    return tok_slash_slash_equal;
                }
                return tok_slash_slash;
            }
            return tok_slash;
        case '+':
            if (Char == '=')
            {
                consume();
                return tok_plus_equal;
            }
            return tok_plus;
        case '-':
            if (Char == '=')
            {
                consume();
                return tok_minus_equal;
            }
            return tok_minus;
        case '%':
            if (Char == '%')
            {
                consume();
                return tok_percent_equal;
            }
            return tok_percent;
        case '!':
            if (Char == '=')
            {
               consume();
               return tok_bang_equal;
            }
            return tok_bang;
        case '=':
            if (Char == '=')
            {
                consume();
                return tok_equal_equal;
            }
            return tok_equal;
        case '<':
            if (Char == '=')
            {
                consume();
                return tok_less_equal;
            }
            return tok_less;
        case '>':
            if (Char == '=')
            {
                consume();
                return tok_greater_equal;
            }
            return tok_greater;
        case '&':
            if (Char == '=')
            {
                consume();
                return tok_bit_and_equal;
            }
            return tok_bit_and;
        case '^':
            if (Char == '=')
            {
                consume();
                return tok_bit_xor_equal;
            }
            return tok_bit_xor;
        case '|':
            if (Char == '=')
            {
                consume();
                return tok_bit_or_equal;
            }
            return tok_bit_or;
        case '~':
            if (Char == '=')
            {
                consume();
                return tok_bit_not_equal;
            }
            return tok_bit_not;
        case ',':
            return tok_comma;
        case '.':
            return tok_dot;
        case '(':
            return tok_left_paren;
        case ')':
            return tok_right_paren;
        case '{':
            return tok_left_brace;
        case '}':
            return tok_right_brace;
        case '[':
            return tok_left_bracket;
        case ']':
            return tok_right_bracket;
        }

    }
}

char* getLexeme()
{
    return Lexeme;
}

int init(const char* FilePath)
{
    SourceCode = fopen(FilePath, "rb");
    if (SourceCode == nullptr) 
    {
        perror("Error opening file");
        return 1;
    }

    ChunkLength = (int)fread(Chunk[CurChunkIdx], 1, CHUNK_SIZE, SourceCode);

    if (ChunkLength < CHUNK_SIZE)
    {
        Chunk[CurChunkIdx][ChunkLength] = EOF;
    }

    return 0;
}

int quit()
{
    return fclose(SourceCode);
}

}
