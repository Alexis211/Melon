#!/bin/sh

i586-elf-gcc -c test.c -o test.o -I ../i586-elf/include -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector
i586-elf-ld test.o ../i586-elf/lib/libc.a -T ../../Source/UnixUserland/App.ld -L ../../Source/UnixUserland -o test -Map map.txt
