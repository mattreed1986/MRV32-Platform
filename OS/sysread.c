#include "kernel_types.h"

extern fd_table global_fd_table;
extern context_t trap_ctx;
extern int read_handler(char *buf, int size, int flag);

static inline void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
    return dst;
}

int file_read(int fd, char *buf, int count) {
	uint8_t *read_address = (uint8_t *)global_fd_table.number[fd]->entry->starting_addr;
	int i;
	for (i = 0; i < count; i++) {
		if (global_fd_table.number[fd]->position == global_fd_table.number[fd]->entry->size) break;
		buf[i] = read_address[global_fd_table.number[fd]->position];
		global_fd_table.number[fd]->position++;
	}
	return i;
}

ssize_t sysread(int fd, char *buf, int count, int raw_flag) {
	
	if (fd < 0 || fd >= MAX_FDS) return -1;
	
	fd_table *table = &global_fd_table;
	
	if (table->number[fd]->type == FT_TTY) {
		return read_handler( buf, count, raw_flag);
	}
	else if (table->number[fd]->type == FT_FILE) return file_read(fd, buf, count);
	else return -1;
	
}


