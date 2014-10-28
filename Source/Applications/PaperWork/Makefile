.PHONY: clean, mrproper

CXX = i586-elf-g++
CXXFLAGS = -nostartfiles -nostdlib -ffreestanding -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LD = i586-elf-ld
LDFLAGS = -T ../../Library/App.ld -L ../../Library -Map Map.txt

Objects = PaperWork.o
OutFile = PaperWork

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
