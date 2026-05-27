ENTRY(boot)

MEMORY {
    RAM (rwx) : ORIGIN = 0x0000, LENGTH = 128K
}

SECTIONS {
    .text : {
        *(.text)
    } > RAM

    .rodata : {
        *(.rodata)
    } > RAM

    .data : {
        *(.data)
    } > RAM

	/* Kernel stack */
	.kernel_stack (NOLOAD) : {
	    . = ALIGN(16);
	    _kstack_bottom = .;
	    . += 1024;
	    _kstack_top = .;
	}
	
	/* User/program stack */
	.user_stack (NOLOAD) : {
	    . = ALIGN(16);
	    _ustack_bottom = .;
	    . += 1024;
	    _ustack_top = .;
	}

    .bss : {
	    _bss_start = .;
	    *(.bss)
	    *(COMMON)	
	    _bss_end = .;
	} > RAM
}
