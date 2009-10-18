.PHONY: clean, mrproper

ASM = nasm
ASMFLAGS = -f elf
LD = ld
LDFLAGS = --entry=start -Ttext=40000000

Applications = asmdemo

all: $(Applications)
	echo "* Done with applications : $(Applications)"

rebuild: mrproper all

%: %.o
	echo "* Linking $<..."
	$(LD) $(LDFLAGS) $< -o $@

%.o: %.asm
	echo "* Compiling $<..."
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	echo "* Removing object files..."
	rm -rf *.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(Applications)
