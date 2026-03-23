#pragma once

#include <search.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include <db.h>
#include <fcntl.h>
#include <mach/machine.h>

namespace hotspot::debugger::aarch64
{
typedef struct symtab_symbol
{
    char *name;       // name like __ZThread_...
    uintptr_t offset; // to loaded address
    uintptr_t size;   // size strlen
} symtab_symbol;

uintptr_t search_symbol(DB *hash_table, uintptr_t base, const char *sym_name, int *sym_size);
} // namespace hotspot::debugger::aarch64