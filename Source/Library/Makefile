.PHONY: clean, mrproper

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I Common -I Userland -I Interface -D THIS_IS_MELON_USERLAND

ASM = nasm
ASMFLAGS = -f elf

LDFLAGS = -r
LD = ld

Library = Melon.o
Objects = Common/WChar.class.uo \
		  Common/CMem.ns.uo \
		  Common/Mutex.class.uo \
		  Common/Heap.class.uo \
		  Common/Heap-index.class.uo \
		  Common/String.class.uo \
		  Userland/Syscall/Syscall.wtf.uo \
		  Userland/Syscall/RessourceCaller.class.uo \
		  Userland/Start.uo

all: $(Library)
	echo "* Done with library"

rebuild: mrproper all

$(Library): $(Objects)
	echo "* Linking melon library $(Library)..."
	$(LD) $(LDFLAGS) $^ -o $@

%.uo: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.uo: %.asm
	echo "* Compiling $<..."
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	echo "* Removing object files..."
	rm -rf $(Objects)

mrproper: clean
	echo "* Removing library..."
	rm -rf $(Library)