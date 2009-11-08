[BITS 16]

[GLOBAL v86test]
v86test:
	dw v86test_end - v86test_start	
v86test_start:
	mov ax, 0013h
	int 10h
	int 3
v86test_end:
	db 0
