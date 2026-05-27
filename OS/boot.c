#include "kernel_types.h"

extern void trap_entry();
extern void shell();
extern char _bss_start;
extern char _bss_end;
extern fd_table global_fd_table;
extern directory_entry_t dir_table[MAX_FILES];
static file_t fd_entries[MAX_FDS];

int find_free_entry() {
    	for (int i = 0; i < MAX_FILES; i++) {
        	if (!dir_table[i].used) {
            		return i;
        	}
    	}
    	return -1;
}

int create_file(const char *name, uint32_t start, uint32_t size) {
    	int idx = find_free_entry();
    	if (idx < 0) return -1;

    	int i = 0;
    	while (i < MAX_FILENAME - 1 && name[i]) {
        	dir_table[idx].name[i] = name[i];
        	i++;
   	}
    	dir_table[idx].name[i] = 0;

    	dir_table[idx].starting_addr = start;
    	dir_table[idx].size = size;
    	dir_table[idx].used = 1;

    	return idx;
}

void boot() {
    	char *p = &_bss_start;
    	while (p < &_bss_end) *p++ = 0;
    
    
    	for (int i = 0; i < MAX_FDS; i++) {
        	global_fd_table.number[i] = &fd_entries[i];
    	}    
    
    	global_fd_table.number[0]->type = FT_TTY;  // stdin
    	global_fd_table.number[1]->type = FT_TTY;  // stdout
   	global_fd_table.number[2]->type = FT_TTY;  // stderr

    	create_file( "Basic", 8000, 38696);
    	create_file( "Kilo", 48000, 48000);
   
    	shell();
}
