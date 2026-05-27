#include "syscalls.h"
#include "rv32i_stdlib.h"

char argv[64];

int process_input(char *buf, int len)
{
    int w = 0;

    for (int r = 0; r < len; r++) {
        char c = buf[r];


        if (c == 0x08 || c == 0x7F) {
            if (w > 0) w--;
        }
        else {
            argv[w++] = c;
        }
    }

    argv[w] = '\0';
    return w;
}

void shell() {
	while (1) {
		char buf[64];
		sys_write( 1, "user:~$ ", 8);	
		int n = sys_read(0, buf, 63);
		buf[n] = '\0';
		process_input( buf, n);
		if (!sys_execve(argv)) sys_write( 1, "\n\rINVALID COMMAND\n\r", 19);
	}	
}
