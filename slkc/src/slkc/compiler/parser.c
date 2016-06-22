#include <slkc/compiler/parser.h>
#include <slkc/memory.h>
#include <slkc/array.h>
#include <slkc/error.h>

static skLinearAllocator ast_allocator_struct;
static skAllocator ast_allocator;

skAstNode* sk_make_node(skEAstNodeType node_type) {
	if (node_type == NODE_EOF)
		return NULL;
	skAstNode* node = sk_mem_alloc(ast_allocator, sizeof(skAstNode), 4);
	node->type = node_type;
	node->left = NULL;
	node->right = NULL;
	node->INT32 = 0;
	return node;
}

typedef enum skEBlockType {
	NONE,
	LOOP_BLOCK,
	FUNC_BLOCK,
} skEBlockType;

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
skAstNode* sk_parse_expr_member_access(skToken* token_stream, usize* index);
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
skAstNode* sk_parse_expr_list(skToken* token_stream, usize* index);
skAstNode* sk_parse_expr_type_name(skToken* token_stream, usize* index);
/* Declarations */
skAstNode* sk_parse_decl_const_var(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_var(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_list(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_members(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_struct(skToken* token_stream, usize* index);
skAstNode* sk_parse_decl_function(skToken* token_stream, usize* index);
skAstNode* sk_parse_function_signature(skToken* token_stream, usize* index);
/* Statements */
skAstNode* sk_parse_stmt_return(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_while(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_continue(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_break(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_assign(skToken* token_stream, usize* index);
skAstNode* sk_parse_stmt_else(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_stmt_else_if(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_stmt_if(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_stmt(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_stmt_seq(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_stmt_block(skToken* token_stream, usize* index, skEBlockType block_type);
skAstNode* sk_parse_program(skToken* token_stream, usize* index);

/* Constants */
skAstNode* sk_parse_const_bool(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_BOOL);
	node->BOOL = token.value_bool;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_int(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_INT);
	node->INT32 = token.value_int;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_float(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_FLOAT);
	node->FLOAT32 = token.value_float;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_string(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_STRING);
	node->STRING = token.value_ascii_str.array;
	++(*index);
	return node;
}
skAstNode* sk_parse_const_struct(skToken* token_stream, usize* index) {
	skAstNode* const_struct = sk_make_node(NODE_CONST_STRUCT);
	++*index;
	const_struct->left = sk_parse_expr_list(token_stream, index);
	return const_struct;
}
skAstNode* sk_parse_const_array(skToken* token_stream, usize* index) {
	skAstNode* const_array = sk_make_node(NODE_CONST_ARRAY);
	++*index;
	const_array->left = sk_parse_expr_list(token_stream, index);
	return const_array;
}
skAstNode* sk_parse_const_varname(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* node = sk_make_node(NODE_CONST_VARNAME);
	node->STRING = token.value_ascii_str.array;
	++(*index);
	return node;
}
skAstNode* sk_parse_expr_factor(skToken* token_stream, usize* index) {
	skAstNode* factor = NULL;
	skToken token = token_stream[*index];
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return sk_make_node(NODE_EOF);
	}
	if (token.type == TOKEN_LIT_INT) {
		factor = sk_parse_const_int(token_stream, index);
	} else if (token.type == TOKEN_LIT_FLOAT) {
		factor = sk_parse_const_float(token_stream, index);
	} else if (token.type == TOKEN_LIT_BOOL) {
		factor = sk_parse_const_bool(token_stream, index);
	} else if (token.type == TOKEN_LIT_STRING) {
		factor = sk_parse_const_string(token_stream, index);
	} else if (token.type == TOKEN_IDENT) {
		if (*index + 1 < sk_array_length(token_stream)) {
			if (token_stream[*index + 1].type == TOKEN_SYM_DOT) {
				factor = sk_parse_expr_member_access(token_stream, index);
				return factor;
			}
		}
		factor = sk_parse_const_varname(token_stream, index);
	} else if (token.type == TOKEN_SYM_LCURLY) {
		factor = sk_parse_const_struct(token_stream, index);
		if (*index < sk_array_length(token_stream) &&
			token_stream[*index].type == TOKEN_SYM_RCURLY) {
			++*index;
			return factor;
		} else {
			sk_emit_error(token_stream[*index].line, "Missing }");
		}
	} else if (token.type == TOKEN_SYM_LBRACK) {
		factor = sk_parse_const_array(token_stream, index);
		if (*index < sk_array_length(token_stream) &&
			token_stream[*index].type == TOKEN_SYM_RBRACK) {
			++*index;
			return factor;
		} else {
			sk_emit_error(token_stream[*index].line, "Missing ]");
		}
	} else if (token.type == TOKEN_SYM_LPAREN) {
		++*index;
		factor = sk_parse_expr_or(token_stream, index);
		if (token_stream[*index].type != TOKEN_SYM_RPAREN) {
			sk_emit_error(token_stream[*index].line, "Missing )");
		}
		++*index;
	} else {
		sk_emit_error(token_stream[*index].line, "Invalid Token: %s", sk_token_name(token_stream[*index].type));
	}
	return factor;
}

/* Expressions */
skAstNode* sk_parse_expr_type_name(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	skAstNode* type_name = NULL;
	if (token.type == TOKEN_WORD_INT) {
		++*index;
		type_name = sk_make_node(NODE_TYPE_INT);
	} else if (token.type == TOKEN_WORD_FLOAT) {
		++*index;
		type_name = sk_make_node(NODE_TYPE_FLOAT);
	} else if (token.type == TOKEN_WORD_BOOL) {
		++*index;
		type_name = sk_make_node(NODE_TYPE_BOOL);
	} else if (token.type == TOKEN_WORD_VOID) {
		++*index;
		type_name = sk_make_node(NODE_TYPE_VOID);
	} else if (token.type == TOKEN_IDENT) {
		skAstNode* user_type = sk_make_node(NODE_TYPE_USER);
		user_type->left = sk_parse_const_varname(token_stream, index);
		type_name = user_type;
	} else if (token.type == TOKEN_WORD_REF) {
		skAstNode* ref_type = sk_make_node(NODE_DECL_REFERENCE);
		++*index;
		ref_type->left = sk_parse_expr_type_name(token_stream, index);
		if (ref_type->left == NULL) {
			sk_emit_error(token_stream[*index].type, "Invalid type reference.");
		}
		type_name = ref_type;
	} else if (token.type == TOKEN_WORD_CONST) {
		skAstNode* const_type = sk_make_node(NODE_DECL_CONST);
		++*index;
		const_type->left = sk_parse_expr_type_name(token_stream, index);
		if (const_type->left == NULL) {
			sk_emit_error(token_stream[*index].type, "Invalid type reference.");
		}
		type_name = const_type;
	}
	if (type_name != NULL) {
		if (token_stream[*index].type == TOKEN_SYM_LBRACK) {
			skAstNode* array_type = NULL;
			++*index;
			if (token_stream[*index].type != TOKEN_SYM_RBRACK) {
				array_type = sk_make_node(NODE_DECL_VAR_SIZED_ARRAY);
				array_type->right = sk_parse_expr_or(token_stream, index);
			} else {
				array_type = sk_make_node(NODE_DECL_VAR_NOSIZED_ARRAY);
			}
			array_type->left = type_name;
			if (token_stream[*index].type != TOKEN_SYM_RBRACK) {
				sk_emit_error(token_stream[*index].line, "Missing ]");
			}
			++*index;
			return array_type;
		}
	}
	return type_name;
}

skAstNode* sk_parse_expr_member_access(skToken* token_stream, usize* index) {
	if (*index + 1 < sk_array_length(token_stream)) {
		if ((token_stream[*index].type == TOKEN_IDENT &&
			token_stream[*index + 1].type == TOKEN_SYM_DOT) ||
			(index - 1 > 0 && token_stream[*index - 1].type == TOKEN_SYM_DOT) &&
			token_stream[*index].type == TOKEN_IDENT) {
			skAstNode* member_access = sk_make_node(NODE_EXPR_MEMBER_ACCESS);
			member_access->left = sk_parse_const_varname(token_stream, index);
			if (token_stream[*index].type != TOKEN_SYM_DOT) {
				return member_access;
			}
			++*index;
			member_access->right = sk_parse_expr_factor(token_stream, index);
			if (member_access->right == NULL ||
				member_access->right->type == NODE_EOF) {
				sk_emit_error(token_stream[*index].line, "Invalid Member Access");
			}
			return member_access;
		}
	}
	return NULL;
}

skAstNode* sk_parse_expr_call(skToken* token_stream, usize* index) {
	if (*index + 1 < sk_array_length(token_stream)) {
		skToken token = token_stream[*index];
		if (token.type == TOKEN_IDENT && token_stream[*index + 1].type == TOKEN_SYM_LPAREN) {
			skAstNode* call = sk_make_node(NODE_EXPR_CALL);
			call->left = sk_parse_const_varname(token_stream, index);
			if (token_stream[*index].type != TOKEN_SYM_LPAREN) {
				sk_emit_error(token_stream[*index].line, "Missing (");
			}
			++*index;
			if (token_stream[*index].type == TOKEN_SYM_RPAREN) {
				++*index;
				return call;
			}
			call->right = sk_parse_expr_list(token_stream, index);
			if (*index < sk_array_length(token_stream) &&
				token_stream[*index].type != TOKEN_SYM_RPAREN) {
				sk_emit_error(token_stream[*index].line, "Missing )");
			}
			++*index;
			return call;
		}
	}
	return sk_parse_expr_factor(token_stream, index);
}
skAstNode* sk_parse_expr_negate(skToken* token_stream, usize* index) {
	skToken token = token_stream[*index];
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return sk_make_node(NODE_EOF);
	}
	if (token.type == TOKEN_SYM_MINUS) {
		++*index;
		skAstNode* negate = sk_make_node(NODE_EXPR_NEGATE);
		negate->left = sk_parse_expr_call(token_stream, index);
		if (negate->left = NULL) {
			sk_emit_error(token.line, "Missing Expression");
		}
		return negate;
	}
	return sk_parse_expr_call(token_stream, index);
}
skAstNode* sk_parse_expr_multiplicative(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_negate(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_ASTERISK) {
		expr = sk_make_node(NODE_EXPR_MULTIPLY);
	} else if (token_stream[*index].type == TOKEN_SYM_SLASH) {
		expr = sk_make_node(NODE_EXPR_DIVIDE);
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_multiplicative(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_additive(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_multiplicative(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_PLUS) {
		expr = sk_make_node(NODE_EXPR_ADD);
	} else if (token_stream[*index].type == TOKEN_SYM_MINUS) {
		expr = sk_make_node(NODE_EXPR_SUBTRACT);
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_additive(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_modulus(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_additive(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_PERCENT) {
		expr = sk_make_node(NODE_EXPR_MODULUS);
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_modulus(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_bitwise(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_modulus(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN &&
		token_stream[*index + 1].type == TOKEN_SYM_LOWERTHAN) {
		expr = sk_make_node(NODE_EXPR_BITWISE_SHL);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN &&
		token_stream[*index + 1].type == TOKEN_SYM_GREATERTHAN) {
		expr = sk_make_node(NODE_EXPR_BITWISE_SHR);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_CARET) {
		expr = sk_make_node(NODE_EXPR_BITWISE_XOR);
	} else if (token_stream[*index].type == TOKEN_SYM_OBELISK) {
		expr = sk_make_node(NODE_EXPR_BITWISE_OR);
	} else if (token_stream[*index].type == TOKEN_SYM_AMPERSAND) {
		expr = sk_make_node(NODE_EXPR_BITWISE_AND);
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_bitwise(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_compare(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_bitwise(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_EQUAL &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
		expr = sk_make_node(NODE_EXPR_CMP_EQ);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_EXCLAM &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
		expr = sk_make_node(NODE_EXPR_CMP_NE);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
		expr = sk_make_node(NODE_EXPR_CMP_GTE);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
		expr = sk_make_node(NODE_EXPR_CMP_LTE);
		++*index;
	} else if (token_stream[*index].type == TOKEN_SYM_GREATERTHAN) {
		expr = sk_make_node(NODE_EXPR_CMP_GT);
	} else if (token_stream[*index].type == TOKEN_SYM_LOWERTHAN) {
		expr = sk_make_node(NODE_EXPR_CMP_LT);
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_compare(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_not(skToken* token_stream, usize* index) {
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return sk_make_node(NODE_EOF);
	}
	if (token_stream[*index].type == TOKEN_SYM_EXCLAM &&
		token_stream[*index + 1].type != TOKEN_SYM_EQUAL) {
		++*index;
		skAstNode * not = sk_make_node(NODE_EXPR_NOT);
		not->left = sk_parse_expr_compare(token_stream, index);
		if (not->left == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return not;
	}
	return sk_parse_expr_compare(token_stream, index);
}
skAstNode* sk_parse_expr_and(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_not(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_AMPERSAND &&
		token_stream[*index + 1].type == TOKEN_SYM_AMPERSAND) {
		expr = sk_make_node(NODE_EXPR_LOGIC_AND);
		++*index;
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_and(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}
skAstNode* sk_parse_expr_or(skToken* token_stream, usize* index) {
	skAstNode* expr = NULL;
	skAstNode* lhs = sk_parse_expr_and(token_stream, index);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return lhs;
	}
	if (token_stream[*index].type == TOKEN_SYM_OBELISK &&
		token_stream[*index + 1].type == TOKEN_SYM_OBELISK) {
		expr = sk_make_node(NODE_EXPR_LOGIC_OR);
		++*index;
	}
	if (expr != NULL) {
		++*index;
		expr->left = lhs;
		expr->right = sk_parse_expr_or(token_stream, index);
		if (expr->right == NULL) {
			sk_emit_error(token_stream[*index].line, "Missing expression");
		}
		return expr;
	}
	return lhs;
}

skAstNode* sk_parse_expr_list(skToken* token_stream, usize* index) {
	skAstNode* expr = sk_make_node(NODE_EXPR_LIST);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return expr;
	}
	expr->left = sk_parse_expr_or(token_stream, index);
	if (expr->left == NULL) {
		return NULL;
	}
	if (token_stream[*index].type == TOKEN_SYM_COMMA) {
		++*index;
		expr->right = sk_parse_expr_list(token_stream, index);
	}
	return expr;
}

/* Declarations */
skAstNode* sk_parse_decl_const_var(skToken* token_stream, usize* index) {
	++*index;
	skAstNode* const_decl = sk_make_node(NODE_DECL_CONST);
	const_decl->left = sk_parse_decl_var(token_stream, index);
	return const_decl;
}

skAstNode* sk_parse_decl_var(skToken* token_stream, usize* index) {
	skAstNode* decl_var = sk_make_node(NODE_DECL_VAR);
	decl_var->left = sk_parse_expr_type_name(token_stream, index);
	if (decl_var->left == NULL) {
		sk_emit_error(token_stream[*index].line, "Invalid Type");
	}
	decl_var->right = sk_parse_stmt_assign(token_stream, index);
	if (decl_var->right == NULL) {
		if (token_stream[*index].type == TOKEN_IDENT) {
			decl_var->right = sk_parse_const_varname(token_stream, index);
		} else {
			sk_emit_error(token_stream[*index].line, "Invalid identifier for variable declaration.");
		}
	}
	return decl_var;
}
skAstNode* sk_parse_decl_list(skToken* token_stream, usize* index) {
	skAstNode* decl = sk_make_node(NODE_DECL_VAR_LIST);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return decl;
	}
	skToken token = token_stream[*index];
	if (token.type == TOKEN_WORD_INT ||
		token.type == TOKEN_WORD_FLOAT ||
		token.type == TOKEN_WORD_BOOL ||
		token.type == TOKEN_WORD_VOID ||
		token.type == TOKEN_IDENT ||
		token.type == TOKEN_WORD_REF ||
		token.type == TOKEN_WORD_CONST) {
		decl->left = sk_parse_decl_var(token_stream, index);
	} else {
		return NULL;
	}
	if (decl->left == NULL) {
		return NULL;
	}
	if (token_stream[*index].type == TOKEN_SYM_COMMA) {
		++*index;
		decl->right = sk_parse_decl_list(token_stream, index);
	}
	return decl;
}
skAstNode* sk_parse_decl_members(skToken* token_stream, usize* index) {
	skAstNode* decl = sk_make_node(NODE_DECL_MEMBER_LIST);
	if (token_stream[*index].type == TOKEN_EOF ||
		*index >= sk_array_length(token_stream)) {
		return decl;
	}
	skToken token = token_stream[*index];
	if (token.type == TOKEN_WORD_INT ||
		token.type == TOKEN_WORD_FLOAT ||
		token.type == TOKEN_WORD_BOOL ||
		token.type == TOKEN_WORD_VOID ||
		token.type == TOKEN_IDENT ||
		token.type == TOKEN_WORD_REF ||
		token.type == TOKEN_WORD_CONST) {
		decl->left = sk_parse_decl_var(token_stream, index);
	} else {
		return NULL;
	}
	if (decl->left == NULL) {
		return NULL;
	}
	if (token_stream[*index].type == TOKEN_SYM_SEMICOLON) {
		++*index;
		decl->right = sk_parse_decl_members(token_stream, index);
	}
	return decl;
}

skAstNode* sk_parse_decl_struct(skToken* token_stream, usize* index) {
	++*index;
	skAstNode* decl_struct = sk_make_node(NODE_DECL_STRUCT);
	if (token_stream[*index].type == TOKEN_IDENT) {
		decl_struct->left = sk_parse_const_varname(token_stream, index);
		if (token_stream[*index].type != TOKEN_SYM_LCURLY) {
			sk_emit_error(token_stream[*index].line, "Missing {");
		}
		++*index;
		decl_struct->right = sk_parse_decl_members(token_stream, index);
		if (token_stream[*index].type != TOKEN_SYM_RCURLY) {
			sk_emit_error(token_stream[*index].line, "Missing }");
		}
		++*index;
	} else {
		sk_emit_error(token_stream[*index].line, "Invalid struct identifier.");
	}
	return decl_struct;
}
skAstNode* sk_parse_stmt_block(skToken* token_stream, usize* index, skEBlockType block_type) {
	if (token_stream[*index].type == TOKEN_SYM_LCURLY) {
		++*index;
		if (token_stream[*index].type == TOKEN_SYM_RCURLY) {
			++*index;
			return NULL;
		}
		skAstNode* block = sk_parse_stmt_seq(token_stream, index, block_type);
		if (token_stream[*index].type != TOKEN_SYM_RCURLY) {
			sk_emit_error(token_stream[*index].line, "Missing }");
		}
		++*index;
		return block;
	} else {
		sk_emit_error(token_stream[*index].line, "Missing {");
	}
}

skAstNode* sk_parse_decl_type_varname(skToken* token_stream, usize* index) {
	skAstNode* type_varname_pair = sk_make_node(NODE_DECL_TYPE_VARNAME_PAIR);
	type_varname_pair->left = sk_parse_expr_type_name(token_stream, index);
	type_varname_pair->right = sk_parse_const_varname(token_stream, index);
	return type_varname_pair;
}

skAstNode* sk_parse_function_signature(skToken* token_stream, usize* index) {
	skAstNode* func_sign = sk_make_node(NODE_DECL_FUNCTION_SIGNATURE);
	func_sign->left = sk_parse_decl_type_varname(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_LPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing (");
	}
	++*index;
	if (token_stream[*index].type == TOKEN_SYM_RPAREN) {
		++*index;
		return func_sign;
	}
	func_sign->right = sk_parse_decl_list(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_RPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing )");
	}
	++*index;
	return	func_sign;
}
skAstNode* sk_parse_decl_function(skToken* token_stream, usize* index) {
	skAstNode* decl_func = sk_make_node(NODE_DECL_FUNCTION);
	decl_func->left = sk_parse_function_signature(token_stream, index);
	decl_func->right = sk_parse_stmt_block(token_stream, index, FUNC_BLOCK);
	return decl_func;
}
/* Statements */
skAstNode* sk_parse_stmt_return(skToken* token_stream, usize* index) {
	skAstNode* return_stmt = sk_make_node(NODE_STMT_RETURN);
	++*index;
	return_stmt->left = sk_parse_expr_or(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_SEMICOLON) {
		sk_emit_error(token_stream[*index].line, "Missing ;");
	}
	++*index;
	return return_stmt;
}
skAstNode* sk_parse_stmt_while(skToken* token_stream, usize* index) {
	skAstNode* while_stmt = sk_make_node(NODE_STMT_WHILE);
	++*index;
	if (token_stream[*index].type != TOKEN_SYM_LPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing (");
	}
	++*index;
	while_stmt->left = sk_parse_expr_or(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_RPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing )");
	}
	++*index;
	while_stmt->right = sk_parse_stmt_block(token_stream, index, LOOP_BLOCK);
	return while_stmt;
}
skAstNode* sk_parse_stmt_continue(skToken* token_stream, usize* index) {
	++*index;
	if (token_stream[*index].type != TOKEN_SYM_SEMICOLON) {
		sk_emit_error(token_stream[*index].line, "Missing ;");
	}
	++*index;
	return sk_make_node(NODE_STMT_CONTINUE);
}
skAstNode* sk_parse_stmt_break(skToken* token_stream, usize* index) {
	++*index;
	if (token_stream[*index].type != TOKEN_SYM_SEMICOLON) {
		sk_emit_error(token_stream[*index].line, "Missing ;");
	}
	++*index;
	return sk_make_node(NODE_STMT_BREAK);
}
skAstNode* sk_parse_stmt_assign(skToken* token_stream, usize* index) {
	if (*index + 1 < sk_array_length(token_stream) &&
		token_stream[*index].type == TOKEN_IDENT &&
		token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
		skAstNode* assign = sk_make_node(NODE_STMT_ASSING);
		assign->left = sk_parse_const_varname(token_stream, index);
		if (token_stream[*index].type != TOKEN_SYM_EQUAL) {
			sk_emit_error(token_stream[*index].line, "Invalide assignment");
		}
		++*index;
		assign->right = sk_parse_expr_or(token_stream, index);
		return assign;
	}
	return NULL;
}
skAstNode* sk_parse_stmt_else(skToken* token_stream, usize* index, skEBlockType block_type) {
	skAstNode* else_stmt = sk_make_node(NODE_STMT_ELSE);
	++*index;
	else_stmt->right = sk_parse_stmt_block(token_stream, index, block_type);
	return else_stmt;
}
skAstNode* sk_parse_stmt_else_if(skToken* token_stream, usize* index, skEBlockType block_type) {
	skAstNode* else_if_stmt = sk_make_node(NODE_STMT_ELSE_IF);
	++*index;
	if (token_stream[*index].type != TOKEN_SYM_LPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing (");
	}
	++*index;
	else_if_stmt->left = sk_parse_expr_or(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_RPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing )");
	}
	++*index;
	else_if_stmt->right = sk_parse_stmt_block(token_stream, index, block_type);
	if (token_stream[*index].type == TOKEN_WORD_ELSE_IF) {
		if (else_if_stmt->right != NULL)
			else_if_stmt->right->left = sk_parse_stmt_else_if(token_stream, index, block_type);
		else
			else_if_stmt->right = sk_parse_stmt_else_if(token_stream, index, block_type);
	} else if (token_stream[*index].type == TOKEN_WORD_ELSE) {
		if (else_if_stmt->right != NULL)
			else_if_stmt->right->left = sk_parse_stmt_else(token_stream, index, block_type);
		else
			else_if_stmt->right = sk_parse_stmt_else(token_stream, index, block_type);
	}
	return else_if_stmt;
}
skAstNode* sk_parse_stmt_if(skToken* token_stream, usize* index, skEBlockType block_type) {
	skAstNode* if_stmt = sk_make_node(NODE_STMT_IF);
	++*index;
	if (token_stream[*index].type != TOKEN_SYM_LPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing (");
	}
	++*index;
	if_stmt->left = sk_parse_expr_or(token_stream, index);
	if (token_stream[*index].type != TOKEN_SYM_RPAREN) {
		sk_emit_error(token_stream[*index].line, "Missing )");
	}
	++*index;
	if_stmt->right = sk_parse_stmt_block(token_stream, index, block_type);
	if (token_stream[*index].type == TOKEN_WORD_ELSE_IF) {
		if (if_stmt->right != NULL)
			if_stmt->right->left = sk_parse_stmt_else_if(token_stream, index, block_type);
		else
			if_stmt->right = sk_parse_stmt_else_if(token_stream, index, block_type);
	} else if (token_stream[*index].type == TOKEN_WORD_ELSE) {
		if (if_stmt->right != NULL)
			if_stmt->right->left = sk_parse_stmt_else(token_stream, index, block_type);
		else
			if_stmt->right = sk_parse_stmt_else(token_stream, index, block_type);
	}
	return if_stmt;
}
bool sk_is_typename(skToken token) {
	return (token.type == TOKEN_WORD_VOID ||
		token.type == TOKEN_WORD_INT ||
		token.type == TOKEN_WORD_BOOL ||
		token.type == TOKEN_WORD_FLOAT ||
		token.type == TOKEN_WORD_REF ||
		token.type == TOKEN_IDENT ||
		token.type == TOKEN_WORD_CONST);
}

skAstNode* sk_parse_stmt(skToken* token_stream, usize* index, skEBlockType block_type) {
	skAstNode* stmt = NULL;
	if (*index < sk_array_length(token_stream)) {
		skToken token = token_stream[*index];
		if (block_type != NONE &&
			token.type == TOKEN_WORD_IF) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_if(token_stream, index, block_type);
		} else if (block_type != NONE && 
			token.type == TOKEN_WORD_WHILE) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_while(token_stream, index);
		} else if (token.type == TOKEN_IDENT &&
			*index + 1 < sk_array_length(token_stream) &&
			token_stream[*index + 1].type == TOKEN_SYM_EQUAL) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_assign(token_stream, index);
			if (token_stream[*index].type != TOKEN_SYM_SEMICOLON) {
				sk_emit_error(token_stream[*index].line, "Missing ;");
			}
			++*index;
		} else if (sk_is_typename(token)) {
			usize offset = token.type == TOKEN_WORD_REF || token.type == TOKEN_WORD_CONST ? 3 : 2;
			skToken next = token_stream[*index + 1];
			if (next.type == TOKEN_WORD_REF)
				offset += 1;
			if (*index + offset < sk_array_length(token_stream) &&
				token_stream[*index + offset].type == TOKEN_SYM_LPAREN) {
				stmt = sk_make_node(NODE_STMT);
				stmt->left = sk_parse_decl_function(token_stream, index);
			} else if (*index + offset < sk_array_length(token_stream) &&
				(token_stream[*index + offset].type == TOKEN_SYM_EQUAL ||
				token_stream[*index + offset].type == TOKEN_SYM_SEMICOLON)) {
				stmt = sk_make_node(NODE_STMT);
				stmt->left = sk_parse_decl_var(token_stream, index);
				if (token_stream[*index].type != TOKEN_SYM_SEMICOLON) {
					sk_emit_error(token_stream[*index].line, "Missing ;");
				}
				++*index;
			}
		} else if (block_type == LOOP_BLOCK &&
			token.type == TOKEN_WORD_BREAK) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_break(token_stream, index);
		} else if (block_type == LOOP_BLOCK &&
			token.type == TOKEN_WORD_CONTINUE) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_continue(token_stream, index);
		} else if (block_type == FUNC_BLOCK &&
			token.type == TOKEN_WORD_RETURN) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_stmt_return(token_stream, index);
		} else if (block_type == NONE &&
			token.type == TOKEN_WORD_STRUCT) {
			stmt = sk_make_node(NODE_STMT);
			stmt->left = sk_parse_decl_struct(token_stream, index);
		}
	} else {
		sk_emit_error(0, "Invalid Statement");
	}
	return stmt;
}
skAstNode* sk_parse_stmt_seq(skToken* token_stream, usize* index, skEBlockType block_type) {
	skAstNode* stmt = sk_parse_stmt(token_stream, index, block_type);
	if (stmt != NULL && *index < sk_array_length(token_stream)) {
		stmt->right = sk_parse_stmt_seq(token_stream, index, block_type);
	}
	return stmt;
}
skAstNode* sk_parse_program(skToken* token_stream, usize* index) {
	return sk_parse_stmt_seq(token_stream, index, NONE);
}
skAstNode* sk_parse_token_stream(skToken* token_stream) {
	ast_allocator_struct;
	sk_linear_allocator_init(&ast_allocator_struct, sk_mem_alloc(sk_gmalloc(), SK_SIZE_MB32, 4), SK_SIZE_MB32);
	ast_allocator = sk_linear_allocator(&ast_allocator_struct);
	if (sk_array_length(token_stream) > 0) {
		usize index = 0;
		return sk_parse_program(token_stream, &index);
	} else {
		return sk_make_node(NODE_EOF);
	}
}
void sk_parser_destroy() {
	sk_mem_clear(ast_allocator);
}
