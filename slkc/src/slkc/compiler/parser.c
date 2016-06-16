#include <slkc/compiler/parser.h>
#include <slkc/memory.h>
#include <slkc/array.h>
#include <slkc/error.h>

static skLinearAllocator ast_allocator_struct;
static skAllocator ast_allocator;

skAstNode* sk_make_node(skEAstNodeType node_type)
{
	if (node_type == NODE_EOF)
		return NULL;
	skAstNode* node = sk_mem_alloc(ast_allocator, sizeof(skAstNode), 4);
	node->type = node_type;
	node->left = NULL;
	node->right = NULL;
	node->INT32 = 0;
	return node;
}

/* Constants */
skAstNode* sk_parse_const_bool(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_int(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_float(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_string(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_struct(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_array(skToken* token_stream, usize* index);
skAstNode* sk_parse_const_varname(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_factor(skToken* token_stream, usize* index);
/* Expressions */
skAstNode* sk_parse_expr_call(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_negate(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_multiplicative(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_additive(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_modulus(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_bitwise(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_compare(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_not(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_and(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_or(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_member_access(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_list(skToken* token_stream, usize* index);
/* Declarations */
skAstNode* sk_parse_decl_var(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_list(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_struct(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_function(skToken* token_stream, usize* index);
/* Statements */
skAstNode* sk_parse_stmt_return(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_while(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_continue(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_break(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_assign(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_else(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_else_if(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_if(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_seq(skToken* token_stream, usize* index);
skAstNode* sk_parse_program(skToken* token_stream, usize* index);

/* Constants */
skAstNode* sk_parse_const_bool(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_BOOL);
	node->BOOL = token.value_bool;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_int(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_INT);
	node->INT32 = token.value_int;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_float(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_FLOAT);
	node->FLOAT32 = token.value_float;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_string(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_STRING);
	node->STRING = token.value_ascii_str.array;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_struct(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_const_array(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_const_varname(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_VARNAME);
	node->STRING = token.value_ascii_str.array;
	++(*index);
	return node;
}
skAstNode* sk_parse_expr_factor(skToken* token_stream, usize* index)
{
	skAstNode* factor = NULL;
	skToken token = token_stream[*index];
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return sk_make_node(NODE_EOF);
	}
	if (token.type == TOKEN_LIT_INT)
	{
		factor = sk_parse_const_int(token_stream, index);
	}
	else if (token.type == TOKEN_LIT_FLOAT)
	{
		factor = sk_parse_const_float(token_stream, index);
	}
	else if (token.type == TOKEN_LIT_BOOL)
	{
		factor = sk_parse_const_bool(token_stream, index);
	}
	else if (token.type == TOKEN_LIT_STRING)
	{
		factor = sk_parse_const_string(token_stream, index);
	}
	else if (token.type == TOKEN_IDENT)
	{
		if (*index + 1 < sk_array_length(token_stream))
		{
			if (token_stream[*index + 1].type == TOKEN_SYM_DOT)
			{
				factor = sk_parse_expr_member_access(token_stream, index);
				return factor;
			}
		}
		factor = sk_parse_const_varname(token_stream, index);
	}
	else if (token.type == TOKEN_SYM_LCURLY)
	{
		factor = sk_parse_const_struct(token_stream, index);
	}
	else if (token.type == TOKEN_SYM_LBRACK)
	{
		factor = sk_parse_const_array(token_stream, index);
	}
	else if (token.type == TOKEN_SYM_LPAREN)
	{
		++*index;
		factor = sk_parse_expr_member_access(token_stream, index);
		if (token_stream[*index].type != TOKEN_SYM_RPAREN)
		{
			sk_emit_error(token_stream[*index].line, "Missing )");
		}
		++*index;
	}
	return factor;
}
/* Expressions */
skAstNode* sk_parse_expr_call(skToken* token_stream, usize* index)
{
	if (*index + 1 < sk_array_length(token_stream))
	{
		skToken token = token_stream[*index];
		if (token.type == TOKEN_IDENT && token_stream[*index + 1].type == TOKEN_SYM_LPAREN)
		{
			skAstNode* call = sk_make_node(NODE_EXPR_CALL);
			call->left = sk_parse_const_varname(token_stream, index);
			if (token_stream[*index].type != TOKEN_SYM_LPAREN)
			{
				sk_emit_error(token_stream[*index].line, "Missing (");
			}
			++*index;
			if (token_stream[*index].type == TOKEN_SYM_RPAREN)
			{
				++*index;
				return call;
			}
			call->right = sk_parse_expr_list(token_stream, index);
			if (*index < sk_array_length(token_stream) &&
				token_stream[*index].type != TOKEN_SYM_RPAREN)
			{
				sk_emit_error(token_stream[*index].line, "Missing )");
			}
			++*index;
			return call;
		}
	}
	return sk_parse_expr_factor(token_stream, index);
}
skAstNode* sk_parse_expr_negate(skToken* token_stream, usize* index)
{
	skToken token = token_stream[*index];
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return sk_make_node(NODE_EOF);
	}
	if (token.type == TOKEN_SYM_MINUS)
	{
		++*index;
		skAstNode* negate = sk_make_node(NODE_EXPR_NEGATE);
		negate->left = sk_parse_expr_call(token_stream, index);
		if (negate->left = NULL)
		{
			sk_emit_error(token.line, "Missing Expression");
		}
		return negate;
	}
	return sk_parse_expr_call(token_stream, index);
}
skAstNode* sk_parse_expr_multiplicative(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_negate(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_ASTERISK)
	{
		expr = sk_make_node(NODE_EXPR_MULTIPLY);
	}
	else if (token_stream[*index].type == TOKEN_SYM_SLASH)
	{
		expr = sk_make_node(NODE_EXPR_DIVIDE);
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_multiplicative(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_additive(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_multiplicative(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_PLUS)
	{
		expr = sk_make_node(NODE_EXPR_ADD);
	}
	else if (token_stream[*index].type == TOKEN_SYM_MINUS)
	{
		expr = sk_make_node(NODE_EXPR_SUBTRACT);
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_additive(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_modulus(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_additive(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_PERCENT)
	{
		expr = sk_make_node(NODE_EXPR_MODULUS);
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_modulus(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_bitwise(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_modulus(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN &&
		token_stream[*index + 1].type == TOKEN_SYM_LOWERTHAN)
	{
		expr = sk_make_node(NODE_EXPR_BITWISE_SHL);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN &&
			 token_stream[*index + 1].type == TOKEN_SYM_GREATERTHAN)
	{
		expr = sk_make_node(NODE_EXPR_BITWISE_SHR);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_CARET)
	{
		expr = sk_make_node(NODE_EXPR_BITWISE_XOR);
	}
	else if (token_stream[*index].type == TOKEN_SYM_OBELISK)
	{
		expr = sk_make_node(NODE_EXPR_BITWISE_OR);
	}
	else if (token_stream[*index].type == TOKEN_SYM_AMPERSAND)
	{
		expr = sk_make_node(NODE_EXPR_BITWISE_AND);
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_bitwise(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_compare(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_bitwise(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_EQUAL &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL)
	{
		expr = sk_make_node(NODE_EXPR_CMP_EQ);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_EXCLAM &&
			 token_stream[*index + 1].type == TOKEN_SYM_EQUAL)
	{
		expr = sk_make_node(NODE_EXPR_CMP_NE);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN &&
			 token_stream[*index + 1].type == TOKEN_SYM_EQUAL)
	{
		expr = sk_make_node(NODE_EXPR_CMP_GTE);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN &&
			 token_stream[*index + 1].type == TOKEN_SYM_EQUAL)
	{
		expr = sk_make_node(NODE_EXPR_CMP_LTE);
		++*index;
	}
	else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN)
	{
		expr = sk_make_node(NODE_EXPR_CMP_GT);
	}
	else if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN)
	{
		expr = sk_make_node(NODE_EXPR_CMP_LT);
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_compare(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_not(skToken* token_stream, usize* index)
{
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return sk_make_node(NODE_EOF);
	}
	if (token_stream[*index].type == TOKEN_SYM_EXCLAM &&
		token_stream[*index + 1].type != TOKEN_SYM_EQUAL)
	{
		++*index;
		skAstNode * not = sk_make_node(NODE_EXPR_NOT);
		not->left = sk_parse_expr_compare(token_stream, index);
		if (not->left == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return not;
	}
	return sk_parse_expr_compare(token_stream, index);
}
skAstNode* sk_parse_expr_and(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_not(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_AMPERSAND &&
		token_stream[*index + 1].type == TOKEN_SYM_AMPERSAND)
	{
		expr = sk_make_node(NODE_EXPR_LOGIC_AND);
		++*index;
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_and(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_or(skToken* token_stream, usize* index)
{
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_and(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_OBELISK &&
		token_stream[*index + 1].type == TOKEN_SYM_OBELISK)
	{
		expr = sk_make_node(NODE_EXPR_LOGIC_OR);
		++*index;
	}
	if (expr != NULL)
	{
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_or(token_stream, index);
		if (expr->right == NULL)
		{
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_member_access(skToken* token_stream, usize* index)
{
	// TODO: Implement Member Access.
	return sk_parse_expr_or(token_stream, index);
}
skAstNode* sk_parse_expr_list(skToken* token_stream, usize* index)
{
	skAstNode* expr = sk_make_node(NODE_EXPR_LIST);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream))
	{
		return expr;
	}
	expr->left = sk_parse_expr_member_access(token_stream, index);
	if (expr->left == NULL)
	{
		return NULL;
	}
	if (token_stream[*index].type == TOKEN_SYM_COMMA)
	{
		++*index;
		expr->right = sk_parse_expr_list(token_stream, index);
	}
	return expr;
}
/* Declarations */
skAstNode* sk_parse_decl_var(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_decl_list(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_decl_struct(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_decl_function(skToken* token_stream, usize* index)
{
	return NULL;
}
/* Statements */
skAstNode* sk_parse_stmt_return(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_while(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_continue(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_break(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_assign(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_else(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_else_if(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_if(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_stmt_seq(skToken* token_stream, usize* index)
{
	return NULL;
}
skAstNode* sk_parse_program(skToken* token_stream, usize* index)
{
	return sk_parse_expr_member_access(token_stream, index);
}


skAstNode* sk_parse_token_stream(skToken* token_stream)
{
	ast_allocator_struct;
	sk_linear_allocator_init(&ast_allocator_struct, sk_mem_alloc(sk_gmalloc(), SK_SIZE_MB32, 4), SK_SIZE_MB32);
	ast_allocator = sk_linear_allocator(&ast_allocator_struct);
	if (sk_array_length(token_stream) > 0)
	{
		usize index = 0;
		return sk_parse_program(token_stream, &index);
	}
	else
	{
		return sk_make_node(NODE_EOF);
	}
}

void sk_parser_destroy()
{
	sk_mem_clear(ast_allocator);
}
