; This is the procedure that is first run when starting a V86 thread
; Parameters : esp+4 = page directory to switch to, esp+8 = v86_regs_t pointer
; Algorithm :
; - Switch page directory
; - Create a false IRET structure, with : (first pushed first)
;   GS, FS, DS, ES, SS, SP, EFLAGS (with eflags.if and eflags.vm), CS, IP
; - Set general purpose regisers : ax bx cx dx di si
; - Call iret, switch to V86 mode.

[BITS 32]

[GLOBAL v86_run]
v86_run:
	mov eax, [esp + 4]
	mov cr3, eax
	mov eax, [esp + 8]

	xor ebx, ebx
	mov bx, [eax + 20]		; get GS
	push ebx
	mov bx, [eax + 18]		; get FS
	push ebx
	mov bx, [eax + 14]		; get DS
	push ebx
	mov bx, [eax + 16]		; get ES
	push ebx
	mov bx, [eax + 22]		; get SS
	push ebx
	mov bx, [eax + 26]		; get SP
	push ebx
	pushf
	pop ebx
	or ebx, 20200h
	push ebx
	xor ebx, ebx
	mov bx, [eax + 12]		; get CS
	push ebx
	mov bx, [eax + 24]		; get IP
	push ebx

	mov bx, [eax + 2]
	mov cx, [eax + 4]
	mov dx, [eax + 6]
	mov di, [eax + 8]
	mov si, [eax + 10]
	mov ax, [eax]

	iret
