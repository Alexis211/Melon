.PHONY: clean, mrproper, Init.rfs, commit

Projects = Kernel Library Tools/MakeRamFS Applications/Shell Applications/SampleApps

Kernel = Source/Kernel/Melon.ke
RamFS = Init.rfs
RamFSFiles = :/System :/System/Applications :/System/Configuration :/System/Keymaps \
	:/Applications :/Applications/Demos :/Applications/Shell \
	Source/Kernel/Ressources/Configuration/Users:/System/Configuration/Users \
	Source/Kernel/Ressources/Configuration/Groups:/System/Configuration/Groups \
	Source/Kernel/Ressources/Keymaps/fr.mkm:/System/Keymaps/fr.mkm \
	Source/Kernel/Ressources/Texts/Welcome.txt:/Welcome.txt \
	Source/Applications/SampleApps/asmdemo:/Applications/Demos/ASMDemo.app \
	Source/Applications/SampleApps/cxxdemo:/Applications/Demos/CPPDemo.app \
	Source/Applications/Shell/Shell:/Applications/Shell/Shell.app \
	Source/Applications/Shell/Help.txt:/Applications/Shell/Help.txt \
	:/Useless \
   	Source/Kernel/Ressources/Graphics/logo.text.cxd:/Useless/Melon-logo 

Files = $(Kernel) $(RamFS)
Floppy = Melon.img

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

floppy: $(Files)
	mkdir Mount
	sudo mount $(Floppy) Mount -o loop
	sudo cp Grub-menu.cfg Mount/boot/menu.cfg
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
