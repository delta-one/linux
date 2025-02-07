#ifndef LISTS_CPP_WRAPPER_H_
#define LISTS_CPP_WRAPPER_H_

#include <stddef.h>

#include "cf_defs.h"

size_t cpp_list_count_nodes(struct list_head *head);
void CPP_INIT_LIST_HEAD(struct list_head *list);
void cpp_for_each_sfix(struct sfix_list *list,
		       bool (*callback)(struct sfix_node *, void *usr_args),
		       void *usr_args);
void cpp_list_add_tail(struct list_head *new_, struct list_head *head);


#endif // LISTS_CPP_WRAPPER_H_
