#include "MelonBinary.class.h"

Binary* MelonBinary::load(File& file) {
	u32int magic;
	if (!file.read<u32int>(&magic)) return 0;
	if (magic == 0xFEEDBEEF) {
		MelonBinary* r = new MelonBinary;
		file.read<u32int>(&r->m_size);
		file.read<u32int>(&r->m_org);
		r->m_data = (u8int*)Mem::alloc(r->m_size);
		file.read(r->m_size, r->m_data);
		return r;
	} else {
		return 0;
	}
}

MelonBinary::~MelonBinary() {
	delete m_data;
}

thread_entry_t MelonBinary::toProcess(Process* p) {
	if (p == 0) return 0;
	for (u32int i = m_org; i < m_org + m_size; i += 0x1000) {
		p->getPagedir()->allocFrame(i, true, true);
	}
	p->getPagedir()->switchTo();
	memcpy((u8int*)m_org, m_data, m_size);
	return (thread_entry_t)m_org;
}
