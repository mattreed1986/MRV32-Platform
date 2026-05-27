#include "kernel_types.h"

extern fd_table global_fd_table;
extern directory_entry_t dir_table[MAX_FILES];


int sysclose(int fd) {

	if (global_fd_table.number[fd]->type == FT_NONE || global_fd_table.number[fd]->type == FT_TTY) return -1;
	
	global_fd_table.number[fd]->entry->size = global_fd_table.number[fd]->position;
	
	global_fd_table.number[fd]->type = FT_NONE;
	
	return 0;
	
}
