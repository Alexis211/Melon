#include "V86Thread.class.h"
#include <TaskManager/Task.ns.h>

#include <TaskManager/V86/V86.ns.h>

void V86Thread::runV86(V86Thread* thread, u32int data, u32int ss, u32int cs) {
	thread->m_process->getPagedir()->switchTo();
	//Setup values on user stack
	u32int *stack = (u32int*)(FP_TO_LINEAR(ss, V86_STACKSIZE));
	stack--; *stack = data;

	u32int sp = V86_STACKSIZE - 4;

	//Setup a false iret structure on the kernel stack, containing (first pushed first) :
	// - gs = cs
	// - fs = cs
	// - ds = cs
	// - es = cs
	// - stack segment = ss (temporarily in ecx)
	// - stack pointer = sp (temp in ebx)
	// - flags (OR'ed with EFLAGS.VM)
	// - code segment = cs (temp in eax)
	// - instruction pointer = ip (is 0)
	asm volatile("			\
			pushl %%eax;	\
			pushl %%eax;	\
			pushl %%eax;	\
			pushl %%eax;	\
			pushl %%ecx;	\
			pushl %%ebx;	\
			pushf;			\
			pop %%ebx;		\
			or $0x200, %%ebx;	\
			or $0x20000, %%ebx;	\
			push %%ebx;		\
			pushl %%eax;	\
			pushl $0;		\
			iret;			\
			" : : "a"(cs), "c"(ss), "b"(sp));
}

/*
 *	Set up a V86 task :
 *	Allocate space for the kernel stack
 *	Map frames in lower 1MB : IVT (first page), BDA (0xA0000 to 0xFFFFF)
 *  Find somewhere to put the stack and the code, still in lower 1MB
 *  Map that space, and copy the 16bit code to that place
 *  Setup values on the kernel stack for starting the thread (V86Thread::runV86),
 *  giving it entry point and stack location
 */

V86Thread::V86Thread(v86_function_t* entry, v86_retval_t* ret, u32int data) : Thread() {
	m_ret = ret; m_ret->finished = false;
	m_xchgspace = 0;
	m_isKernel = true;
	m_if = true;
	m_process = Task::currProcess();
	m_kernelStack.addr = Mem::alloc(STACKSIZE);
	m_kernelStack.size = STACKSIZE;	

	m_process->getPagedir()->switchTo();

	//Map all lower memory
	V86::map();

	u16int cs = V86::allocSeg(entry->size);	//Alocate segments for the code to run in
	u8int* codeptr = (u8int*)(FP_TO_LINEAR(cs, 0));
	memcpy(codeptr, entry->data, entry->size);	//Copy the code there

	u16int ss = V86::allocSeg(V86_STACKSIZE);

	u32int* stack = (u32int*)((u32int)m_kernelStack.addr + m_kernelStack.size);
	stack--; *stack = cs;	//Pass code segment (ip = 0)
	stack--; *stack = ss;	//Pass stack segment (sp = V86_STACKSIZE)
	stack--; *stack = data;	//Pass data for thread
	stack--; *stack = (u32int)this;	//Pass thread pointer
	stack--; *stack = 0;
	m_esp = (u32int)stack;
	m_ebp = m_esp + 8;
	m_eip = (u32int)runV86;

	m_state = T_RUNNING;
	m_process->registerThread(this);
	Task::registerThread(this);
}

bool V86Thread::handleV86GPF(registers_t *regs) {
	u8int* ip = (u8int*)FP_TO_LINEAR(regs->cs, regs->eip);
	u16int *ivt = 0;
	u16int *stack = (u16int*)FP_TO_LINEAR(regs->ss, (regs->useresp & 0xFFFF));
	u32int *stack32 = (u32int*)stack;
	bool is_operand32 = false, is_address32 = false;

	while (true) {
		switch (ip[0]) {
			case 0x66:		// O32
				is_operand32 = true;
				ip++; regs->eip = (u16int)(regs->eip + 1);
				break;
			case 0x67:		// A32
				is_address32 = true;
				ip++; regs->eip = (u16int)(regs->eip + 1);
				break;
			case 0x9C:		// PUSHF
				if (is_operand32) {
					regs->useresp = ((regs->useresp & 0xFFFF) - 4) & 0xFFFF;
					stack32--;
					*stack32 = regs->eflags & VALID_FLAGS;
					if (m_if)
						*stack32 |= EFLAGS_IF;
					else
						*stack32 &= ~EFLAGS_IF;
				} else {
					regs->useresp = ((regs->useresp & 0xFFFF) - 2) & 0xFFFF;
					stack--;
					*stack = regs->eflags;
					if (m_if)
						*stack |= EFLAGS_IF;
					else
						*stack &= ~EFLAGS_IF;
				}
				regs->eip = (u16int)(regs->eip + 1);
				return true;
			case 0x9D:		// POPF
				if (is_operand32) {
					regs->eflags = EFLAGS_IF | EFLAGS_VM | (stack32[0] & VALID_FLAGS);
					m_if = (stack32[0] & EFLAGS_IF) != 0;
					regs->useresp = ((regs->useresp & 0xFFFF) + 4) & 0xFFFF;
				} else {
					regs->eflags = EFLAGS_IF | EFLAGS_VM | stack[0];
					m_if = (stack[0] & EFLAGS_IF) != 0;
					regs->useresp = ((regs->useresp & 0xFFFF) + 2) & 0xFFFF;
				}
				regs->eip = (u16int)(regs->eip + 1);
				return true;
			case 0xCD:		// INT N
				if (ip[1] == 3) return false;	//Breakpoint exception, here used for telling that thread has ended
				if (ip[1] == 60) {		//INT 60 is used so that the real mode code can retrieve some regs from caller
					regs->eax = m_ret->regs->eax;
					regs->ebx = m_ret->regs->ebx;
					regs->ecx = m_ret->regs->ecx;
					regs->edx = m_ret->regs->edx;
					regs->edi = m_ret->regs->edi;
					regs->esi = m_ret->regs->esi;
					regs->eip = (u16int)(regs->eip + 2);
					return true;
				}

				stack -= 3;
				regs->useresp = ((regs->useresp & 0xFFFF) - 6) & 0xFFFF;

				stack[0] = (u16int)(regs->eip + 2);
				stack[1] = regs->cs;
				stack[2] = (u16int)regs->eflags;

				regs->cs = ivt[ip[1] * 2 + 1];
				regs->eip = ivt[ip[1] * 2];
				return true;
			case 0xCF:		// IRET
				regs->eip = stack[0];
				regs->cs = stack[1];
				regs->eflags = EFLAGS_IF | EFLAGS_VM | stack[2];
				m_if = (stack[2] & EFLAGS_IF) != 0;
				regs->useresp = ((regs->useresp & 0xFFFF) + 6) & 0xFFFF;
				return false;
			case 0xFA:		// CLI
				m_if = false;
				regs->eip = (u16int)(regs->eip + 1);
				return true;
			case 0xFB:		// STI
				m_if = true;
				regs->eip = (u16int)(regs->eip + 1);
				return true;
			default:
				return false;
		}
	}
}

void V86Thread::handleException(registers_t *regs, int no) {
	if (no == 13) {			//General protection fault
		if (!handleV86GPF(regs)) {
			m_ret->finished = true;
			*(m_ret->regs) = *regs;
			Task::currentThreadExits(0);
			return;
		}
	} else {
		Thread::handleException(regs, no);
	}
}
