.PHONY: clean, mrproper

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -ffreestanding -fno-exceptions -fno-rtti -I ../../Library/Common -I ../../Library/Interface -I ../../Library/Userland -D THIS_IS_MELON_USERLAND

LD = ld
LDFLAGS = -T ../../Library/App.ld -L ../../Library

Objects = Shell.class.o
OutFile = Shell

Applets = Applets/cat Applets/halt Applets/reboot Applets/uptime Applets/free Applets/ls \
		  Applets/pwd Applets/rm Applets/mkdir

all: $(OutFile) $(Applets)
	echo "* Done with $(OutFile)."

rebuild: mrproper all

$(OutFile): $(Objects)
	echo "* Linking $@..."
	$(LD) $(LDFLAGS) $^ -o $@

Applets/%: Applets/%.o
	echo "* Linking $@..."
	$(LD) $(LDFLAGS) $^ -o $@ -Map $@.map

%.o: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	echo "* Removing object files..."
	rm -rf *.o Applets/*.o

mrproper: clean
	echo "* Removing applications..."
	rm -rf $(OutFile) $(Applets)
