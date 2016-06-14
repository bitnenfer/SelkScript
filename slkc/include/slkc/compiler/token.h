#ifndef SLKC_COMPILER_TOKEN_H
#define SLKC_COMPILER_TOKEN_H

#include <slkc/types.h>

typedef enum skETokenType
{
	TOKEN_IDENT,
	TOKEN_EOF,
	TOKEN_SYM_COMMA,
	TOKEN_SYM_DOT,
	TOKEN_SYM_SEMICOLON,
	TOKEN_SYM_PLUS,
	TOKEN_SYM_MINUS,
	TOKEN_SYM_ASTERISK,
	TOKEN_SYM_SLASH,
	TOKEN_SYM_EQUAL,
	TOKEN_SYM_EXCLAM,
	TOKEN_SYM_LPAREN,
	TOKEN_SYM_RPAREN,
	TOKEN_SYM_LCURLY,
	TOKEN_SYM_RCURLY,
	TOKEN_SYM_LBRACK,
	TOKEN_SYM_RBRACK,
	TOKEN_SYM_PERCENT,
	TOKEN_SYM_AMPRESAND,
	TOKEN_SYM_OBELISK,
	TOKEN_SYM_CARET,
	TOKEN_SYM_LOWERTHAN,
	TOKEN_SYM_GREATERTHAN,
	TOKEN_WORD_INT,
	TOKEN_WORD_FLOAT,
	TOKEN_WORD_VOID,
	TOKEN_WORD_BOOL,
	TOKEN_WORD_STRUCT,
	TOKEN_WORD_RETURN,
	TOKEN_WORD_WHILE,
	TOKEN_WORD_BREAK,
	TOKEN_WORD_CONTINUE,
	TOKEN_WORD_ELSE,
	TOKEN_WORD_ELSE_IF,
	TOKEN_WORD_IF,
	TOKEN_WORD_REF,
	TOKEN_WORD_CONST,
	TOKEN_LIT_STRING,
	TOKEN_LIT_INT,
	TOKEN_LIT_FLOAT,
	TOKEN_LIT_BOOL
} skETokenType;

typedef struct skToken
{
	skETokenType type;
	usize line;
	union
	{
		float value_float;
		int value_int;
		int value_bool;
		struct
		{
			char* array;
			usize length;
		} value_ascii_str;
	};
} skToken;

skToken sk_make_token(skETokenType type, usize line);

#endif /* !SLKC_COMPILER_TOKEN_H */