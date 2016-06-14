#include <slkc/list.h>

usize sk_list_count(skList* list)
{
	usize count = 0;
	skList* head = list->head_node;
	for (skList* node = head->next_node; node != head; node = node->next_node)
	{
		++count;
	}
	return count;
}

void sk_list_remove(skList* list)
{
	list->prev_node->next_node = list->next_node;
	list->next_node->prev_node = list->prev_node;
	list->next_node = list;
	list->prev_node = list;
	list->head_node = list;
}

void sk_list_clear(skList* list)
{
	if (list->head_node != list)
	{
		while (list->next_node != list)
			sk_list_remove(list->next_node);
	}
	else
	{
		sk_list_remove(list);
	}
}

void sk_list_insert_before(skList* list, skList* target)
{
	/* remove ourselfs from previous list */
	sk_list_remove(list);
	/* set linking pointers */
	list->next_node = target;
	list->prev_node = target->prev_node;
	target->prev_node = list;
	list->prev_node->next_node = list;
	list->head_node = target->head_node;
}

void sk_list_insert_after(skList* list, skList* target)
{
	/* remove ourselfs from previous list */
	sk_list_remove(list);
	/* set linking pointers */
	list->prev_node = target;
	list->next_node = target->next_node;
	target->next_node = list;
	list->next_node->prev_node= list;
	list->head_node = target->head_node;
}

void sk_list_insert_tail(skList* list, skList* target)
{
	sk_list_insert_before(list, target->head_node);
}

void sk_list_insert_head(skList* list, skList* target)
{
	sk_list_insert_after(list, target->head_node);
}
