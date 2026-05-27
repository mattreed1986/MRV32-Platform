.section .text
.globl _start

_start:
    la sp, _kstack_top
    call boot
