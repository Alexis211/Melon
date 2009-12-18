.PHONY: clean, mrproper

CXXFLAGS = -nostartfiles -nostdlib -ffreestanding -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LDFLAGS = -T ../../Library/App.ld -L ../../Library

Objects = main.o \
		  Shell.ns.o \
		  Shell-fs.ns.o
OutFile = Shell

all: $(OutFile)
	echo "* Done with $(OutFile)."

rebuild: mrproper all

$(OutFile): $(Objects)
	echo "* Linking $@..."
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	echo "* Removing object files..."
	rm -rf *.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(OutFile)
