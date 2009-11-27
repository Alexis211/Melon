.PHONY: clean, mrproper, Init.rfs, commit

Projects = Kernel Library Tools/MakeRamFS Applications/Shell Applications/PaperWork Applications/Demos

Kernel = Source/Kernel/Melon.ke
RamFS = Init.rfs
RamFSFiles = :/System :/System/Applications :/System/Configuration :/System/Keymaps \
	:/Applications :/Applications/Demos :/Applications/Shell :/Mount \
	Source/Kernel/Ressources/Configuration/Users:/System/Configuration/Users \
	Source/Kernel/Ressources/Configuration/Groups:/System/Configuration/Groups \
	Source/Kernel/Ressources/Keymaps/fr.mkm:/System/Keymaps/fr.mkm \
	Source/Kernel/Ressources/Texts/Welcome.txt:/Welcome.txt \
	Source/Applications/Demos/asmdemo:/Applications/Demos/ASMDemo.app \
	Source/Applications/Demos/cxxdemo:/Applications/Demos/CPPDemo.app \
	Source/Applications/Demos/GOL:/Applications/Demos/GOL.app \
	Source/Applications/Shell/Shell:/Applications/Shell/Shell.app \
	Source/Applications/Shell/Help.txt:/Applications/Shell/Help.txt \
	Source/Applications/PaperWork/PaperWork:/System/Applications/PaperWork.app \
	:/Useless \
   	Source/Kernel/Ressources/Graphics/logo.text.cxd:/Useless/Melon-logo 

Files = $(Kernel) $(RamFS)
Floppy = Melon.img
HDD = HDD.img

all:
	for p in $(Projects); do \
		echo "=> Building $$p"; \
		make -C Source/$$p -s; \
	done

$(Files): all

rebuild:
	for p in $(Projects); do \
		echo "=> Building $$p"; \
		make -C Source/$$p rebuild -s; \
	done

clean:
	for p in $(Projects); do \
		echo "=> Building $$p"; \
		make -C Source/$$p clean -s; \
	done

mrproper:
	for p in $(Projects); do \
		echo "=> Building $$p"; \
		make -C Source/$$p mrproper -s; \
	done

commit: mrproper
	git add .
	git commit -a
	git push origin

$(RamFS):
	Source/Tools/MakeRamFS/MakeRamFS $(RamFS) $(RamFSFiles)

$(Floppy): $(Files)
	mkdir Mount; exit 0
	sudo mount $(Floppy) Mount -o loop
	sudo ./CopyToFDD.sh
	sleep 0.4
	sudo umount Mount

$(HDD): $(Kernel)
	mkdir Mount; exit 0
	sudo losetup -o 32256 /dev/loop3 $(HDD)
	sudo mount /dev/loop3 Mount
	sudo ./CopyToHDD.sh
	sudo umount /dev/loop3
	sudo losetup -d /dev/loop3	

bochs: $(Floppy)
	bochs -f Bochs.cfg

qemu: $(Floppy)
	qemu -fda $(Floppy) -hda $(HDD) -boot a -m 16

qemu-hdd: $(HDD)
	qemu -fda $(Floppy) -hda $(HDD) -boot c -m 16

qemu_debug:
	qemu -fda $(Floppy) -hda $(HDD) -boot a -m 16 -s -S & gdb Source/Kernel/Melon.ke -x Qemu-GDB-Debug-CMD 

stats:
	echo; echo " ** Statistics for project O3S ** "; \
	echo -n "Lines of code : "; \
	cat Source/*/{*,*/*,*/*/*,*/*/*/*}.{c,asm,cpp,h} 2> /dev/null | wc -l; \
	echo "TODOs : "; \
	git grep TODO \
	#cat Source/*/{*,*/*,*/*/*}.{c,asm,cpp,h} 2> /dev/null | grep TODO;
