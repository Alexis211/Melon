#!/bin/sh

# We assume Mount/ is the directory where the image is mounted, and Source/ is the directory with all the compiled files

# GRUB is already installed to image, just update config file if needed
cp Grub-menu-hdd.cfg Mount/grub/menu.lst

# Create directories
mkdir Mount/{System,Applications,Volumes}
mkdir Mount/System/{Applications,Configuration,Logs,Keymaps}
mkdir Mount/Applications/{Demos,Shell}
mkdir Mount/Applications/Shell/Applets
mkdir Mount/Volumes/{Floppy,InitRFS}

# Copy welcome text
cp Source/Kernel/Ressources/Texts/Welcome.txt Mount

# Copy kernel and initramfs
cp Source/Kernel/Melon.ke Mount/System
cp Init.rfs Mount/System

# Copy system ressources
cp Source/Kernel/Ressources/Configuration/* Mount/System/Configuration
cp Source/Kernel/Ressources/Keymaps/*.mkm Mount/System/Keymaps
cp Source/Applications/PaperWork/PaperWork Mount/System/Applications/PaperWork.app
cp Source/Applications/Shell/Shell Mount/Applications/Shell/Shell.app
cp Source/Applications/Shell/Applets/rot13 Mount/Applications/Shell/Applets
cp Source/Applications/Shell/Help.txt Mount/Applications/Shell

# Create mount configuration file
echo "/Volumes/InitRFS:ramfs:0" > Mount/System/Configuration/Mount
echo "/Volumes/Floppy:block.floppy:0:0" >> Mount/System/Configuration/Mount

# Copy demo apps
cp Source/Applications/Demos/GOL Mount/Applications/Demos/GOL.app
cp Source/Applications/Demos/asmdemo Mount/Applications/Demos/ASMDemo.app
cp Source/Applications/Demos/cxxdemo Mount/Applications/Demos/CPPDemo.app

echo "*** Launching a BASH shell, if you want to do any maintenance ***"
bash || exit 0
