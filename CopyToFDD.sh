#!/bin/sh

# We assume Mount/ is the directory where the image is mounted, and Source/ is the directory with all the compiled files

# Update GRUB's menu.cfg
cp Grub-menu-fdd.cfg Mount/boot/menu.cfg

# Create directories
mkdir Mount/{System,Applications,Volumes}
mkdir Mount/Applications/{Shell,Demos}
mkdir Mount/System/{Applications,Logs,Configuration}
mkdir Mount/Volumes/{HDD,InitRFS}

# Copy kernel and ramfs
cp Source/Kernel/Melon.ke Mount/System
cp Init.rfs Mount/System

# Copy system files
cp Source/Kernel/Ressources/Configuration/* Mount/System/Configuration
cp Source/Applications/PaperWork/PaperWork Mount/System/Applications/PaperWork.app
cp Source/Applications/Shell/Shell Mount/Applications/Shell/Shell.app

# Create mount configuration file
echo "/Volumes/InitRFS:ramfs:0" > Mount/System/Configuration/Mount
echo "/Volumes/HDD:block.ata:0:1" >> Mount/System/Configuration/Mount

# Copy demo apps
cp Source/Applications/Demos/GOL Mount/Applications/Demos/GOL.app
cp Source/Applications/Demos/asmdemo Mount/Applications/Demos/ASM.dem

#echo "*** Launching a BASH shell, if you want to do any maintenance ***"
bash || exit 0
