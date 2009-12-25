.PHONY: clean, mrproper, Init.rfs, floppy, commit

Projects = Kernel Library Tools/MakeRamFS Applications/Shell Applications/PaperWork Applications/Demos

Kernel = Source/Kernel/Melon.ke
RamFS = Init.rfs
RamFSFiles = :/System :/System/Configuration :/System/Keymaps :/Applets \
	Source/Kernel/Ressources/Configuration/Users:/System/Configuration/Users \
	Source/Kernel/Ressources/Configuration/Groups:/System/Configuration/Groups \
	Source/Kernel/Ressources/Keymaps/fr.mkm:/System/Keymaps/fr.mkm \
	Source/Kernel/Ressources/Texts/Welcome.txt:/Welcome.txt \
	Source/Applications/Shell/Shell:/Shell.app \
	Source/Applications/Shell/Help.txt:/Help.txt \
	Source/Applications/Shell/Applets/cat:/Applets/cat \
	Source/Applications/Shell/Applets/halt:/Applets/halt \
	Source/Applications/Shell/Applets/reboot:/Applets/reboot \
	Source/Applications/Shell/Applets/uptime:/Applets/uptime \
	Source/Applications/Shell/Applets/free:/Applets/free \
	Source/Applications/Shell/Applets/ls:/Applets/ls \
	Source/Applications/Shell/Applets/pwd:/Applets/pwd \
	Source/Applications/Demos/GOL:/GOL.app \
   	Source/Kernel/Ressources/Graphics/logo.text.cxd:/Melon-logo 

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
	git commit -a; exit 0
	git push github
	git push home

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
