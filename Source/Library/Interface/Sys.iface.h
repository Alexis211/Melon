#ifndef SYS_IFACE_H
#define SYS_IFACE_H

#define SYIF_IFID 0xFF

#define SYIF_HALT 0x1	//Halt system, root only (v) | no arguments
#define SYIF_REBOOT 0x2	//Reboot system, root only (v) | no arguments
#define SYIF_UPTIME 0x3	//Get uptime (i) | no arguments
#define SYIF_TOTALRAM 0x4	//Get toal amount of RAM in Ko (i) | no argmuents
#define SYIF_FREERAM 0x5	//Get free amount of RAM in Ko (i) | no arguments

#endif
