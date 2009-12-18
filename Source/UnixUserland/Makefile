.PHONY: clean, mrproper

CC = gcc
CFLAGS = -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra -I../../Ports/newlib-1.15.0/newlib/libc/include -I ../Library/Interface

LD = ld
LDFLAGS = -r

OutFile = MelonUnix.o
Objects = _exit.o \
		  _start.o \
		  close.o \
		  environ.o \
		  execve.o \
		  fork.o \
		  fstat.o \
		  getpid.o \
		  isatty.o \
		  kill.o \
		  link.o \
		  lseek.o \
		  open.o \
		  read.o \
		  sbrk.o \
		  stat.o \
		  times.o \
		  unlink.o \
		  wait.o \
		  write.o

all: $(OutFile)
	echo "* Done with $(OutFile)."

rebuild: mrproper all

$(OutFile): $(Objects)
	echo "* Linking executable : $(OutFile)..."
	$(LD) $(LDFLAGS) -o $(OutFile) $^

%.o: %.c
	echo "* Compiling $<..."
	$(CC) -c $< -o $@ $(CFLAGS)
	
clean:
	echo "* Removing object files..."
	rm -rf $(Objects)

mrproper: clean
	echo "* Removing executable: $(OutFile)"
	rm -rf $(OutFile)
