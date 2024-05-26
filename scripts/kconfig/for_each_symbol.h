#ifndef FOR_EACH_SYMBOL_H
#define FOR_EACH_SYMBOL_H

#include "expr.h"

void for_all_symbols_func(void (*cb)(struct symbol *sym, void *data), void
		*data);

#endif // FOR_EACH_SYMBOL_H
