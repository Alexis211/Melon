[GLOBAL loader]           ; making entry point visible to linker
[EXTERN kmain]            ; kmain is defined in kmain.wtf.cpp
 
; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required
 
section .text
align 4
MultiBootHeader:
   dd MAGIC
   dd FLAGS
   dd CHECKSUM
 
; reserve initial kernel stack space
STACKSIZE equ 0x4000                  ; that's 16k.

extern start_ctors, end_ctors, start_dtors, end_dtors	; these are required for global objects
 
loader:		;here, we load our false GDT, used for having the kernel in higher half
	lgdt [trickgdt]
	mov cx, 0x10;
	mov ds, cx;
	mov es, cx;
	mov fs, cx;
	mov gs, cx;
	mov ss, cx;

	jmp 0x08:higherhalf


higherhalf:		; now we're running in higher half

   mov esp, stack+STACKSIZE           ; set up the stack
   push eax                           ; pass Multiboot magic number
   add ebx, 0xC0000000				  ; update the MB info structure so that it is in the new seg.
   push ebx                           ; pass Multiboot info structure

static_ctors_loop:			; construct global objects
   mov ebx, start_ctors
   jmp .test
.body:
   call [ebx]
   add ebx,4
.test:
   cmp ebx, end_ctors
   jb .body
 
   call  kmain                       ; call kernel proper

   cli		; disable interuptions

static_dtors_loop:					 ; useless, kernel should never return
   mov ebx, start_dtors				; destruct global objects
   jmp .test
.body:
   call [ebx]
   add ebx,4
.test:
   cmp ebx, end_dtors
   jb .body
 
hang:
   hlt                                ; halt machine should kernel return
   jmp   hang

[section .setup]	; this is included in the .setup section, so that it thinks it is at 0x00100000

trickgdt:		; our false GDT
   dw gdt_end - gdt - 1		; gdt limit
   dd gdt					; gdt base

gdt:
   dd 0, 0					; null GDT entry
   db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; kernel code segment
   db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; kernel data segment

gdt_end:
 
[section .bss]
align 32
stack:
   resb STACKSIZE                     ; reserve 16k stack on a quadword boundary
