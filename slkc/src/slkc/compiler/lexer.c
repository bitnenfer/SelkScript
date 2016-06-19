#include <slkc/compiler/lexer.h>
#include <slkc/array.h>
#include <slkc/error.h>
#include <string.h>

//#define TOKEN_DEBUG

static const ascii_char* WORD_DIGIT = "0123456789";
static const ascii_char* WORD_ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
static const ascii_char* WORD_BINDIGIT = "01";
static const ascii_char* WORD_HEXDIGIT = "0123456789abcdefABCDEF";

static const ascii_char* KEYWORD_INT = "int";
static const ascii_char* KEYWORD_FLOAT = "float";
static const ascii_char* KEYWORD_VOID = "void";
static const ascii_char* KEYWORD_BOOL = "bool";
static const ascii_char* KEYWORD_STRUCT = "struct";
static const ascii_char* KEYWORD_RETURN = "return";
static const ascii_char* KEYWORD_WHILE = "while";
static const ascii_char* KEYWORD_BREAK = "break";
static const ascii_char* KEYWORD_CONTINUE = "continue";
static const ascii_char* KEYWORD_ELSE = "else";
static const ascii_char* KEYWORD_ELSE_IF = "else if";
static const ascii_char* KEYWORD_IF = "if";
static const ascii_char* KEYWORD_REF = "ref";
static const ascii_char* KEYWORD_CONST = "const";
static const ascii_char* KEYWORD_IMPORT = "import";

static bool sk_scan_keyword(const ascii_char* Keyword, usize* char_index, ascii_char* source);
static bool sk_lex_is_eof(usize index, usize offset, ascii_char* source);
static ascii_char sk_lex_next_char(usize* index, ascii_char* source);
static ascii_char sk_lex_peek_char(usize* index, usize offset, ascii_char* source);
static ascii_char sk_lex_curr_char(usize* index, ascii_char* source);
static bool sk_lex_empty(usize* line, usize* index, ascii_char* source);
static bool sk_lex_symbols(skToken* token, usize* index, ascii_char* source);
static bool sk_lex_keywords(skToken* token, usize* index, ascii_char* source);
static bool find_char(ascii_char c, ascii_char* arr, usize size);
static	bool sk_lex_termsymbols(skToken* token, usize* index, ascii_char* source);
/* ------------------------------------------------------- */

bool sk_scan_keyword(const ascii_char* Keyword, usize* char_index, ascii_char* source) {
	ascii_char c = sk_lex_curr_char(char_index, source);
	if (c == Keyword[0]) {
		usize index;
		usize length = strlen(Keyword);
		for (index = 1; index < length; ++index) {
			if (sk_lex_peek_char(char_index, index, source) != Keyword[index]) {
				return false;
			}
		}
		for (index = 0; index < length; ++index) {
			sk_lex_next_char(char_index, source);
		}
		--(*char_index);
		return true;
	}
	return false;
}

