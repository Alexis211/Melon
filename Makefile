.PHONY: clean, mrproper, Init.rfs

Projects = Kernel Tools/MakeRamFS Applications/ASMApps

Kernel = Source/Kernel/Melon.ke
RamFS = Init.rfs
RamFSFiles = :/System :/System/Applications :/System/Configuration :/System/Keymaps \
	Source/Kernel/Ressources/Keymaps/fr.mkm:/System/Keymaps/fr.mkm \
	Source/Kernel/Ressources/Texts/Welcome.txt:/Welcome.txt \
	Source/Applications/ASMApps/test:/System/Applications/Test \
	:/Useless \
	Source/Kernel/Ressources/Texts/Info.txt:/Useless/Info.txt \
   	Source/Kernel/Ressources/Graphics/logo.text.cxd:/Useless/Melon-logo 

Files = $(Kernel) $(RamFS)
Floppy = Melon.img

all:
	for p in $(Projects); do \
		make -C Source/$$p -s; \
	done

$(Files): all

rebuild:
	for p in $(Projects); do \
		make -C Source/$$p rebuild -s; \
	done

clean:
	for p in $(Projects); do \
		make -C Source/$$p clean -s; \
	done

mproper:
	for p in $(Projects); do \
		make -C Source mrproper -s; \
	done

$(RamFS):
	Source/Tools/MakeRamFS/MakeRamFS $(RamFS) $(RamFSFiles)

floppy: $(Files)
	sudo mount $(Floppy) Mount -o loop
	for f in $(Files); do \
		sudo cp $$f Mount; \
	done
	sleep 0.4
	sudo umount Mount

bochs:
	bochs -f Bochs.cfg

qemu:
	qemu -fda $(Floppy) -m 16

qemu_debug:
	qemu -fda $(Floppy) -m 16 -s -S & gdb Source/Kernel/Melon.ke -x Qemu-GDB-Debug-CMD 

stats:
	echo; echo " ** Statistics for project O3S ** "; \
	echo -n "Lines of code : "; \
	cat Source/*/{*,*/*,*/*/*}.{c,asm,cpp,h} 2> /dev/null | wc -l; \
	echo "TODOs : "; \
	git grep TODO
	#cat Source/*/{*,*/*,*/*/*}.{c,asm,cpp,h} 2> /dev/null | grep TODO;
