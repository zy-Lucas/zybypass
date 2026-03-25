#include "symtab.h"
#include <string.h>

namespace hotspot::debugger::aarch64
{
uintptr_t search_symbol(DB *hash_table, uintptr_t base, const char *sym_name, int *sym_size)
{
    DBT key, value;
    int ret;

    // library does not have symbol table
    if (!hash_table)
        return 0;

    key.data = (char *)sym_name;
    key.size = strlen(sym_name) + 1;
    ret = (*hash_table->get)(hash_table, &key, &value, 0);
    if (!ret)
    {
        symtab_symbol *sym = (symtab_symbol *)value.data;
        uintptr_t rslt = (uintptr_t)((char *)base + sym->offset);
        if (sym_size)
            *sym_size = sym->size;
        return rslt;
    }

    return 0;
}
} // namespace hotspot::debugger::aarch64