#ifndef DEF_ELFBINARY_CLASS_H
#define DEF_ELFBINARY_CLASS_H

#include <Linker/Binary.proto.h>
#include <SimpleList.class.h>

/* elf_phdr_t :: p_type : program header entries types */
#define	PT_NULL             0
#define	PT_LOAD             1
#define	PT_DYNAMIC          2
#define	PT_INTERP           3
#define	PT_NOTE             4
#define	PT_SHLIB            5
#define	PT_PHDR             6
#define	PT_LOPROC  0x70000000
#define	PT_HIPROC  0x7fffffff

/* elf_phdr_t :: p_flags : program header entries flags */
#define PF_X	(1 << 0)
#define PF_W	(1 << 1)
#define PF_R	(1 << 2)

struct elf_ehdr_t {
	u8int e_ident[16];      /* ELF identification */
	u16int e_type;             /* 2 (exec file) */
	u16int e_machine;          /* 3 (intel architecture) */
	u32int e_version;          /* 1 */
	u32int e_entry;            /* starting point */
	u32int e_phoff;            /* program header table offset */
	u32int e_shoff;            /* section header table offset */
	u32int e_flags;            /* various flags */
	u16int e_ehsize;           /* ELF header (this) size */

	u16int e_phentsize;        /* program header table entry size */
	u16int e_phnum;            /* number of entries */

	u16int e_shentsize;        /* section header table entry size */
	u16int e_shnum;            /* number of entries */

	u16int e_shstrndx;         /* index of the section name string table */
};

struct elf_phdr_t {
	u32int p_type;             /* type of segment */
	u32int p_offset;
	u32int p_vaddr;
	u32int p_paddr;
	u32int p_filesz;
	u32int p_memsz;
	u32int p_flags;
	u32int p_align;
};

struct phdr_t {
	elf_phdr_t h;
	u8int* data;
};

class ElfBinary : public Binary {
	private:
	elf_ehdr_t m_ehdr;
	SimpleList<phdr_t> *m_phdr;

	ElfBinary() {}

	public:
	virtual ~ElfBinary();
	static Binary* load(File& file);

	thread_entry_t toProcess(Process* p);
};

#endif
