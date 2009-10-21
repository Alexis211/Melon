.PHONY: clean, mrproper

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LD = ld
LDFLAGS = -T ../../Library/Link.ld

Objects = main.o \
		  Shell.ns.o
OutFile = Shell

all: $(OutFile)
	echo "* Done with $(OutFile)."

rebuild: mrproper all

$(OutFile): $(Objects)
	echo "* Linking $@.o..."
	$(LD) $(LDFLAGS) ../../Library/Melon.o $^ -o $@

%.o: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	echo "* Removing object files..."
	rm -rf *.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(OutFile)
