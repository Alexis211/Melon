.PHONY: clean, mrproper

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -ffreestanding -fno-exceptions -fno-rtti -I Common -I Userland -I Interface -D THIS_IS_MELON_USERLAND -D RANDOM_SEED=1`date +%N`LL

ASM = nasm
ASMFLAGS = -f elf

LD = ld

Objects = Common/WChar.class.uo \
		  Common/CMem.ns.uo \
		  Common/Mutex.class.uo \
		  Common/Heap.class.uo \
		  Common/Heap-index.class.uo \
		  Common/String.class.uo \
		  Common/TextFile.class.uo \
		  Common/ByteArray.class.uo \
		  Common/Rand.ns.uo \
		  Common/OStream.proto.uo \
		  Common/IStream.proto.uo \
		  Common/FileStream.class.uo \
		  Common/StringStream.class.uo \
		  Common/cppsupport.wtf.uo \
		  Userland/App/ShellApp.proto.uo \
		  Userland/App/StreamApp.proto.uo \
		  Userland/Syscall/Syscall.wtf.uo \
		  Userland/Syscall/RessourceCaller.class.uo \
		  Userland/Start.uo

all: libMelon.o
	echo "* Done with library"

rebuild: mrproper all

libMelon.o: $(Objects)
	echo "* Linking static Melon library libMelon.o..."
	$(LD) -r $^ -o $@

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
