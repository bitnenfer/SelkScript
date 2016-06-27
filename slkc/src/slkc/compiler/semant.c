#include <slkc/compiler/semant.h>
#include <slkc/memory.h>
#include <slkc/array.h>
#include <slkc/list.h>
#include <string.h>

#define MAX_VARIABLES 256

/*TODO: Implement Heap Allocator*/
static skMallocAllocator scope_allocator_struct;
static skAllocator scope_allocator;

static skMallocAllocator variable_allocator_struct;
static skAllocator variable_allocator;

typedef struct skKeyValuePair {
	union {
		float32 float_value;
		int32 int_value;
		bool bool_type;
		ascii_char* string_value;
		struct skKeyValuePair* pair_value;
	};
	int32 hash;
} skKeyValuePair;

typedef struct skScopeData {
	skKeyValuePair* variables;
	skList scope_list;
} skScopeData;

void sk_init_scope(skScopeData* scope) {
	scope->scope_list.owner = scope;
	scope->variables = sk_array(variable_allocator, skKeyValuePair, MAX_VARIABLES, 4);
}

void sk_push_scope(skScopeData* root) {
	skScopeData* scope = sk_mem_alloc(scope_allocator, sizeof(skScopeData), 4);
	sk_init_scope(scope);
	sk_list_insert_tail(&scope->scope_list, &root->scope_list);
}

void sk_pop_scope(skScopeData* root) {
	skScopeData* tail = sk_list_tail(&root->scope_list)->owner;
	sk_array_free(tail->variables);
	sk_mem_free(scope_allocator, tail);
	sk_list_remove(tail);
}

int32 sk_hash(ascii_char* key, int32 max) {
	int32 hash;
	int32 index = 0;
	usize len = strlen(key);
	while (hash < INTMAX_MAX &&
		index < len) {
		hash = hash << 8;
		hash = hash + key[index];
		++index;
	}
	return hash % max;
}

#define sk_push_variable(key, type, value) { \
}