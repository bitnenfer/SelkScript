#ifndef SLKC_COMPILER_LEXER_H
#define SLKC_COMPILER_LEXER_H

#include <slkc/types.h>
#include <slkc/compiler/token.h>

skToken* sk_lex_source(ascii_char* source, usize source_size);
void sk_free_token_stream(skToken* token_stream);

#endif /* !SLKC_COMPILER_LEXER_H */