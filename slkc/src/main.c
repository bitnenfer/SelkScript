#include <slkc/load_file.h>
#include <slkc/compiler/lexer.h>
#include <slkc/compiler/parser.h>
#include <slkc/array.h>
#include <stdio.h>

int main(int argc, const char* argv[])
{
	sk_init_memory(SK_SIZE_MB2);
	usize source_size;
	char* source = sk_load_file(sk_gmalloc(), "data/test.slk", &source_size);
	ascii_char* char_arr = sk_array(sk_gmalloc(), ascii_char, source_size + 1, 4);
	ascii_char zero = 0;
	for (usize index = 0; index <  source_size; ++index)
	{
		sk_array_push(char_arr, source[index]);
	}
	sk_array_push(char_arr, zero);
	skToken* token_stream = sk_lex_source(char_arr, source_size);
	skAstNode* ast_root = sk_parse_token_stream(token_stream);
	sk_parser_destroy();
	getchar();
	sk_shutdown_memory();
	return 0;
}