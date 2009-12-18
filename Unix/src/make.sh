#!/bin/sh

gcc -c test.c -o test.o -I ../i686-elf/include -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector
ld test.o ../i686-elf/lib/libc.a -T ../../Source/UnixUserland/App.ld -L ../../Source/UnixUserland -o test
