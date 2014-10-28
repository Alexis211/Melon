#include "V86Thread.class.h"
#include <TaskManager/Task.ns.h>

#include <TaskManager/V86/V86.ns.h>

//in v86.wtf.asm
extern "C" void v86_run(u32int pd_phys,	//A page directory's physical address
	v86_regs_t* regs);

/*
 *	Set up a V86 task :
 *	Allocate space for the kernel stack
 *	Map frames in lower 1MB
 *  Find somewhere to put the stack and the code, still in lower 1MB
 *  Copy the 16bit code to that place
 *  Setup values on the kernel stack for starting the thread (V86Thread::runV86),
 *  giving it entry point and stack location
 */

V86Thread::V86Thread(v86_function_t* entry, v86_retval_t* ret) : Thread() {
	m_ret = ret;
	setup();
	m_continueOnIret = true;

	m_ret->regs->cs = V86::allocSeg(entry->size);	//Alocate segments for the code to run in
	u8int* codeptr = (u8int*)(FP_TO_LINEAR(m_ret->regs->cs, 0));
	memcpy(codeptr, entry->data, entry->size);	//Copy the code there
	
	m_ret->regs->ip = 0;

	m_state = T_RUNNING;
	m_process->registerThread(this);
	Task::registerThread(this);
}

V86Thread::V86Thread(u8int int_no, v86_retval_t* ret) : Thread() {
	m_ret = ret;
	setup();
	m_continueOnIret = false;

	//setup CS:IP for running interrupt
	u16int* ivt = 0;

	m_ret->regs->cs = ivt[int_no * 2 + 1];
	m_ret->regs->ip = ivt[int_no * 2];

	m_state = T_RUNNING;
	m_process->registerThread(this);
	Task::registerThread(this);
}

void V86Thread::setup() {
	m_ret->finished = false; m_xchgspace = 0; m_isKernel = true;
	m_process = Task::currProcess();
	m_if = true;	//Set virtual interrupt flag
	m_kernelStack.addr = Mem::alloc(STACKSIZE); m_kernelStack.size = STACKSIZE; //Setup kernel stack

	m_process->getPagedir()->switchTo();

	//Map all lower memory
	V86::map();

	//Allocate space for v86 stack
	m_ret->regs->ss = V86::allocSeg(V86_STACKSIZE);
	m_ret->regs->sp = V86_STACKSIZE - 4;

	//Setup kernel stack for running v86_run (entry procedure)
	u32int* stack = (u32int*)((u32int)m_kernelStack.addr + m_kernelStack.size);
	stack--; *stack = (u32int)m_ret->regs;
	stack--; *stack = m_process->getPagedir()->physicalAddr;
	stack--; *stack = 0;
	m_esp = (u32int)stack;
	m_ebp = m_esp + 4;
	m_eip = (u32int)v86_run;
}

bool V86Thread::handleV86GPF(registers_t *regs) {
	u8int* ip = (u8int*)FP_TO_LINEAR(regs->cs, regs->eip);
	u16int *ivt = 0;
	u16int *stack = (u16int*)FP_TO_LINEAR(regs->ss, (regs->useresp & 0xFFFF));
	u32int *stack32 = (u32int*)stack;
	bool is_operand32 = false;

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
				return m_continueOnIret;
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
			m_ret->regs->ax = (u16int)regs->eax;
			m_ret->regs->bx = (u16int)regs->ebx;
			m_ret->regs->cx = (u16int)regs->ecx;
			m_ret->regs->dx = (u16int)regs->edx;
			m_ret->regs->di = (u16int)regs->edi;
			m_ret->regs->si = (u16int)regs->esi;
			Task::currentThreadExits(0);
			return;
		}
	} else {
		Thread::handleException(regs, no);
	}
}
