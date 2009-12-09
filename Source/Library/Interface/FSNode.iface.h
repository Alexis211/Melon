#ifndef DEF_FSNODE_IFACE_H
#define DEF_FSNODE_IFACE_H

/*
 * The FSNode interface handles all this :
 * - Navigating in the filesystem
 * - Listing contents of directories
 * - Creating/removing/truncating files and directories
 * - Getting/Setting current working directory (CWD)
 */

enum {
	NT_FILE = 1,
	NT_DIRECTORY = 2,
	NT_SYMLINK = 3,
	NT_MOUNTPOINT = 4
};

#define FNIF_OBJTYPE 0x14

//S : static, GET : get, R : root, FN : fsnode

#define FNIF_SGETRFN 0	//Get root filesystem node (R) | no arguments
#define FNIF_SGETCWD 1	//Get current working directory (R) | no arguments
#define FNIF_SFIND 2	//Find a node following a path from a node (R) | S:filename, R:start_node
#define FNIF_SMKDIR 3	//Makes a directory (R) | S:filename, R:start_node

#define FNIF_GETNAME 0x10	//Get name of a node (S) | no arguments
#define FNIF_TYPE 0x11	//Get type of a node (I: NT_*) | no arguments
#define FNIF_GETPARENT 0x12 //Get parent of a node (R) | no arguments
#define FNIF_GETLENGTH 0x13	//Get length of contents of a node (I) | no arguments
#define FNIF_GETUID 0x14	//Get UID for a node (i) | no arguments
#define FNIF_GETGID 0x15	//Get GID for a node (i) | no arguments
#define FNIF_GETPERM 0x16	//Get permissions for a node (i) | no arguments
#define FNIF_GETPATH 0x17	//Get absolute path to node (S) | no arguments
#define FNIF_SETCWD 0x18	//Sets node as current working directory (v) | no arguments
#define FNIF_REMOVE 0x19	//Remove node (b) | no arguments

#define FNIF_GETIDXCHILD 0x20	//Get child node from index (R) | i:index
#define FNIF_GETNAMECHILD 0x21	//Get child node from name (R) | S:name

#endif
