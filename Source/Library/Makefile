.PHONY: clean, mrproper

CXX = g++
CXXFLAGS = -nostartfiles -nostdlib -fno-exceptions -fno-rtti -I Common -I Userland -D THIS_IS_MELON_USERLAND

LDFLAGS = -r
LD = ld

Library = Melon.o
Objects = Common/WChar.class.uo \
		  Common/CMem.ns.uo \
		  Userland/Syscall/Syscall.wtf.uo

all: $(Library)
	echo "* Done with library"

rebuild: mrproper all

$(Library): $(Objects)
	echo "* Linking melon library $(Library)..."
	$(LD) $(LDFLAGS) $^ -o $@

%.uo: %.cpp
	echo "* Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	echo "* Removing object files..."
	rm -rf $(Objects)

mrproper: clean
	echo "* Removing library..."
	rm -rf $(Library)
