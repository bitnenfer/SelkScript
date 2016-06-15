#ifndef SLKC_COMPILER_TOKEN_H
#define SLKC_COMPILER_TOKEN_H

#include <slkc/types.h>
#include <slkc/macros.h>

typedef enum skETokenType
{
	TOKEN_INVALID,
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
	TOKEN_SYM_AMPERSAND,
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
	TOKEN_WORD_IMPORT,
	TOKEN_LIT_STRING,
	TOKEN_LIT_INT,
	TOKEN_LIT_FLOAT,
	TOKEN_LIT_BOOL
} skETokenType;

SK_FORCEINLINE ascii_char* sk_token_name(skETokenType type)
{
	switch (type)
	{
		case TOKEN_INVALID:
			return "TOKEN_INVALID";
		case TOKEN_IDENT:
			return "TOKEN_IDENT";
		case TOKEN_EOF:
			return "TOKEN_EOF";
		case TOKEN_SYM_COMMA:
			return "TOKEN_SYM_COMMA";
		case TOKEN_SYM_DOT:
			return "TOKEN_SYM_DOT";
		case TOKEN_SYM_SEMICOLON:
			return "TOKEN_SYM_SEMICOLON";
		case TOKEN_SYM_PLUS:
			return "TOKEN_SYM_PLUS";
		case TOKEN_SYM_MINUS:
			return "TOKEN_SYM_MINUS";
		case TOKEN_SYM_ASTERISK:
			return "TOKEN_SYM_ASTERISK";
		case TOKEN_SYM_SLASH:
			return "TOKEN_SYM_SLASH";
		case TOKEN_SYM_EQUAL:
			return "TOKEN_SYM_EQUAL";
		case TOKEN_SYM_EXCLAM:
			return "TOKEN_SYM_EXCLAM";
		case TOKEN_SYM_LPAREN:
			return "TOKEN_SYM_LPAREN";
		case TOKEN_SYM_RPAREN:
			return "TOKEN_SYM_RPAREN";
		case TOKEN_SYM_LCURLY:
			return "TOKEN_SYM_LCURLY";
		case TOKEN_SYM_RCURLY:
			return "TOKEN_SYM_RCURLY";
		case TOKEN_SYM_LBRACK:
			return "TOKEN_SYM_LBRACK";
		case TOKEN_SYM_RBRACK:
			return "TOKEN_SYM_RBRACK";
		case TOKEN_SYM_PERCENT:
			return "TOKEN_SYM_PERCENT";
		case TOKEN_SYM_AMPERSAND:
			return "TOKEN_SYM_AMPERSAND";
		case TOKEN_SYM_OBELISK:
			return "TOKEN_SYM_OBELISK";
		case TOKEN_SYM_CARET:
			return "TOKEN_SYM_CARET";
		case TOKEN_SYM_LOWERTHAN:
			return "TOKEN_SYM_LOWERTHAN";
		case TOKEN_SYM_GREATERTHAN:
			return "TOKEN_SYM_GREATERTHAN";
		case TOKEN_WORD_INT:
			return "TOKEN_WORD_INT";
		case TOKEN_WORD_FLOAT:
			return "TOKEN_WORD_FLOAT";
		case TOKEN_WORD_VOID:
			return "TOKEN_WORD_VOID";
		case TOKEN_WORD_BOOL:
			return "TOKEN_WORD_BOOL";
		case TOKEN_WORD_STRUCT:
			return "TOKEN_WORD_STRUCT";
		case TOKEN_WORD_RETURN:
			return "TOKEN_WORD_RETURN";
		case TOKEN_WORD_WHILE:
			return "TOKEN_WORD_WHILE";
		case TOKEN_WORD_BREAK:
			return "TOKEN_WORD_BREAK";
		case TOKEN_WORD_CONTINUE:
			return "TOKEN_WORD_CONTINUE";
		case TOKEN_WORD_ELSE:
			return "TOKEN_WORD_ELSE";
		case TOKEN_WORD_ELSE_IF:
			return "TOKEN_WORD_ELSE_IF";
		case TOKEN_WORD_IF:
			return "TOKEN_WORD_IF";
		case TOKEN_WORD_REF:
			return "TOKEN_WORD_REF";
		case TOKEN_WORD_CONST:
			return "TOKEN_WORD_CONST";
		case TOKEN_WORD_IMPORT:
			return "TOKEN_WORD_IMPORT";
		case TOKEN_LIT_STRING:
			return "TOKEN_LIT_STRING";
		case TOKEN_LIT_INT:
			return "TOKEN_LIT_INT";
		case TOKEN_LIT_FLOAT:
			return "TOKEN_LIT_FLOAT";
		case TOKEN_LIT_BOOL:
			return "TOKEN_LIT_BOOL";
		default:
			return "INVALID";
	}
}

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