ENTRY (start)

SECTIONS{
    . = 0x10000000;

    .text : {
        *(.text)
    }

    .rodata ALIGN (0x1000)  :{
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
		start_ctors = .;
		*(.ctor*)
		end_ctors = .;
		start_dtors = .;
		*(.dtor*)
		end_dtors = .;
        *(.data)
    }

    .bss : {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }

	end = .; _end = .; __end = .;
}
