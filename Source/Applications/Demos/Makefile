.PHONY: clean, mrproper

ASM = nasm
ASMFLAGS = -f bin

CXX = i586-elf-g++
CXXFLAGS = -nostartfiles -nostdlib -ffreestanding -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LD = i586-elf-ld
LDFLAGS = -T ../../Library/App.ld -L ../../Library

Applications = asmdemo cxxdemo GOL

all: $(Applications)
	echo "* Done with applications : $(Applications)"

rebuild: mrproper all

%: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@.o
	echo "* Linking $@.o..."
	$(LD) $(LDFLAGS) $@.o -o $@

%: %.asm
	echo "* Compiling $<..."
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	echo "* Removing object files..."
	rm -rf *.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(Applications)
