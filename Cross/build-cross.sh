#!/bin/sh

cd `dirname $0`

export PREFIX=`pwd`
export TARGET=i586-elf

mkdir src
cd src

echo " ==> FETCHING CROSS COMPILER ARCHIVES <=="
if [ ! -f gcc-core-4.4.2.tar.bz2 ]; then wget http://ftp.gnu.org/gnu/gcc/gcc-4.4.2/gcc-core-4.4.2.tar.bz2; fi
if [ ! -f gcc-g++-4.4.2.tar.bz2 ]; then wget http://ftp.gnu.org/gnu/gcc/gcc-4.4.2/gcc-g++-4.4.2.tar.bz2; fi
if [ ! -f binutils-2.20.tar.bz2 ]; then wget http://ftp.gnu.org/gnu/binutils/binutils-2.20.tar.bz2; fi

echo " ==> EXTRACTING ARCHIVES <=="
tar xjf gcc-core-4.4.2.tar.bz2
tar xjf gcc-g++-4.4.2.tar.bz2
tar xjf binutils-2.20.tar.bz2

mkdir build-binutils
mkdir build-gcc

echo " =>> STARTING BUILD <<="
cd build-binutils
../binutils-2.20/configure --prefix=$PREFIX --target=$TARGET --disable-nls || exit
make all || exit
make install || exit

export PATH=$PATH:$PREFIX/bin

cd ../build-gcc
../gcc-4.4.2/configure --prefix=$PREFIX --target=$TARGET --disable-nls --enable-languages=c,c++ --without-headers || exit
make all-gcc || exit
make install-gcc || exit
make all-target-libgcc || exit
make install-target-libgcc || exit
