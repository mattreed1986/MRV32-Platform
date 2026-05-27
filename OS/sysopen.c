#include "kernel_types.h"
#include "rv32i_stdlib.h"

extern fd_table global_fd_table;
extern directory_entry_t dir_table[MAX_FILES];

int find_next_fd() {
	for( int i = 0; i < MAX_FDS; i++) {
		if (!global_fd_table.number[i]->type) return i;
	}
	return -1;
}

void create_new_entry(char *filename, int j) {
	strcpy(dir_table[j].name, filename);
	uint32_t start = dir_table[j-1].starting_addr + dir_table[j-1].size + 12;
	dir_table[j].starting_addr = start;
	dir_table[j].size = 0;
	dir_table[j].used = 1;
	return;
}

int find_next_entry() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!dir_table[i].used) {
            return i;
        }
    }
    return -1;
}

int dir_search(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (dir_table[i].used &&
            !strcmp(dir_table[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int sysopen(char *filename, int append, int create) {

	int i = dir_search(filename);
	int j;
	if (i == -1 && !create) {
		return -1;
	}
	else if (i == -1 && create) {
		j = find_next_entry();
		if (j < 0) return -1;
		create_new_entry(filename, j);
	}
	if (i == -1) i = j;
	int k = find_next_fd();
	if (k < 0) {
		return -1;
	}
	if (append) {
		global_fd_table.number[k]->position = dir_table[i].size;
	}
	else  { 
		global_fd_table.number[k]->position = 0;
	}
	global_fd_table.number[k]->type = FT_FILE;
	global_fd_table.number[k]->entry = &dir_table[i];
	return k;
	
}
