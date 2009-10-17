[GLOBAL read_eip]
read_eip:
	pop eax
	jmp eax

[GLOBAL idle_task]
idle_task:
	sti
	hlt
	jmp idle_task

[GLOBAL atomic_exchange]
atomic_exchange:
   mov ecx, [esp+4]		; Get lock address
   mov eax, [esp+8]		; Get new value
   xchg eax, [ecx]		; Old value goes in eax
   ret

[GLOBAL copy_page_physical]
copy_page_physical:
   push ebx              ; According to __cdecl, we must preserve the contents of EBX.
   pushf                 ; push EFLAGS, so we can pop it and reenable interrupts
                         ; later, if they were enabled anyway.
   cli                   ; Disable interrupts, so we aren't interrupted.
                         ; Load these in BEFORE we disable paging!
   mov ebx, [esp+12]     ; Source address
   mov ecx, [esp+16]     ; Destination address

   mov edx, cr0          ; Get the control register...
   and edx, 0x7fffffff   ; and...
   mov cr0, edx          ; Disable paging.

   mov edx, 1024         ; 1024*4bytes = 4096 bytes to copy

.loop:
   mov eax, [ebx]        ; Get the word at the source address
   mov [ecx], eax        ; Store it at the dest address
   add ebx, 4            ; Source address += sizeof(word)
   add ecx, 4            ; Dest address += sizeof(word)
   dec edx               ; One less word to do
   jnz .loop

   mov edx, cr0          ; Get the control register again
   or  edx, 0x80000000   ; and...
   mov cr0, edx          ; Enable paging.

   popf                  ; Pop EFLAGS back.
   pop ebx               ; Get the original value of EBX back.
   ret


[GLOBAL sample_task]
sample_task:
	mov eax, 0x00000001	;temporarily defined as syscall id for writing one char to screen
	mov ebx, 'a'		;loopingly write a's to screen
	int 64
	mov eax, 0x00000002	;temporary syscall for sleeping
	mov ebx, 20			;20ms
	int 64
	jmp sample_task
	int 66				;finish task - will never happen since we have an infinite loop
[GLOBAL sample_task_size]
sample_task_size:
	dd sample_task_size - sample_task