static bool sk_lex_is_eof(usize index, usize offset, ascii_char* source) {
	return index + offset >= sk_array_length(source);
}
static ascii_char sk_lex_next_char(usize* index, ascii_char* source) {
	if (sk_lex_is_eof(*index, 0, source)) {
		return -1;
	}
	ascii_char c = source[*index];
	++(*index);
	return c;
}
static ascii_char sk_lex_peek_char(usize* index, usize offset, ascii_char* source) {
	if (sk_lex_is_eof(*index, offset, source)) {
		return -1;
	}
	ascii_char c = source[*index + offset];
	return c;
}
static ascii_char sk_lex_curr_char(usize* index, ascii_char* source) {
	if (sk_lex_is_eof(*index, 0, source)) {
		return -1;
	}
	ascii_char c = source[*index];
	return c;
}
static bool sk_lex_empty(usize* line, usize* index, ascii_char* source) {
	ascii_char c = sk_lex_curr_char(index, source);
	if (c == '\n') {
		++(*line);
		sk_lex_next_char(index, source);
		return true;
	}
	if (c == 32 || c == ' ' || c == '\r' || c == '\t') {
		sk_lex_next_char(index, source);
		return true;
	}
	return false;
}
static bool sk_lex_symbols(skToken* token, usize* index, ascii_char* source) {
	ascii_char c = sk_lex_curr_char(index, source);
	if (c == ',') {
		token->type = TOKEN_SYM_COMMA;
		return true;
	} else if (c == '.') {
		token->type = TOKEN_SYM_DOT;
		return true;
	} else if (c == ';') {
		token->type = TOKEN_SYM_SEMICOLON;
		return true;
	} else if (c == '+') {
		token->type = TOKEN_SYM_PLUS;
		return true;
	} else if (c == '-') {
		token->type = TOKEN_SYM_MINUS;
		return true;
	} else if (c == '*') {
		token->type = TOKEN_SYM_ASTERISK;
		return true;
	} else if (c == '/') {
		token->type = TOKEN_SYM_SLASH;
		return true;
	} else if (c == '=') {
		token->type = TOKEN_SYM_EQUAL;
		return true;
	} else if (c == '!') {
		token->type = TOKEN_SYM_EXCLAM;
		return true;
	} else if (c == '(') {
		token->type = TOKEN_SYM_LPAREN;
		return true;
	} else if (c == ')') {
		token->type = TOKEN_SYM_RPAREN;
		return true;
	} else if (c == '{') {
		token->type = TOKEN_SYM_LCURLY;
		return true;
	} else if (c == '}') {
		token->type = TOKEN_SYM_RCURLY;
		return true;
	} else if (c == '[') {
		token->type = TOKEN_SYM_LBRACK;
		return true;
	} else if (c == ']') {
		token->type = TOKEN_SYM_RBRACK;
		return true;
	} else if (c == '%') {
		token->type = TOKEN_SYM_PERCENT;
		return true;
	} else if (c == '&') {
		token->type = TOKEN_SYM_AMPERSAND;
		return true;
	} else if (c == '|') {
		token->type = TOKEN_SYM_OBELISK;
		return true;
	} else if (c == '^') {
		token->type = TOKEN_SYM_CARET;
		return true;
	} else if (c == '<') {
		token->type = TOKEN_SYM_LOWERTHAN;
		return true;
	} else if (c == '>') {
		token->type = TOKEN_SYM_GREATERTHAN;
		return true;
	}
	return false;
}
static bool sk_lex_keywords(skToken* token, usize* index, ascii_char* source) {
	if (sk_scan_keyword(KEYWORD_INT, index, source)) {
		token->type = TOKEN_WORD_INT;
		return true;
	} else if (sk_scan_keyword(KEYWORD_FLOAT, index, source)) {
		token->type = TOKEN_WORD_FLOAT;
		return true;
	} else if (sk_scan_keyword(KEYWORD_VOID, index, source)) {
		token->type = TOKEN_WORD_VOID;
		return true;
	} else if (sk_scan_keyword(KEYWORD_BOOL, index, source)) {
		token->type = TOKEN_WORD_BOOL;
		return true;
	} else if (sk_scan_keyword(KEYWORD_STRUCT, index, source)) {
		token->type = TOKEN_WORD_STRUCT;
		return true;
	} else if (sk_scan_keyword(KEYWORD_RETURN, index, source)) {
		token->type = TOKEN_WORD_RETURN;
		return true;
	} else if (sk_scan_keyword(KEYWORD_WHILE, index, source)) {
		token->type = TOKEN_WORD_WHILE;
		return true;
	} else if (sk_scan_keyword(KEYWORD_BREAK, index, source)) {
		token->type = TOKEN_WORD_BREAK;
		return true;
	} else if (sk_scan_keyword(KEYWORD_CONTINUE, index, source)) {
		token->type = TOKEN_WORD_CONTINUE;
		return true;
	} else if (sk_scan_keyword(KEYWORD_ELSE_IF, index, source)) {
		token->type = TOKEN_WORD_ELSE_IF;
		return true;
	} else if (sk_scan_keyword(KEYWORD_IF, index, source)) {
		token->type = TOKEN_WORD_IF;
		return true;
	} else if (sk_scan_keyword(KEYWORD_ELSE, index, source)) {
		token->type = TOKEN_WORD_ELSE;
		return true;
	} else if (sk_scan_keyword(KEYWORD_REF, index, source)) {
		token->type = TOKEN_WORD_REF;
		return true;
	} else if (sk_scan_keyword(KEYWORD_CONST, index, source)) {
		token->type = TOKEN_WORD_CONST;
		return true;
	} else if (sk_scan_keyword(KEYWORD_IMPORT, index, source)) {
		token->type = TOKEN_WORD_IMPORT;
		return true;
	}
	return false;
}
static bool find_char(ascii_char c, ascii_char* arr, usize size) {
	usize i = 0;
	for (i = 0; i < size; ++i) {
		if (c == arr[i])
			return true;
	}
	return false;
}
static	bool sk_lex_termsymbols(skToken* token, usize* index, ascii_char* source) {
	ascii_char c = sk_lex_curr_char(index, source);
	usize size = strlen(WORD_DIGIT);
	if (find_char(c, WORD_DIGIT, strlen(WORD_DIGIT))) {
		token->type = TOKEN_LIT_INT;
		ascii_char* str = sk_array(sk_gmalloc(), ascii_char, sizeof(ascii_char), 4);
		sk_array_push(str, c);
		sk_lex_next_char(index, source);
		c = sk_lex_curr_char(index, source);
		while (find_char(c, WORD_DIGIT, size) ||
			c == '.') {
			if (c == '.') {
				if (token->type == TOKEN_LIT_INT) {
					token->type = TOKEN_LIT_FLOAT;
					sk_array_push(str, c);
					sk_lex_next_char(index, source);
					c = sk_lex_curr_char(index, source);
				} else {
					sk_emit_error(token->line, "Invalid decimal point.");
				}
			}
			sk_array_push(str, c);
			sk_lex_next_char(index, source);
			c = sk_lex_curr_char(index, source);
		}
		ascii_char zero = 0;
		sk_array_push(str, zero);
		if (token->type == TOKEN_LIT_INT) {
			token->value_int = atoi(str);
		} else if (token->type == TOKEN_LIT_FLOAT) {
			token->value_float = atof(str);
		}
		sk_array_free(str);
		--(*index);
		return true;
	} else if (sk_scan_keyword("true", index, source)) {
		token->type = TOKEN_LIT_BOOL;
		token->value_bool = true;
		return true;
	} else if (sk_scan_keyword("false", index, source)) {
		token->type = TOKEN_LIT_BOOL;
		token->value_bool = false;
		return true;
	} else if (find_char(c, WORD_ALPHA, strlen(WORD_ALPHA))) {
		size = strlen(WORD_ALPHA);
		token->type = TOKEN_IDENT;
		ascii_char* str = sk_array(sk_gmalloc(), ascii_char, sizeof(ascii_char), 4);
		sk_array_push(str, c);
		sk_lex_next_char(index, source);
		c = sk_lex_curr_char(index, source);
		while (find_char(c, WORD_ALPHA, size) ||
			find_char(c, WORD_DIGIT, size)) {
			sk_array_push(str, c);
			sk_lex_next_char(index, source);
			c = sk_lex_curr_char(index, source);
		}
		ascii_char zero = 0;
		sk_array_push(str, zero);
		token->value_ascii_str.array = str;
		token->value_ascii_str.length = sk_array_length(str);
		--(*index);
		return true;
	}
	return false;
}

skToken* sk_lex_source(ascii_char* source, usize source_size) {
	skToken* token_stream = sk_array(sk_lmalloc(), skToken, 256, 4);
	skToken token;
	usize index = 0;
	usize line = 1;
	usize length = sk_array_length(source);
	while (index < length) {
		token.line = line;
		token.type = TOKEN_INVALID;
		if (sk_lex_curr_char(&index, source) == -1) {
			break;
		} else if (sk_lex_empty(&line, &index, source)) continue;
		else if (sk_lex_symbols(&token, &index, source));
		else if (sk_lex_keywords(&token, &index, source));
		else if (sk_lex_termsymbols(&token, &index, source));
		else if (sk_lex_curr_char(&index, source) == 0) {
			token.type = TOKEN_EOF;
		} else {
			ascii_char c = sk_lex_curr_char(&index, source);
			sk_emit_error(line, "Invalid character %c [ASCII = %d ]", c, c);
		}
		if (token.type != TOKEN_INVALID) {
#ifdef TOKEN_DEBUG
			printf("Token Type: %s\n", sk_token_name(token.type));
			if (token.type == TOKEN_IDENT) {
				printf("\tIdent Name: %s\n", token.value_ascii_str.array);
			} else if (token.type == TOKEN_LIT_INT) {
				printf("\tInt Value: %d\n", token.value_int);
			} else if (token.type == TOKEN_LIT_FLOAT) {
				printf("\tFloat Value: %f\n", token.value_float);
			} else if (token.type == TOKEN_LIT_BOOL) {
				printf("\tBoolean Value: %s\n", token.value_bool ? "true" : "false");
			} else if (token.type == TOKEN_LIT_STRING) {
				printf("\tString Value: %s\n", token.value_ascii_str.array);
			}
#endif
			sk_array_push(token_stream, token);
			if (token.type == TOKEN_EOF)
				break;
		}
		sk_lex_next_char(&index, source);
	}
	return	token_stream;
}

void sk_free_token_stream(skToken* token_stream) {
	sk_mem_clear(sk_lmalloc());
}
