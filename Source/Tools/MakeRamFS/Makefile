.PHONY: clean, mrproper

CC = gcc
CXX = g++
LD = gcc
LDFLAGS = -lstdc++
CFLAGS =
CXXFLAGS =

OutFile = MakeRamFS
Objects = main.o 

all: $(OutFile)
	echo "* Done with $(OutFile)."

rebuild: mrproper all

$(OutFile): $(Objects)
	echo "* Linking executable : $(OutFile)..."
	$(LD) -o $(OutFile) $(LDFLAGS) $^

%.o: %.c
	echo "* Compiling $<..."
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.cpp
	echo "* Compiling $<..."
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	echo "* Removing object files..."
	rm -rf *.o
	rm -rf */*.o
	rm -rf */*/*.o

mrproper: clean
	echo "* Removing executable : $(OutFile)"
	rm -rf $(OutFile)
