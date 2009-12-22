[GLOBAL read_eip]
read_eip:
	pop eax
	jmp eax

[GLOBAL idle_task]
idle_task:
	sti
	hlt
	jmp idle_task

