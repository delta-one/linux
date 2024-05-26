#include "for_each_symbol.h"
#include "internal.h"
#include "expr.h"

/**
 * for_all_symbols_func() - function equivalent to the macro
 *
 * @cb: callback function, called with the symbol of the current iteration and
 * @data that the callee passed
 * @data: used as second argument to @cb
 *
 * Used for embedding the macro into C++ despite the code it expands to not
 * being C++ compatible.
 */
void for_all_symbols_func(void (*cb)(struct symbol *sym, void *data), void *data)
{
	struct symbol *sym;

	for_all_symbols(sym)
		cb(sym, data);
}
