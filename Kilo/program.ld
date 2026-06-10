ENTRY(_start)

SECTIONS
{
    . = 12000*4;

    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .sdata : { *(.sdata*) }
    .bss : {
        __bss_start = .;
        *(.sbss*)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    _stack_top = . + 4096;
}




