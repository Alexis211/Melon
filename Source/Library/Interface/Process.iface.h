#ifndef DEF_PROCESS_IFACE_H
#define DEF_PROCESS_IFACE_H

#define E_AUTODELETE 0x0FFFFF03

#define PRIF_OBJTYPE 0x20

//S = static, GET = get, C = current, PR = process
#define PRIF_SGETCPR 0	//Get current process (R) | no arguments
#define PRIF_SRUN 1	//Run a new process (R) | S:executable_name
#define PRIF_SWAIT 2	//Wait for a process to end (i:return value) | R:process to wait for

#define PRIF_EXIT 0x01	//Exit from current process (v) | no arguments
#define PRIF_ALLOCPAGES 0x02	//Allocate frames for pages (v) | i:position, i:count
#define PRIF_FREEPAGES 0x03	//Free frames for pages (v) | i:position, i:count
#define PRIF_GETPID 0x04	//Get PID of a process (i) | no arguments
#define PRIF_GETPPID 0x05	//Get PPID of a process (i) | no arguments
#define PRIF_GETUID 0x06	//Get UID of a process (i) | no arguments

#define PRIF_ARGC 0x10	//Get argument count for a process (i) | no arguments
#define PRIF_ARGV 0x11	//Get argument value for a process (S) | i:argument_index

#define PRIF_START 0x20	//Start execution of a child process (v) | no arguments
#define PRIF_AUTODELETE 0x21	//Set a child to auto-delete itself when it ends (v) | b:does it autodelete?
#define PRIF_PUSHARG 0x28	//Push an argument to a child process (v) | S:argument
#define PRIF_SETOUTVT 0x29	//Set output VirtualTerminal for a child process (v) | r:virtual terminal
#define PRIF_SETINVT 0x30	//Set input VirtualTerminal for a child process (v) | r:virtual terminal

//Authenticate with password/without password (being in group root)
#define PRIF_AUTHPW 0x40	//Authenticate current process to a user (b:succeeded?) | S:username, S:password
#define PRIF_AUTHNOPW 0x41	//Authenticate to a user without password, root only (b:succeeded?) | S:username

#endif
