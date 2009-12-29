#include "ElfBinary.class.h"
#include <TaskManager/Task.ns.h>

ElfBinary::~ElfBinary() {
	for (SimpleList<phdr_t> *iter = m_phdr; iter != 0; iter = iter->next()) {
		delete iter->v().data;
	}
}

Binary* ElfBinary::load(File& file) {
	elf_ehdr_t hdr;
	if (!file.read<elf_ehdr_t> (&hdr)) return 0;
	//Verify we have an elf file
	if (hdr.e_ident[0] != 0x7F or hdr.e_ident[1] != 'E' or hdr.e_ident[2] != 'L' or hdr.e_ident[3] != 'F') return 0;

	//Store elf header into a new ElfBinary
	ElfBinary* b = new ElfBinary();
	memcpy((u8int*)&b->m_ehdr, (const u8int*) &hdr, sizeof(elf_ehdr_t));
	b->m_phdr = 0;

	//Load program headers
	file.seek(hdr.e_phoff, SM_BEGINNING);
	for (u32int i = 0; i < hdr.e_phnum; i++) {
		b->m_phdr = b->m_phdr->cons(phdr_t());
		file.read<elf_phdr_t>(&b->m_phdr->v().h);	//Load current entry from file
	}
	//Load data
	for (SimpleList<phdr_t> *iter = b->m_phdr; iter != 0; iter = iter->next()) {
		iter->v().data = (u8int*)Mem::alloc(iter->v().h.p_filesz);
		file.seek(iter->v().h.p_offset, SM_BEGINNING);
		file.read(iter->v().h.p_filesz, iter->v().data);
	}

	return b;
}

thread_entry_t ElfBinary::toProcess(Process* p) {
	for (SimpleList<phdr_t> *iter = m_phdr; iter != 0; iter = iter->next()) {
		phdr_t &e = iter->v();
		if (e.h.p_type == PT_LOAD) {
			((SimpleSegment*)p->addSeg(new SimpleSegment(true, (e.h.p_flags & PF_W) != 0, e.h.p_vaddr, e.h.p_memsz)))->switchToPd();
			memcpy((u8int*)e.h.p_vaddr, (const u8int*)e.data, e.h.p_filesz);
			if (e.h.p_memsz > e.h.p_filesz) {	//set to zero all the remaining space
				u8int* x = (u8int*)e.h.p_vaddr;
				for (u32int i = e.h.p_vaddr + e.h.p_filesz; i < e.h.p_filesz; i++) {
					x[i] = 0;
				}
			}
		}
	}
	return (thread_entry_t)m_ehdr.e_entry;
}
