#include <slkc/load_file.h>
#include <slkc/compiler/lexer.h>
#include <slkc/compiler/parser.h>
#include <slkc/compiler/ast_node.h>
#include <slkc/array.h>
#include <stdio.h>

void print_ast_node(skAstNode* node, ascii_char* side, usize deep) {
	if (node != NULL && node->type != NODE_EOF) {
		for (usize index = 0; index < deep; ++index) {
			printf("    ");
		}
		printf("%s", side);
		printf("%s ", sk_ast_node_name(node));
		switch (node->type) {
			case NODE_CONST_BOOL:
				printf("Value = %s\n", node->BOOL ? "true" : "false");
				break;
			case NODE_CONST_INT:
				printf("Value = %d\n", node->INT32);
				break;
			case NODE_CONST_FLOAT:
				printf("Value = %f\n", node->FLOAT32);
				break;
			case NODE_CONST_STRING:
				printf("Value = %s\n", node->STRING);
				break;
			case NODE_CONST_VARNAME:
				printf("Value = %s\n", node->STRING);
				break;
			default:
				printf("\n");
				break;
		}
		print_ast_node(node->left, "Left => ", deep + 1);
		print_ast_node(node->right, "Right => ", deep + 1);
	}
}

int main(int argc, const char* argv[]) {
	sk_init_memory(SK_SIZE_MB2);
	usize source_size;
	char* source = sk_load_file(sk_gmalloc(), "data/test.slk", &source_size);
	ascii_char* char_arr = sk_array(sk_gmalloc(), ascii_char, source_size + 1, 4);
	ascii_char zero = 0;
	for (usize index = 0; index < source_size; ++index) {
		sk_array_push(char_arr, source[index]);
	}
	sk_array_push(char_arr, zero);
	skToken* token_stream = sk_lex_source(char_arr, source_size);
	skAstNode* ast_root = sk_parse_token_stream(token_stream);
	print_ast_node(ast_root, "Root => ", 0);
	sk_parser_destroy();
	getchar();
	sk_shutdown_memory();
	return 0;
}