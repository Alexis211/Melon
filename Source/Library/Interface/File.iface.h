#ifndef DEF_FILE_IFACE_H
#define DEF_FILE_IFACE_H

enum {
	FM_READ = 0,		//Open for read, put cursor at beginning
	FM_TRUNCATE = 1,	//Open for write, truncating file before
	FM_APPEND = 2,		//Open for write, put cursor at end
	FM_REPLACE = 3		//Open for write, put cursor at beginning
};

enum {
	SM_FORWARD = 0,		//Seek from actual position
	SM_BACKWARD = 1,	//Seek from actual position, backward
	SM_BEGINNING = 2,	//Seek from start of file
	SM_END = 3,			//Seek from end of file
};

#define FLIF_OBJTYPE 0x13

#define FLIF_SOPEN 0x01	//Open a file (R:ressource id of opened file) | S:filename, i:mode (FM_*), r:start_node

#define FLIF_CLOSE 0x05	//Close a file (v) | no arguments
#define FLIF_VALID 0x06	//True if a file is valid (b) | no arguments

#define FLIF_READ 0x0A	//Read from a file (i:number of bytes read) | i:length, *:data
#define FLIF_WRITE 0x0B	//Write to a file (b) | i:length, *:data

#define FLIF_SEEK 0x10	//Seek to a position in a file (b) | j:position, J:position, i:mode (SM_*)
#define FLIF_POSITION 0x11	//Get position in a file (I) | no arguments
#define FLIF_LENGTH 0x12	//Get length of a file (I) | no arguments
#define FLIF_EOF 0x13	//Are we at end of file (b) | no arguments

#endif
