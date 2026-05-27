.section .text._start
.globl _start

_start:
    # Set up stack pointer at top of PROG region
    lui   sp, 0x19         # sp = 0x20000
    #addi  sp, sp, 1804    # sp = 0x2000 + 1804 = 9996

    # Call main
    call  main
    
    # Jump to appropriate address
    addi  a7, zero, 2
    ecall
