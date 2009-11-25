#!/bin/sh

# We assume Mount/ is the directory where the image is mounted, and Source/ is the directory with all the compiled files

# Update GRUB's menu.cfg
cp Grub-menu-fdd.cfg Mount/boot/menu.cfg

# Create directories
mkdir Mount/{System,Apps,Mount}
mkdir Mount/System/{Applications,Logs,Configuration}

# Copy system files
cp Source/Kernel/Ressources/Configuration/* Mount/System/Configuration
cp Source/Applications/PaperWork/PaperWork Mount/System/Applications/PaperWork.app
cp Source/Applications/Shell/Shell Mount/Apps/Shell.app

# Copy demo apps
cp Source/Applications/Demos/GOL Mount/Apps/GOL.app
cp Source/Applications/Demos/asmdemo Mount/Apps/ASM.dem

# Copy kernel and ramfs
cp Source/Kernel/Melon.ke Mount/System
cp Init.rfs Mount/System

#echo "*** Launching a BASH shell, if you want to do any maintenance ***"
#bash || exit 0
