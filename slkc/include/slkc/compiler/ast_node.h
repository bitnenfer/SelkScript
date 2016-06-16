#ifndef SLKC_COMPILER_AST_NODE_H
#define SLKC_COMPILER_AST_NODE_H

#include <slkc/types.h>

typedef enum skEAstNodeType
{
	NODE_TYPE_NAME,
	NODE_CONST_BOOL,
	NODE_CONST_INT,
	NODE_CONST_FLOAT,
	NODE_CONST_STRING,
	NODE_CONST_STRUCT,
	NODE_CONST_ARRAY,
	NODE_CONST_VARNAME,
	NODE_EXPR_CALL,
	NODE_EXPR_NEGATE,
	NODE_EXPR_MULTIPLY,
	NODE_EXPR_DIVIDE,
	NODE_EXPR_ADD,
	NODE_EXPR_SUBTRACT,
	NODE_EXPR_MODULUS,
	NODE_EXPR_BITWISE_SHL,
	NODE_EXPR_BITWISE_SHR,
	NODE_EXPR_BITWISE_XOR,
	NODE_EXPR_BITWISE_OR,
	NODE_EXPR_BITWISE_AND,
	NODE_EXPR_CMP_EQ,
	NODE_EXPR_CMP_GT,
	NODE_EXPR_CMP_LT,
	NODE_EXPR_CMP_NE,
	NODE_EXPR_CMP_GTE,
	NODE_EXPR_CMP_LTE,
	NODE_EXPR_NOT,
	NODE_EXPR_LOGIC_AND,
	NODE_EXPR_LOGIC_OR,
	NODE_EXPR_MEMBER_ACCESS,
	NODE_EXPR_LIST,
	NODE_DECL_VAR,
	NODE_DECL_VAR_LIST,
	NODE_DECL_STRUCT,
	NODE_DECL_FUNCTION,
	NODE_STMT_RETURN,
	NODE_STMT_WHILE,
	NODE_STMT_CONTINUE,
	NODE_STMT_BREAK,
	NODE_STMT_ASSING,
	NODE_STMT_ELSE,
	NODE_STMT_ELSE_IF,
	NODE_STMT_IF,
	NODE_STMT_SEQ,
	NODE_PROGRAM,
	NODE_EOF
} skEAstNodeType;


struct skAstNode;
typedef struct skStructType
{
	usize ID;
	struct skAstNode* body;
} skStructType;

typedef struct skAstNode
{
	skEAstNodeType type;
	struct skAstNode* left;
	struct skAstNode* right;
	usize line;
	union
	{
		int32 INT32;
		float32 FLOAT32;
		bool BOOL;
		ascii_char* STRING;

	};
} skAstNode;

SK_FORCEINLINE ascii_char* sk_ast_node_name(skAstNode* node)
{
	skEAstNodeType type = node->type;
	switch (type)
	{
		case NODE_TYPE_NAME:
			return "NODE_TYPE_NAME";
		case NODE_CONST_BOOL:
			return "NODE_CONST_BOOL";
		case NODE_CONST_INT:
			return "NODE_CONST_INT";
		case NODE_CONST_FLOAT:
			return "NODE_CONST_FLOAT";
		case NODE_CONST_STRING:
			return "NODE_CONST_STRING";
		case NODE_CONST_STRUCT:
			return "NODE_CONST_STRUCT";
		case NODE_CONST_ARRAY:
			return "NODE_CONST_ARRAY";
		case NODE_CONST_VARNAME:
			return "NODE_CONST_VARNAME";
		case NODE_EXPR_CALL:
			return "NODE_EXPR_CALL";
		case NODE_EXPR_NEGATE:
			return "NODE_EXPR_NEGATE";
		case NODE_EXPR_MULTIPLY:
			return "NODE_EXPR_MULTIPLY";
		case NODE_EXPR_DIVIDE:
			return "NODE_EXPR_DIVIDE";
		case NODE_EXPR_ADD:
			return "NODE_EXPR_ADD";
		case NODE_EXPR_SUBTRACT:
			return "NODE_EXPR_SUBTRACT";
		case NODE_EXPR_MODULUS:
			return "NODE_EXPR_MODULUS";
		case NODE_EXPR_BITWISE_SHL:
			return "NODE_EXPR_BITWISE_SHL";
		case NODE_EXPR_BITWISE_SHR:
			return "NODE_EXPR_BITWISE_SHR";
		case NODE_EXPR_BITWISE_XOR:
			return "NODE_EXPR_BITWISE_XOR";
		case NODE_EXPR_BITWISE_OR:
			return "NODE_EXPR_BITWISE_OR";
		case NODE_EXPR_BITWISE_AND:
			return "NODE_EXPR_BITWISE_AND";
		case NODE_EXPR_CMP_EQ:
			return "NODE_EXPR_CMP_EQ";
		case NODE_EXPR_CMP_GT:
			return "NODE_EXPR_CMP_GT";
		case NODE_EXPR_CMP_LT:
			return "NODE_EXPR_CMP_LT";
		case NODE_EXPR_CMP_NE:
			return "NODE_EXPR_CMP_NE";
		case NODE_EXPR_CMP_GTE:
			return "NODE_EXPR_CMP_GTE";
		case NODE_EXPR_CMP_LTE:
			return "NODE_EXPR_CMP_LTE";
		case NODE_EXPR_NOT:
			return "NODE_EXPR_NOT";
		case NODE_EXPR_LOGIC_AND:
			return "NODE_EXPR_LOGIC_AND";
		case NODE_EXPR_LOGIC_OR:
			return "NODE_EXPR_LOGIC_OR";
		case NODE_EXPR_MEMBER_ACCESS:
			return "NODE_EXPR_MEMBER_ACCESS";
		case NODE_EXPR_LIST:
			return "NODE_EXPR_LIST";
		case NODE_DECL_VAR:
			return "NODE_DECL_VAR";
		case NODE_DECL_VAR_LIST:
			return "NODE_DECL_VAR_LIST";
		case NODE_DECL_STRUCT:
			return "NODE_DECL_STRUCT";
		case NODE_DECL_FUNCTION:
			return "NODE_DECL_FUNCTION";
		case NODE_STMT_RETURN:
			return "NODE_STMT_RETURN";
		case NODE_STMT_WHILE:
			return "NODE_STMT_WHILE";
		case NODE_STMT_CONTINUE:
			return "NODE_STMT_CONTINUE";
		case NODE_STMT_BREAK:
			return "NODE_STMT_BREAK";
		case NODE_STMT_ASSING:
			return "NODE_STMT_ASSING";
		case NODE_STMT_ELSE:
			return "NODE_STMT_ELSE";
		case NODE_STMT_ELSE_IF:
			return "NODE_STMT_ELSE_IF";
		case NODE_STMT_IF:
			return "NODE_STMT_IF";
		case NODE_STMT_SEQ:
			return "NODE_STMT_SEQ";
		case NODE_PROGRAM:
			return "NODE_PROGRAM";
		case NODE_EOF:
			return "NODE_EOF";
		default:
			return "INVALID";
			break;
	}
}

#endif /* !SLKC_COMPILER_AST_NODE_H */