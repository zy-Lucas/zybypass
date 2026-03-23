#pragma once

#include "symtab.h"
#include <limits.h>
#include <unistd.h>

namespace hotspot::debugger::aarch64
{
#define BUF_SIZE (PATH_MAX + NAME_MAX + 1)

typedef struct lib_info
{
    char name[BUF_SIZE];
    uintptr_t base;
    DB *hash_table;
    int fd;
    struct lib_info *next;
    size_t memsz;
} lib_info;
} // namespace hotspot::debugger::aarch64