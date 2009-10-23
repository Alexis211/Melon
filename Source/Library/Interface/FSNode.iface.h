#ifndef DEF_FSNODE_IFACE_H
#define DEF_FSNODE_IFACE_H

enum {
	NT_FILE = 1,
	NT_DIRECTORY = 2,
	NT_SYMLINK = 3,
	NT_MOUNTPOINT = 4
};

#define FNIF_OBJTYPE 0x14

//S : static, GET : get, R : root, FN : fsnode
#define FNIF_SGETRFN 0
#define FNIF_SGETCWD 1	//Get current working directory
#define FNIF_SFIND 2	//Find a node following a path from a node

#define FNIF_GETNAME 0x10
#define FNIF_TYPE 0x11
#define FNIF_GETPARENT 0x12
#define FNIF_GETLENGTH 0x13
#define FNIF_GETUID 0x14
#define FNIF_GETGID 0x15
#define FNIF_GETPERM 0x16
#define FNIF_GETPATH 0x17
#define FNIF_SETCWD 0x18		//Sets node as current working directory

#define FNIF_GETIDXCHILD 0x20	//Get child node from index
#define FNIF_GETNAMECHILD 0x21	//Get child node from name

#endif
