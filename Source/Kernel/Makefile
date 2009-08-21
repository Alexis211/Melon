.PHONY: clean, mrproper

CC = gcc
CXX = g++
LD = ld
LDFLAGS = -T Link.ld -Map Map.txt --oformat=elf32-i386
CFLAGS = -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra -Werror
CXXFLAGS = -nostartfiles -nostdlib -fno-rtti -fno-exceptions -I . -Wall -Werror -Wno-write-strings
ASM = nasm
ASMFLAGS = -f elf

OutFile = Melon.ke
Objects = Core/kmain.wtf.o \
		  Core/loader.wtf.o \
		  Core/cppsupport.wtf.o \
		  Core/Sys.ns.o \
		  Core/CMem.ns.o \
		  MemoryManager/Mem.ns.o \
		  DisplayManager/Disp.ns.o \
		  VTManager/VirtualTerminal.class.o \
		  Devices/Display/VGATextOutput.class.o

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
