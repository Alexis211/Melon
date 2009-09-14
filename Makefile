.PHONY: clean, mrproper, Init.rfs

Files = Source/Kernel/Melon.ke Init.rfs
Floppy = Melon.img

Projects = Kernel Tools/MakeRamFS

RamFS = Init.rfs
RamFSFiles = :/System :/System/Applications Source/Kernel/Ressources/Welcome.txt:/Welcome.txt :/System/Configuration :/Useless Source/Kernel/Ressources/Info.txt:/Useless/Info.txt Source/Kernel/Ressources/logo.cd:/Useless/Melon-logo 

all:
	for p in $(Projects); do \
		make -C Source/$$p -s; \
	done

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

floppy: $(RamFS)
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

stats:
	echo; echo " ** Statistics for project O3S ** "; \
	echo -n "Lines of code : "; \
	cat Source/*/{*,*/*,*/*/*}.{c,asm,cpp,h} 2> /dev/null | wc -l; \
	echo "TODOs : "; \
	git grep TODO
	#cat Source/*/{*,*/*,*/*/*}.{c,asm,cpp,h} 2> /dev/null | grep TODO;
