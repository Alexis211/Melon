#!/bin/sh

# We assume Mount/ is the directory where the image is mounted, and Source/ is the directory with all the compiled files

# Update GRUB's menu.cfg
cp Grub-menu-fdd.cfg Mount/boot/menu.cfg

# Cleanup old stuff
cd Mount
for a in `ls`; do
	if [ $a != boot ]; then rm -rf $a; fi
done
cd ..

# Create directories
mkdir Mount/{System,Volumes,Sandbox}
mkdir Mount/System/{Applications,Logs,Configuration}
mkdir Mount/Volumes/{HDD,InitRFS}

# Copy system files
cp Source/Kernel/Ressources/Configuration/* Mount/System/Configuration
cp Source/Applications/PaperWork/FloppyWelcome.txt Mount/System/Configuration/Welcome

# Copy system apps
cp Source/Applications/PaperWork/PaperWork Mount/System/Applications/PaperWork.app

# Copy kernel and ramfs
cp Source/Kernel/Melon.ke Mount/System
cp Init.rfs Mount/System

# Create mount configuration file
echo "/Sandbox:ramfs:0" > Mount/System/Configuration/Mount
echo "/Volumes/HDD:block.ata:0:1" >> Mount/System/Configuration/Mount

#echo "*** Launching a BASH shell, if you want to do any maintenance ***"
#bash || exit 0
