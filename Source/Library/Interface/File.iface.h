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

#define FLIF_SOPEN 0x01

#define FLIF_CLOSE 0x05
#define FLIF_VALID 0x06

#define FLIF_READ 0x0A
#define FLIF_WRITE 0x0B

#define FLIF_SEEK 0x10
#define FLIF_POSITION 0x11
#define FLIF_LENGTH 0x12
#define FLIF_EOF 0x13

#endif
