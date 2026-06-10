.section .text._start
.globl _start

_start:
    # Set up stack pointer at top of PROG region
    lui   sp, 0x19         # sp = 0x20000
    #addi  sp, sp, 1804     # sp = 0x2070C

    # ── Zero BSS section ──────────────────────────
    # Every uninitialized/zero-initialized global and static
    # variable lives here. Without this loop, _heap_inited,
    # _heap_head, the editor config E, etc. contain garbage.
    la    t0, __bss_start
    la    t1, __bss_end
1:  bge   t0, t1, 2f
    sw    zero, 0(t0)
    addi  t0, t0, 4
    j     1b
2:

    # Call main
    call  main

    # If main returns, exit cleanly
    li    a7, 2            # syscall 2 = exit
    ecall
