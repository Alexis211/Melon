#!/bin/sh

#DIR=`pwd`
#DIR=`dirname $DIR`
#cd $DIR

for KM in `ls | grep cxd`; do
	echo "#define THIS_IS_NOT_MELON" > kmtemp.cpp
	echo "#include <cstring>" >> kmtemp.cpp
	echo "#include <Library/WChar.class.cpp>" >> kmtemp.cpp
	echo "#include \"$KM\"" >> kmtemp.cpp
	cat WriteKeymap.cpp >> kmtemp.cpp

	g++ kmtemp.cpp -o kmtemp -I ../..
	./kmtemp
done

rm kmtemp*
