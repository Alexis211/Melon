.PHONY: clean, mrproper
 
CC = gcc
CXX = g++
LD = ld
LDFLAGS = -T Link.ld -Map Map.txt --oformat=elf32-i386
CFLAGS = -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -I .
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I . -Wall -Werror -Wno-write-strings -funsigned-char -D THIS_IS_MELON -D RANDOM_SEED=1`date +%N`LL -g
ASM = nasm
ASMFLAGS = -f elf
 
OutFile = Melon.ke
Objects = Core/loader.wtf.o \
		Core/kmain.wtf.o \
		Core/cppsupport.wtf.o \
		Core/Sys.ns.o \
		Core/CMem.ns.o \
		Core/Log.ns.o \
		MemoryManager/Mem.ns.o \
		MemoryManager/Heap.class.o \
		MemoryManager/Heap-index.class.o \
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
		TaskManager/Thread.class.o \
		TaskManager/Task.ns.o \
		TaskManager/Task.wtf.o \
		TaskManager/Mutex.class.o \
		VTManager/VirtualTerminal.proto.o \
		VTManager/SimpleVT.class.o \
		VTManager/ScrollableVT.class.o \
		VTManager/PipeVT.class.o \
		VTManager/FileVT.class.o \
		VTManager/VirtualTerminal-kbd.proto.o \
		VTManager/VT.ns.o \
		Shell/KernelShell.class.o \
		Shell/KernelShell-fs.class.o \
		Shell/KernelShell-sys.class.o \
		Library/Bitset.class.o \
		Library/String.class.o \
		Library/ByteArray.class.o \
		Library/WChar.class.o \
		Library/Rand.ns.o \
		VFS/Partition.class.o \
		VFS/Part.ns.o \
		VFS/VFS.ns.o \
		VFS/File.class.o \
		VFS/TextFile.class.o \
		VFS/DirectoryNode.class.o \
		FileSystems/RamFS/RamFS.class.o \
		SyscallManager/IDT.ns.o \
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
	rm -rf *.o
	rm -rf */*.o
	rm -rf */*/*.o
 
mrproper: clean
	echo "* Removing executable : $(OutFile)"
	rm -rf $(OutFile)

