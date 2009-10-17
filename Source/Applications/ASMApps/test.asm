%include "syscalls.asm"

start:
 mov ecx, ' '
 loop:
  inc ecx
  mov eax, SC_PUTCH	;temporarily defined for writing one char to screen
  mov ebx, ecx	
  int 64
  mov eax, SC_SLEEP	;temporary syscall for sleeping
  mov ebx, 30			;20ms
  int 64
  cmp ecx, 127
  jnz loop
 mov eax, 0
 mov eax, SC_PUTCH
 mov ebx, 10	;newline
 int 64
 int 66				;finish task
end:
