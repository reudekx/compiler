#include "parser.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <unordered_map>

/* ====================================================== PARSER ======================================================*/

/*
[논의사항]

1. 현재 lookahead 토큰을 2개 유지하는데 프로그램이 1개 이하의 토큰으로 이뤄져 있을 때도 잘 처리되어야 함.
*/

namespace Parser
{

typedef void (*Action)();

Action Actions[type_count][100] = { nullptr, };

int ParseState = tok_ok;

int Token;
char *Lexeme = nullptr;

EnumStruct TopSymbol;

std::stack<EnumStruct> ParseStack;

std::stack<int> OperatorStack;
std::stack<char *> LiteralStack;

std::stack<int> JumpOrigin;     
std::stack<int> JumpTarget;

std::stack<int> CallStack;

std::stack<int> ScopeStack;

// 임시
std::vector<int> BytecodeList;

struct BytecodeProperty {
    int NumOperands;
    Action BcAction;
};

BytecodeProperty BytecodeTable[bc_length];

void buildBytecodeTable() {
    BytecodeTable[bc_halt] = {0, nullptr};
    BytecodeTable[bc_interrupt] = {0, nullptr};
    BytecodeTable[bc_stack_allocate] = {1, nullptr};
    BytecodeTable[bc_push_null] = {0, nullptr};
    BytecodeTable[bc_push] = {1, nullptr};
    BytecodeTable[bc_pop] = {0, nullptr};
    BytecodeTable[bc_load] = {1, nullptr};
    BytecodeTable[bc_inc] = {0, nullptr};
    BytecodeTable[bc_dec] = {0, nullptr};
    BytecodeTable[bc_fun_def] = {0, nullptr};
    BytecodeTable[bc_fun_end] = {0, nullptr};
    BytecodeTable[bc_call] = {0, nullptr};
    BytecodeTable[bc_return] = {0, nullptr};
    BytecodeTable[bc_jump_if_nil] = {1, nullptr};
    BytecodeTable[bc_jump_if_not_nil] = {1, nullptr};
    BytecodeTable[bc_jump_if_zero] = {1, nullptr};
    BytecodeTable[bc_jump_if_not_zero] = {1, nullptr};
    BytecodeTable[bc_and_jump] = {1, nullptr};
    BytecodeTable[bc_or_jump] = {1, nullptr};    
    BytecodeTable[bc_jump] = {1, nullptr};
    BytecodeTable[bc_store] = {0, nullptr};
    BytecodeTable[bc_local_decl] = {0, nullptr};
    BytecodeTable[bc_or] = {0, nullptr};
    BytecodeTable[bc_and] = {0, nullptr};
    BytecodeTable[bc_not] = {0, nullptr};
    BytecodeTable[bc_equal] = {0, nullptr};
    BytecodeTable[bc_not_equal] = {0, nullptr};
    BytecodeTable[bc_greater_or_equal] = {0, nullptr};
    BytecodeTable[bc_less_or_equal] = {0, nullptr};
    BytecodeTable[bc_greater] = {0, nullptr};
    BytecodeTable[bc_less] = {0, nullptr};
    BytecodeTable[bc_in] = {0, nullptr};
    BytecodeTable[bc_is] = {0, nullptr};
    BytecodeTable[bc_bit_or] = {0, nullptr};
    BytecodeTable[bc_bit_xor] = {0, nullptr};
    BytecodeTable[bc_bit_and] = {0, nullptr};
    BytecodeTable[bc_left_shift] = {0, nullptr};
    BytecodeTable[bc_right_shift] = {0, nullptr};
    BytecodeTable[bc_add] = {0, nullptr};
    BytecodeTable[bc_sub] = {0, nullptr};
    BytecodeTable[bc_mul] = {0, nullptr};
    BytecodeTable[bc_div] = {0, nullptr};
    BytecodeTable[bc_int_div] = {0, nullptr};
    BytecodeTable[bc_mod] = {0, nullptr};
    BytecodeTable[bc_neg] = {0, nullptr};
    BytecodeTable[bc_pos] = {0, nullptr};
    BytecodeTable[bc_power] = {0, nullptr};
    BytecodeTable[bc_dot] = {0, nullptr};
}

struct OperatorProperty {
    int Precedence;
    bool LeftAssociative;
    int Bytecode;
};

/*
    tok_operator에 대해,
    현재 Token과 ParseStack의 Top이 모두 tok_operator일 때
    기존의 Top을 Pop하고 Bytecode로서 emit할지를 우선순위 테이블을 이용해 결정한다.

    생각해보니 단항 연산자를 고려 안 했다. 추가 토큰을 정의하고, unary 파싱 함수에서 적절히 Stack에 삽입할 수 있게 하자. [해결함]

    또한 연산자를 처리하는 방법을 다시 생각해봐야 할 것 같다.
    스택을 분리해야 할 수도?
        -> Operator Stack이 필요하다.
        Parse Stack에서 연산자가 파싱될 때 PUSH/POP을 하고,
        또한 하나의 Expr이 파싱 완료될 때 남은 스택을 모조리 POP 한다.
            -> parseBlock에서 하나의 outmost expression이 끝났을 때 진행하면 된다. (혹은 outmost expression이 시작되기 전)

            다만 소괄호 등에 의해 우선순위가 변경된 것이 잘 식별될까? 혹은 또다른 statement로 구분이 되었을 수도 있고

                -> 자리 표시자를 넣어야 할 듯.

                -> 결국에는 scope가 변하는 모든 곳에 scope를 삽입해야 할지?

                    -> 다만 소괄호에 의해서는 scope가 열리면 안 된다. 
                        -> 따로 처리를 해야 함.

                        -> 결론:
                            Scope가 열릴 때도 sentinal token을 삽입하고,
                            소괄호가 열릴 때도 sentinal token을 삽입한다.

*/
OperatorProperty OperatorTable[tok_length];

void buildOperatorTable() {
    OperatorTable[tok_equal] = {1, false, bc_store};
    OperatorTable[tok_local_decl] = {1, false, bc_local_decl};
    OperatorTable[tok_or] = {2, true, bc_or};
    OperatorTable[tok_and] = {3, true, bc_and};
    OperatorTable[tok_not] = {4, false, bc_not};
    OperatorTable[tok_equal_equal] = {5, true, bc_equal};
    OperatorTable[tok_bang_equal] = {5, true, bc_not_equal};
    OperatorTable[tok_greater_equal] = {5, true, bc_greater_or_equal};
    OperatorTable[tok_less_equal] = {5, true, bc_less_or_equal};
    OperatorTable[tok_greater] = {5, true, bc_greater};
    OperatorTable[tok_less] = {5, true, bc_less};
    OperatorTable[tok_in] = {5, true, bc_in};
    OperatorTable[tok_is] = {5, true, bc_is};
    OperatorTable[tok_bit_or] = {6, true, bc_bit_or};
    OperatorTable[tok_bit_xor] = {7, true, bc_bit_xor};
    OperatorTable[tok_bit_and] = {8, true, bc_bit_and};
    OperatorTable[tok_less_less] = {9, true, bc_left_shift};
    OperatorTable[tok_greater_greater] = {9, true, bc_right_shift};
    OperatorTable[tok_plus] = {10, true, bc_add};
    OperatorTable[tok_minus] = {10, true, bc_sub};
    OperatorTable[tok_star] = {11, true, bc_mul};
    OperatorTable[tok_slash] = {11, true, bc_div};
    OperatorTable[tok_slash_slash] = {11, true, bc_int_div};
    OperatorTable[tok_percent] = {11, true, bc_mod};
    OperatorTable[tok_unary_minus] = {12, false, bc_neg};
    OperatorTable[tok_unary_plus] = {12, false, bc_pos};
    OperatorTable[tok_star_star] = {13, true, bc_power};
    OperatorTable[tok_dot] = {14, true, bc_dot};
}

void consume()
{
    Token = Lexer::getToken();
    Lexeme = Lexer::getLexeme();
}

void checkTerminal()
{

    if (Enum::match(TopSymbol, TOK(eof)))
    {
        ParseState = tok_eof;
        return;
    }

    if (Enum::match(TopSymbol, TOK(err)))
    {
        ParseState = tok_err;
        return;
    }

    if (!Enum::match(TopSymbol, {type_tok, Token}))
    {
        std::cout << "Terminal = " << EnumString[TopSymbol.Type][TopSymbol.Value] << ", Token = " << EnumString[type_tok][Token] << std::endl;
        ParseState = tok_err;
        return;
    }

    if (Token == tok_str || Token == tok_num || Token == tok_id)
    {
        std::cout << "LEXEME = " << Lexeme << std::endl;
    }

    consume();
}

void emitBytecode()
{
    std::cout << "==================== Emit Bytecode: " << TopSymbol.Value << " " << EnumString[TopSymbol.Type][TopSymbol.Value] << std::endl;
    BytecodeList.push_back(TopSymbol.Value);
}

void parseProgram()
{
    ParseStack.push(TOK(eof));
    ParseStack.push(SB(scope));
}

void parseScope()
{
    //ParseStack.push(sb_close_scope);
    ParseStack.push(SB(block));
}

void parseBlock()
{

    switch (Token)
    {
    case tok_ok:
        return;
    case tok_eof:
        return;
    case tok_elif:
        return;
    case tok_else:
        return;
    case tok_return:
        consume();
        ParseStack.push(SB(return_expr));
        return;
    case tok_break:
        consume();
        ParseStack.push(SB(break_expr));
        return;
    default:
        break;
    }
    ParseStack.push(SB(block_tail));
    ParseStack.push(SB(top_expr));
}

void parseBlockTail()
{
    switch (Token)
    {
    case tok_ok:
        return;
    case tok_eof:
        return;
    case tok_elif:
        return;
    case tok_else:
        return;
    case tok_return:
        consume();
        ParseStack.push(SB(return_expr));
        return;
    case tok_break:
        consume();
        ParseStack.push(SB(break_expr));
        return;
    default:
        break;
    }
    ParseStack.push(SB(block_tail));
    ParseStack.push(SB(top_expr));
    ParseStack.push(BC(pop)); // block 내의 문장의 사이에서 기존 문장의 결과값을 pop
}


void parseTopExpr()
{
    OperatorStack.push(tok_sentinel);
    ParseStack.push(SB(pop_until_sentinal));
    ParseStack.push(SB(expr_stmt));
}

void parseExprStmt()
{
    if (Token == tok_semicolon)
    {
        ParseStack.push(BC(push_null));
        consume();
        return;
    }
    ParseStack.push(SB(expr_tail));
    ParseStack.push(SB(expr));
}

void parseExpr()
{
    ParseStack.push(SB(call));
    ParseStack.push(SB(primary));
    ParseStack.push(SB(unary));
}

void parseExprTail()
{
    if (Enum::match(ParseStack.top(), {type_tok, Token})) // 현재 토큰과, 파싱 스택의 TOP이 일치하는 경우 -> 더 이상 확장시킬 필요가 없음.
    {
        return;
    }
    if (OperatorTable[Token].Precedence != 0) // 연산자인 경우
    {
        ParseStack.push(SB(expr_tail));
        ParseStack.push(SB(expr));
        ParseStack.push(SB(binary));
    }
}

void parsePrimary()
{
    switch (Token)
    {
    case tok_left_paren:
        consume();
        ParseStack.push(TOK(right_paren));
        ParseStack.push(SB(top_expr));
        return;
    case tok_do:
        consume();
        ParseStack.push(TOK(ok));
        ParseStack.push(SB(scope));
        return;
    case tok_left_brace:
        consume();
        ParseStack.push(SB(table_decl));
        return;
    case tok_let:
        consume();
        ParseStack.push(SB(let_expr));
        return;
    case tok_if:
        consume();
        ParseStack.push(SB(if_expr));
        return;
    case tok_while:
        consume();
        ParseStack.push(SB(while_expr));
        return;
    case tok_for:
        consume();
        ParseStack.push(SB(for_expr));
        return;
    case tok_fun:
        consume();
        ParseStack.push(SB(func_decl));
        return;
    // case tok_return:
    //     consume();
    //     ParseStack.push(sb_return_expr);
    //     return;
    // case tok_break:
    //     consume();
    //     ParseStack.push(sb_break_expr);
    //     return;
    case tok_at:
        consume();
        ParseStack.push(SB(decorator_expr));
        return;
    case tok_id:
        // Lexeme을 이용해야 하기 때문에 Token 소모 X
        ParseStack.push(SB(identifier));
        return;
    case tok_str:
        // Lexeme을 이용해야 하기 때문에 Token 소모 X
        ParseStack.push(SB(literal));
        return;
    case tok_num:
        // Lexeme을 이용해야 하기 때문에 Token 소모 X
        ParseStack.push(SB(literal));
        return;
    default:
        ParseStack.push(TOK(err));
        return;
    }
}

void parseLiteral()
{
    if (Token == tok_str)
    {
        std::cout << "Lexeme = " << Lexeme << std::endl;
        consume();
        ParseStack.push(BC(load));
        return;
    }
    if (Token == tok_num)
    {
        std::cout << "Lexeme = " << Lexeme << std::endl;
        consume();
        ParseStack.push(BC(push));
        return;
    }
    ParseStack.push(TOK(err));
}

void parseIdentifier()
{
    if (Token == tok_id)
    {
        std::cout << "Identifier = " << Lexeme << std::endl;
        consume();
        ParseStack.push(BC(load));
        return;
    }
    ParseStack.push(TOK(err));
}

void pushOperator(int op)
{
    int Precedence = OperatorTable[op].Precedence;
    if (OperatorTable[op].LeftAssociative)
    {
        if (op == tok_and)
        {

        }
        else if (op == tok_or)
        {
            
        }

        while (OperatorStack.top() != tok_sentinel)
        {
            if (Precedence <= OperatorTable[OperatorStack.top()].Precedence)
            {
                BytecodeList.push_back(OperatorTable[OperatorStack.top()].Bytecode);
                std::cout << "==================== Emit Bytecode: " << OperatorTable[OperatorStack.top()].Bytecode 
                    << " " << EnumString[type_bc][OperatorTable[OperatorStack.top()].Bytecode] << std::endl;
                OperatorStack.pop();
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        while (OperatorStack.top() != tok_sentinel)
        {
            if (Precedence < OperatorTable[OperatorStack.top()].Precedence)
            {
                BytecodeList.push_back(OperatorStack.top());
                std::cout << "==================== Emit Bytecode: " << OperatorTable[OperatorStack.top()].Bytecode 
                    << " " << EnumString[type_bc][OperatorTable[OperatorStack.top()].Bytecode] << std::endl;
                OperatorStack.pop();
            }
            else
            {
                break;
            }
        }
    }
    OperatorStack.push(op);
}

void parseUnary()
{
    switch (Token)
    {
    case tok_minus:
        // unary token으로 바꿔서 삽입
        pushOperator(tok_unary_minus);
        break;
    case tok_plus:
        // unary token으로 바꿔서 삽입
        pushOperator(tok_unary_minus);
    case tok_not:
        pushOperator(tok_not);
        break;
    default:
        // epsilon 처리 (즉, 오류가 아님)
        return;
    }

    consume();
    ParseStack.push(SB(unary));
}

// Binary 연산자의 존재 여부는 외부에서 이미 식별됨
void parseBinary()
{
    pushOperator(Token);
    consume();
    // 아직 연산자별 처리를 가정하지 않았다. -> 연산자 테이블로 각각 처리하도록 함. -> 실제 동작은 바이트코드 함수로 구현하면 됨.
}

void parseCall()
{
    if (Token == tok_left_paren)
    {
        consume();
        ParseStack.push(SB(call));
        ParseStack.push(BC(call));
        ParseStack.push(TOK(right_paren));
        ParseStack.push(SB(argument));
        return;
    }
    // epsilon 처리 (즉, 오류가 아님)
}

void parseArgument()
{
    if (Enum::match(ParseStack.top(), {type_tok, Token}))
    {
        return;
    }
    ParseStack.push(SB(argument_tail));
    ParseStack.push(SB(top_expr));
}

void parseArgumentTail()
{
    if (Token == tok_comma)
    {
        consume();
        ParseStack.push(SB(argument_tail));
        ParseStack.push(SB(top_expr));
        return;
    }
    // epsilon 처리
}

void parseTableDecl()
{
    // '{' 는 이미 소모됨.
    ParseStack.push(TOK(right_brace));
    ParseStack.push(SB(table_body));
}

void parseTableBody()
{
    if (Enum::match(ParseStack.top(), {type_tok, Token}))
    {
        return;
    }
    ParseStack.push(SB(table_body_tail));
    ParseStack.push(SB(var_decl));
}

void parseTableBodyTail()
{
    if (Token == tok_comma)
    {
        consume();
        ParseStack.push(SB(table_body_tail));
        ParseStack.push(SB(var_decl));
    }
    // epsilon 처리 (즉, 오류가 아님)
}

void parseVarDecl()
{
    ParseStack.push(SB(top_expr));
    ParseStack.push(TOK(equal));
    ParseStack.push(SB(identifier));
}

void parseLetExpr()
{
    // let은 이미 소모됨.

    ParseStack.push(SB(top_expr));
    ParseStack.push(SB(local_decl));
    ParseStack.push(SB(identifier));
}

void parseLocalDecl()
{
    if (Token == tok_equal)
    {
        OperatorStack.push(tok_local_decl);
        consume();
        return;
    }
    ParseStack.push(TOK(err));
}

void parseIfExpr()
{
    // if는 이미 소모됨.
    ParseStack.push(SB(elif_expr)); // ELIF, ELSE, OK가 등장 가능
    ParseStack.push(SB(scope));
    ParseStack.push(TOK(do));
    ParseStack.push(BC(jump_if_zero)); // 조건 거짓일 시 다음 조건으로 점프 (POP은 내장되어 있다.)
    ParseStack.push(SB(push_jump_index));
    ParseStack.push(SB(top_expr));
}

void parseElifExpr()
{
    if (Token == tok_ok)
    {
        ParseStack.push(SB(handle_if_end));
        ParseStack.push(SB(push_jump_target)); // 종료 부분
        consume();
        return;
    }
    if (Token == tok_else) // 무조건 ok가 등장하므로, 여기서 처리
    {
        consume();
        ParseStack.push(SB(handle_if_end));
        ParseStack.push(SB(push_jump_target)); // 종료 부분
        ParseStack.push(TOK(ok));
        ParseStack.push(SB(scope));
        ParseStack.push(SB(push_jump_target));
        ParseStack.push(BC(jump)); // IF나 ELIF를 마치고 왔다면, 종료 부분으로 JUMP해야 한다.
        ParseStack.push(SB(push_jump_index));
        return;
    }
    if (Token == tok_elif)
    {
        consume();
        ParseStack.push(SB(elif_expr));
        // sb_block의 종료 조건에 Token이 'elif'인 경우를 처리해야 함.
        ParseStack.push(SB(scope));
        ParseStack.push(TOK(do));
        ParseStack.push(BC(jump_if_zero)); // 조건 거짓일 시 다음 조건으로 점프
        ParseStack.push(SB(push_jump_index));
        ParseStack.push(SB(top_expr));
        ParseStack.push(SB(push_jump_target)); // 이전 IF 혹은 ELIF 조건 불만족 시 여기로 JUMP해야 한다.
        ParseStack.push(BC(jump)); // IF나 ELIF를 마치고 왔다면, 종료 부분으로 JUMP해야 한다.
        ParseStack.push(SB(push_jump_index));
        return;
    }
    ParseStack.push(TOK(err));
}

void parseWhileExpr()
{
    // while은 이미 소모됨.
    ParseStack.push(BC(pop)); // 조건이 거짓이어서 종료되었을 때 POP
    ParseStack.push(SB(handle_loop_end));
    ParseStack.push(SB(push_jump_target)); // 종료 위치
    ParseStack.push(TOK(ok));
    ParseStack.push(BC(jump)); // while문 시작 부분으로 되돌아감.
    ParseStack.push(SB(push_jump_index));
    ParseStack.push(SB(scope));
    ParseStack.push(TOK(do));
    ParseStack.push(BC(jump_if_zero)); // 조건이 거짓일 시 반복문 종료
    ParseStack.push(SB(push_jump_index));
    ParseStack.push(SB(top_expr));
    ParseStack.push(SB(push_jump_target)); // 시작 위치
}

void parseForExpr()
{
    // for는 이미 소모됨.
    ParseStack.push(BC(pop)); // 조건이 거짓이어서 종료되었을 때 POP
    ParseStack.push(SB(handle_loop_end));
    ParseStack.push(SB(push_jump_target)); // 종료 위치
    ParseStack.push(TOK(ok));
    ParseStack.push(BC(jump)); // for문 시작 부분으로 jump
    ParseStack.push(SB(push_jump_index));
    ParseStack.push(SB(scope));
    ParseStack.push(TOK(do));
    ParseStack.push(BC(jump_if_nil)); // parameter의 값이 nil일 때 종료
    ParseStack.push(SB(top_expr));
    ParseStack.push(TOK(in));
    ParseStack.push(SB(parameter)); // 반복시 적절히 다음 원소를 호출하도록 변경 필요
    ParseStack.push(SB(push_jump_target)); // 시작 위치
}

void parseFunDecl()
{
    // fun은 이미 소모됨.
    if (Token == tok_left_paren)
    {
        consume();
    }
    else
    {
        ParseStack.push(TOK(err));
        return;
    }

    ParseStack.push(TOK(ok));
    ParseStack.push(SB(scope));
    ParseStack.push(TOK(right_paren));
    ParseStack.push(SB(parameter));
}

void parseParameter()
{
    if (Token == tok_id)
    {
        ParseStack.push(SB(parameter_tail));
        ParseStack.push(SB(identifier));
    }
    // epsilon 처리 (즉, 오류가 아님)
}

void parseParameterTail()
{
    if (Token == tok_comma)
    {
        consume();
        ParseStack.push(SB(parameter_tail));
        ParseStack.push(SB(identifier));
    }
    // epsilon 처리 (즉, 오류가 아님)
}

void parseReturnExpr()
{
    // return은 이미 소모됨.

    // 세미콜론을 이용해 명시적으로 nil값을 반환할 수도 있지만,
    // 문법적으로 모호하지 않은 상황에선 세미콜론이 없어도 됨.
    if (Token == tok_ok)
    {
        return;
    }
    if (Token == tok_eof)
    {
        return;
    }
    if (Token == tok_elif)
    {
        return;
    }
    if (Token == tok_else)
    {
        return;
    }

    ParseStack.push(SB(top_expr));
}

void parseBreakExpr()
{
    // break는 이미 소모됨.

    // 세미콜론을 이용해 명시적으로 nil값을 반환할 수도 있지만,
    // 문법적으로 모호하지 않은 상황에선 세미콜론이 없어도 됨.
    if (Token == tok_ok)
    {
        return;
    }
    if (Token == tok_eof)
    {
        return;
    }
    if (Token == tok_elif)
    {
        return;
    }
    if (Token == tok_else)
    {
        return;
    }

    ParseStack.push(SB(top_expr));
}

void parseDecoratorExpr()
{
    // @는 이미 소모됨.
    ParseStack.push(SB(top_expr));
}

void parseCloseScope()
{

}

void parsePopUntilSentinal()
{
    while (OperatorStack.top() != tok_sentinel)
    {
        BytecodeList.push_back(OperatorTable[OperatorStack.top()].Bytecode);
        std::cout << "==================== Emit Bytecode: " << OperatorTable[OperatorStack.top()].Bytecode 
            << " " << EnumString[type_bc][OperatorTable[OperatorStack.top()].Bytecode] << std::endl;
        OperatorStack.pop();
    }

    OperatorStack.pop();
}

void buildActions()
{
    for (int i = 0; i < tok_length; i++)
    {
        Actions[type_tok][i] = checkTerminal;
    }

    Action* SbAct = Actions[type_sb];

    SbAct[sb_program] = parseProgram;
    SbAct[sb_scope] = parseScope;
    SbAct[sb_block] = parseBlock;
    SbAct[sb_block_tail] = parseBlockTail;
    SbAct[sb_top_expr] = parseTopExpr;
    SbAct[sb_expr_stmt] = parseExprStmt;
    SbAct[sb_expr] = parseExpr;
    SbAct[sb_expr_tail] = parseExprTail;
    SbAct[sb_primary] = parsePrimary;
    SbAct[sb_call] = parseCall;
    SbAct[sb_unary] = parseUnary;
    SbAct[sb_binary] = parseBinary;
    SbAct[sb_table_decl] = parseTableDecl;
    SbAct[sb_let_expr] = parseLetExpr;
    SbAct[sb_if_expr] = parseIfExpr;
    SbAct[sb_while_expr] = parseWhileExpr;
    SbAct[sb_for_expr] = parseForExpr;
    SbAct[sb_func_decl] = parseFunDecl;
    SbAct[sb_return_expr] = parseReturnExpr;
    SbAct[sb_break_expr] = parseBreakExpr;
    SbAct[sb_decorator_expr] = parseDecoratorExpr;
    SbAct[sb_identifier] = parseIdentifier;
    SbAct[sb_literal] = parseLiteral;
    SbAct[sb_table_body] = parseTableBody;
    SbAct[sb_table_body_tail] = parseTableBodyTail;
    SbAct[sb_var_decl] = parseVarDecl;
    SbAct[sb_elif_expr] = parseElifExpr;
    SbAct[sb_parameter] = parseParameter;
    SbAct[sb_parameter_tail] = parseParameterTail;
    SbAct[sb_argument] = parseArgument;
    SbAct[sb_argument_tail] = parseArgumentTail;
    SbAct[sb_local_decl] = parseLocalDecl;
    SbAct[sb_close_scope] = parseCloseScope;
    SbAct[sb_pop_until_sentinal] = parsePopUntilSentinal;

    for (int i = 0; i < bc_length; i++)
    {
        Actions[type_bc][i] = emitBytecode;
    }
}

void tokenize()
{
    while (Token != tok_eof)
    {
        std::cout << Token << " " << EnumString[Token] << std::endl;
        consume();
    }
}

const std::vector<int>& parse()
{
    while (!ParseStack.empty())
    {
        TopSymbol = ParseStack.top();
        ParseStack.pop();
        std::cout << TopSymbol.Value << " " << EnumString[TopSymbol.Type][TopSymbol.Value] << " / " << Token << " " << EnumString[type_tok][Token] << std::endl;
        Action CurAction = Actions[TopSymbol.Type][TopSymbol.Value];
        
        if (CurAction != nullptr)
        {
            CurAction();
        }

        if (ParseState == tok_err)
        {
            return BytecodeList;
        }
    }

    std::cout << "PARSE ENDED." << std::endl;

    return BytecodeList;
}

int init()
{
    buildOperatorTable();
    buildActions();
    consume();
    OperatorStack.push(tok_sentinel);
    ParseStack.push(SB(program));
    return 0;
}

}
