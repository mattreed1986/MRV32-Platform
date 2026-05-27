#include "kernel_types.h"

extern fd_table global_fd_table;
extern context_t trap_ctx;
extern int write_handler(const char *buf, int size);

int file_write(int fd, const char *buf, int size) {
	uint8_t *write_address = (uint8_t *)global_fd_table.number[fd]->entry->starting_addr;
	for (int i = 0; i < size; i++) {
		write_address[global_fd_table.number[fd]->position] = buf[i];
		global_fd_table.number[fd]->position++;
	}
	if (global_fd_table.number[fd]->position > global_fd_table.number[fd]->entry->size) global_fd_table.number[fd]->entry->size = global_fd_table.number[fd]->position;
	return size;
}

ssize_t syswrite( int fd, const char *buf, int size) {

	if (fd < 0 || fd >= MAX_FDS) return -1;
	
	fd_table *table = &global_fd_table;
	
	if (table->number[fd]->type == FT_TTY) {
		size = write_handler(buf, size);
		return size;
	}
	else if (table->number[fd]->type == FT_FILE) {
		size = file_write(fd, buf, size);
		return size;
	}
	else return -1;

}
