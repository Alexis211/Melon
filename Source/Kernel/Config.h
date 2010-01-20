#ifndef DEF_MELON_KERNEL_CONFIG
#define DEF_MELON_KERNEL_CONFIG

//Undefine if you don't want debugging. This doesn't really do anything
#define OPT_DEBUG

//******************************
//								General options
//													*********************************
#define DEFAULT_INIT "/System/Applications/PaperWork.app"
#define DEFAULT_ROOT "ramfs:0"
#define DEFAULT_ENABLEVESA true			//enable VESA by default ?

//******************************  
//								Color scheme
//								 					*********************************
#define TXTLOGO_FGCOLOR 9	//Colors for the melon logo
#define TXTLOGO_BGCOLOR 0

#define STATUSBAR_BGCOLOR 7
#define STATUSBAR_FGCOLOR 0

#define KVT_FGCOLOR 7		//Colors for the vt for selecting a screen mode
#define KVT_BGCOLOR 0
#define KVT_LIGHTCOLOR 9
#define KVT_ENTRYCOLOR 6

//These colors are used for the shell running the init program
#define SHELL_FGCOLOR 0
#define SHELL_BGCOLOR 7

#define PANIC_BGCOLOR 4	//Colors for the panic screen
#define PANIC_FGCOLOR 15
#define BSOD_BGCOLOR 1		//Colors for the panic_dump screen
#define BSOD_FGCOLOR 7
#define HALT_BGCOLOR 6		//Colors for the halt screen
#define HALT_FGCOLOR 7

#endif
