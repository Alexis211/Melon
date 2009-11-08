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
