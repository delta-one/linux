#ifndef FOR_EACH_SYM_PP_H
#define FOR_EACH_SYM_PP_H

#include <functional>
#include "expr.h"

void for_all_symbols_pp(std::function<void(struct symbol *sym)> func);

#endif // FOR_EACH_SYM_PP_H
