#include "kernel_types.h"

fd_table global_fd_table;
directory_entry_t dir_table[MAX_FILES];
context_t ctx;
