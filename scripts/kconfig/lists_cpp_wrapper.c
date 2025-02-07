#include "../include/list.h"
#include "cf_defs.h"
#include "lists_cpp_wrapper.h"

size_t cpp_list_count_nodes(struct list_head *head)
{
	return list_count_nodes(head);
}

void CPP_INIT_LIST_HEAD(struct list_head *list)
{
	INIT_LIST_HEAD(list);
}

void cpp_for_each_sfix(struct sfix_list *list,
		       bool (*callback)(struct sfix_node *, void *usr_args),
		       void *usr_args)
{
	struct sfix_node *pos;
	list_for_each_entry(pos, &list->list, node) {
		if (callback(pos, usr_args))
			break;
	}
}

void cpp_list_add_tail(struct list_head *new_, struct list_head *head)
{
	list_add_tail(new_, head);
}
