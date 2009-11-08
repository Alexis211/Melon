.PHONY: clean, mrproper
 
CC = gcc
CXX = g++
LD = ld
LDFLAGS = -T Link.ld -Map Map.txt --oformat=elf32-i386
CFLAGS = -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -I .
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I . -I ../Library/Common -I ../Library/Interface -Wall -Werror -Wno-write-strings -funsigned-char -D THIS_IS_MELON_KERNEL -D RANDOM_SEED=1`date +%N`LL
ASM = nasm
ASMFLAGS = -f elf
 
OutFile = Melon.ke
Objects = Core/loader.wtf.o \
		Core/kmain.wtf.o \
		Core/cppsupport.wtf.o \
		Core/Sys.ns.o \
		Core/Log.ns.o \
		MemoryManager/Mem.ns.o \
		MemoryManager/PhysMem.ns.o \
		MemoryManager/GDT.wtf.o \
		MemoryManager/GDT.ns.o \
		MemoryManager/PageDirectory.class.o \
		MemoryManager/PageAlloc.ns.o \
		DeviceManager/Disp.ns.o \
		DeviceManager/Dev.ns.o \
		DeviceManager/Time.ns.o \
		DeviceManager/Kbd.ns.o \
		TaskManager/Process.class.o \
		TaskManager/Process-sc.class.o \
		TaskManager/Thread.class.o \
		TaskManager/V86/V86Thread.class.o \
		TaskManager/V86/v86.wtf.o \
		TaskManager/Task.ns.o \
		TaskManager/Task.wtf.o \
		VTManager/VirtualTerminal.proto.o \
		VTManager/SimpleVT.class.o \
		VTManager/ScrollableVT.class.o \
		VTManager/PipeVT.class.o \
		VTManager/FileVT.class.o \
		VTManager/VirtualTerminal-kbd.proto.o \
		VTManager/VirtualTerminal-sc.proto.o \
		VTManager/VT.ns.o \
		Shell/KernelShell.class.o \
		Shell/KernelShell-fs.class.o \
		Shell/KernelShell-sys.class.o \
		Linker/Binary.proto.o \
		Linker/MelonBinary.class.o \
		Linker/ElfBinary.class.o \
		../Library/Common/Bitset.class.o \
		../Library/Common/String.class.o \
		../Library/Common/ByteArray.class.o \
		../Library/Common/WChar.class.o \
		../Library/Common/Rand.ns.o \
		../Library/Common/CMem.ns.o \
		../Library/Common/Heap.class.o \
		../Library/Common/Heap-index.class.o \
		../Library/Common/Mutex.class.o \
		../Library/Common/TextFile.class.o \
		VFS/Partition.class.o \
		VFS/Part.ns.o \
		VFS/VFS.ns.o \
		VFS/FSNode-sc.proto.o \
		VFS/File.class.o \
		VFS/File-sc.class.o \
		VFS/DirectoryNode.class.o \
		UserManager/Usr.ns.o \
		FileSystems/RamFS/RamFS.class.o \
		SyscallManager/IDT.ns.o \
		SyscallManager/Ressource.class.o \
		SyscallManager/Res.ns.o \
		SyscallManager/IDT.wtf.o \
		Devices/Display/VGATextOutput.class.o \
		Devices/Keyboard/PS2Keyboard.class.o \
		Devices/Floppy/FloppyController.class.o \
		Devices/Floppy/FloppyDrive.class.o \
		Devices/Timer.class.o
 
all: $(OutFile)
	echo "* Done with $(OutFile)."
	 
rebuild: mrproper all
 
$(OutFile): $(Objects)
	echo "* Linking executable : $(OutFile)..."
	$(LD) $(LDFLAGS) -o $(OutFile) $^
 
%.o: %.c
	echo "* Compiling $<..."
	$(CC) -c $< -o $@ $(CFLAGS)
 
%.o: %.cpp
	echo "* Compiling $<..."
	$(CXX) -c $< -o $@ $(CXXFLAGS)
 
%.o: %.asm
	echo "* Compiling $<..."
	$(ASM) $(ASMFLAGS) -o $@ $<
 
clean:
	echo "* Removing object files..."
	rm -rf $(Objects)
	rm -rf *.o
	rm -rf */*.o
	rm -rf */*/*.o
	rm -rf ../Library/Common/*.o
 
mrproper: clean
	echo "* Removing executable : $(OutFile)"
	rm -rf $(OutFile)

