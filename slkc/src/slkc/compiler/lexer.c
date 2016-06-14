#include <slkc/compiler/lexer.h>
#include <slkc/array.h>

skToken* sk_lex_source(ascii_char* source, usize source_size)
{
	skToken* token_stream = sk_array(sk_gmalloc(), skToken, 256, 4);
	return token_stream;
}

void sk_free_token_stream(skToken* token_stream)
{
	sk_array_free(token_stream);
}
