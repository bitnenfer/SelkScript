#ifndef SLKC_COMPILER_PARSER_H
#define SLKC_COMPILER_PARSER_H

#include <slkc/compiler/token.h>
#include <slkc/compiler/ast_node.h>

skAstNode* sk_parse_token_stream(skToken* token_stream);
void sk_parser_destroy();

#endif /* !SLKC_COMPILER_PARSER_H */