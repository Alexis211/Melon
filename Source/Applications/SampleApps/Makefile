.PHONY: clean, mrproper

ASM = nasm
ASMFLAGS = -f elf

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LD = ld
LDFLAGS = --entry=start -Ttext=40000000

Applications = asmdemo cxxdemo

all: $(Applications)
	echo "* Done with applications : $(Applications)"

rebuild: mrproper all

%: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@.o
	echo "* Linking $@.o..."
	$(LD) $(LDFLAGS) ../../Library/Melon.o $@.o -o $@

%: %.asm
	echo "* Compiling $<..."
	$(ASM) $(ASMFLAGS) -o $@.o $<
	echo "* Linking $@.o..."
	$(LD) $(LDFLAGS) $@.o -o $@

clean:
	echo "* Removing object files..."
	rm -rf *.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(Applications)
