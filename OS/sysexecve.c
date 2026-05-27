#include "kernel_types.h"

extern directory_entry_t dir_table[MAX_FILES];

static inline int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return *(const uint8_t *)a - *(const uint8_t *)b;
}

void return_from_trap() {
    __asm__ volatile ("sret");
}

int parse_argv(char *input, char arg_storage[5][64], char *argv_array[5]) {
    int argc = 0;

    while (*input && argc < 5) {

        while (*input == ' ') input++;

        if (*input == '\0') break;

        argv_array[argc] = arg_storage[argc];

        int j = 0;
        while (*input != '\0' && *input != ' ' && j < 63) {
            arg_storage[argc][j++] = *input++;
        }
        arg_storage[argc][j] = 0;

        argc++;
    }
    if (argc < 5) {
    	argv_array[argc] = 0;
    }
    return argc;
}

int find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (dir_table[i].used &&
            !strcmp(dir_table[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void set_sepc(unsigned long addr) {
    asm volatile(
        ".insn i 0x73, 0x1, x0, %0, 0x141"
        :
        : "r"(addr)
    );
}

int sysexecve(char *argv) {

    char arg_storage[5][64];
    char *argv_array[5];
    
    int argc = parse_argv(argv, arg_storage, argv_array);
    argv_array[argc] = 0;
    
    int idx = find_file(argv_array[0]);
    if (idx < 0) {
    	return 0;
    }
    
    set_sepc(dir_table[idx].starting_addr);      

    asm volatile ("mv a0, %0" : : "r" (argc));
    asm volatile ("mv a1, %0" : : "r" (argv_array));
    return_from_trap();
}
