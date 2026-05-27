#include "kernel_types.h"

extern void back_to_shell();

ssize_t sysexit() {

	back_to_shell();

}
