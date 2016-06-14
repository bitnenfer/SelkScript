#ifndef SLKC_LIST_H
#define SLKC_LIST_H

#include <slkc/types.h>

typedef struct skList
{
	struct skList* prev_node;
	struct skList* next_node;
	struct skList* head_node;
	void* owner;
} skList;

usize sk_list_count(skList* list);
void sk_list_remove(skList* list);
void sk_list_clear(skList* list);
void sk_list_insert_before(skList* list, skList* node);
void sk_list_insert_after(skList* list, skList* node);
void sk_list_insert_tail(skList* list, skList* target);
void sk_list_insert_head(skList* list, skList* target);

#define sk_list_init(list, owner) {\
	list->owner = (void*)owner;\
	}

#define sk_list_set_owner(list, owner) {list->owner = (void*)owner;}
#define sk_list_get_owner(list, type) ((type*)list->owner)

#endif /* !SLKC_LIST_H */