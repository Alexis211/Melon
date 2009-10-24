#ifndef DEF_PROCESS_IFACE_H
#define DEF_PROCESS_IFACE_H

#define E_AUTODELETE 0x0FFFFF03

#define PRIF_OBJTYPE 0x20

//S = static, GET = get, C = current, PR = process
#define PRIF_SGETCPR 0
#define PRIF_SRUN 1
#define PRIF_SWAIT 2

#define PRIF_EXIT 0x01
#define PRIF_ALLOCPAGE 0x02
#define PRIF_FREEPAGE 0x03
#define PRIF_GETPID 0x04
#define PRIF_GETPPID 0x05

#define PRIF_ARGC 0x10
#define PRIF_ARGV 0x11

#define PRIF_START 0x20
#define PRIF_AUTODELETE 0x21
#define PRIF_PUSHARG 0x28
#define PRIF_SETOUTVT 0x29
#define PRIF_SETINVT 0x30

#endif
