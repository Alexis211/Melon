#!/bin/sh


cd `dirname $0`

PORTSDIR=`pwd`
UNIXDIR=`pwd`/../Unix

INCDIR=$UNIXDIR/i586-elf/include
LIBDIR=$UNIXDIR/i586-elf/lib

echo " ==> FETCHING NEWLIB ARCHIVE <=="
if [ ! -f newlib-1.15.0.tar.gz]; then wget ftp://sources.redhat.com/pub/newlib/newlib-1.15.0.tar.gz; fi

echo " ==> EXTRACTING ARCHIVE <=="
tar xzf newlib-1.15.0.tar.gz

mkdir build-newlib

echo " =>> STARTING BUIL <<="
cd build-newlib
../newlib-1.15.0/configure --prefix=$UNIXDIR --target=i586-elf
make MAKEINFO=makeinfo
make install
