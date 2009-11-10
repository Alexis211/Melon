[BITS 16]

[GLOBAL setvgamode]
setvgamode:
	dw setvgamode_end - setvgamode_start	
setvgamode_start:
	int 60
	int 10h
	int 3
setvgamode_end:
	db 0

[GLOBAL vesa_int]
vesa_int:
	dw vesa_int_end - vesa_int_start
vesa_int_start:
	int 60
	mov es, si
	int 10h
	int 3
vesa_int_end:
	db 0
