#ifndef ENUMS_H
#define ENUMS_H

#define SB(x)                                                                  \
  { type_sb, sb_##x }
#define TOK(x)                                                                 \
  { type_tok, tok_##x }
#define BC(x)                                                                  \
  { type_bc, bc_##x }

enum TypeEnum {
  type_tok = 0,
  type_sb,
  type_bc,
  type_count,
};

enum TokenEnum {
  tok_eof = 0,

  tok_err,

  tok_sentinel,

  tok_extern,

  tok_import,

  tok_let,
  tok_fun,

  tok_do,
  tok_ok,

  tok_return,
  tok_break,

  tok_at,
  tok_semicolon,

  tok_id,

  tok_nil,

  tok_num,
  tok_str,

  tok_plus,
  tok_plus_equal,
  tok_minus,
  tok_minus_equal,
  tok_star,
  tok_star_equal,
  tok_star_star,
  tok_star_star_equal,
  tok_slash,
  tok_slash_equal,
  tok_slash_slash,
  tok_slash_slash_equal,
  tok_percent,
  tok_percent_equal,

  tok_bang,
  tok_bang_equal,
  tok_equal,
  tok_equal_equal,
  tok_greater,
  tok_greater_equal,
  tok_greater_greater,
  tok_less,
  tok_less_equal,
  tok_less_less,

  tok_bit_and,
  tok_bit_and_equal,
  tok_bit_xor,
  tok_bit_xor_equal,
  tok_bit_or,
  tok_bit_or_equal,
  tok_bit_not,
  tok_bit_not_equal,

  tok_and,
  tok_or,
  tok_not,
  tok_in,
  tok_is,

  tok_for,
  tok_while,

  tok_if,
  tok_elif,
  tok_else,

  tok_comma,
  tok_dot,

  tok_left_paren,
  tok_right_paren,
  tok_left_brace,
  tok_right_brace,
  tok_left_bracket,
  tok_right_bracket,

  // 실제 Token으로 식별되진 않으나 편의상 정의함.
  tok_unary_minus,
  tok_unary_plus,
  tok_local_decl,

  tok_length
};

enum SymbolEnum {
  sb_program = 0,
  sb_scope,
  sb_block,
  sb_block_tail,
  sb_top_expr,
  sb_expr_stmt,
  sb_expr,
  sb_expr_tail,
  sb_unary,
  sb_primary,
  sb_call,
  sb_binary,
  sb_table_decl,
  sb_let_expr,
  sb_if_expr,
  sb_while_expr,
  sb_for_expr,
  sb_func_decl,
  sb_return_expr,
  sb_break_expr,
  sb_decorator_expr,
  sb_identifier,
  sb_literal,

  sb_table_body,
  sb_table_body_tail,

  sb_var_decl,

  sb_elif_expr,

  sb_parameter,
  sb_parameter_tail,

  sb_argument,
  sb_argument_tail,

  sb_local_decl,

  sb_open_scope,
  sb_close_scope,

  sb_put_sentinal,
  sb_pop_until_sentinal,

  sb_push_jump_index,
  sb_push_jump_target,

  sb_handle_loop_end,
  sb_handle_if_end,
  sb_handle_logical_operator,

  sb_length
};

enum BytecodeEnum {
  bc_halt = 0,
  bc_interrupt,

  bc_stack_allocate,

  // 상수 스택 연산
  bc_push_null, // 임시 (제거 예정)
  bc_push,
  bc_pop,

  // 변수를 스택에 PUSH
  bc_load,

  bc_inc,
  bc_dec,

  bc_fun_def,
  bc_fun_end,

  // PUSH to CallStack
  bc_call,

  // POP from CallStack
  bc_return,

  bc_jump_if_nil,
  bc_jump_if_not_nil,
  bc_jump_if_zero,
  bc_jump_if_not_zero,
  bc_and_jump,
  bc_or_jump,
  bc_jump,

  // Operators
  bc_store,
  bc_local_decl,
  bc_or,
  bc_and,
  bc_not,
  bc_equal,
  bc_not_equal,
  bc_greater_or_equal,
  bc_less_or_equal,
  bc_greater,
  bc_less,
  bc_in,
  bc_is,
  bc_bit_or,
  bc_bit_xor,
  bc_bit_and,
  bc_left_shift,
  bc_right_shift,
  bc_add,
  bc_sub,
  bc_mul,
  bc_div,
  bc_int_div,
  bc_mod,
  bc_neg,
  bc_pos,
  bc_power,
  bc_dot,

  bc_length
};

struct EnumStruct {
  int Type;
  int Value;
};

namespace Enum {

int match(EnumStruct a, EnumStruct b);

}

/*=================================임시 출력용
 * 문자열======================================*/

extern const char *EnumString[3][100];

#endif
