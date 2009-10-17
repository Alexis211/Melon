.PHONY: clean, mrproper

ASM = nasm

Applications = test

all: $(Applications)
	echo "* Done with ASM applications : $(Applications)"

rebuild: mrproper all

%: %.asm
	echo "* Compiling $<..."
	$(ASM) -o $@ $<

clean:
	echo "* Removing files..."
	rm -rf $(Applications)

mrproper: clean
