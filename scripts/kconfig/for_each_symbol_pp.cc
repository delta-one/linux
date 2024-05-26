#include <cassert>
#include <functional>

extern "C" {
#include "expr.h"
#include "for_each_symbol.h"
}

static void for_all_symbols_pp_body(struct symbol *sym, void *data)
{
	auto &func =
		*static_cast<std::function<void(struct symbol *sym)> *>(data);
	func(sym);
}

void for_all_symbols_pp(std::function<void(struct symbol *sym)> func)
{
	for_all_symbols_func(for_all_symbols_pp_body, &func);
}
