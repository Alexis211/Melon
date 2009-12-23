#ifndef DEF_VITRUALTERMINAL_IFACE_H
#define DEF_VITRUALTERMINAL_IFACE_H

#define VTIF_OBJTYPE 0x10

//S = static, GET = get, PR = process, IN/OUT : in/out, VT = virtualterminal
#define VTIF_SGETPRINVT 6	//Get process input virtual terminal (R) | no arguments
#define VTIF_SGETPROUTVT 7	//Get process output virtual terminal (R) | no arguments

#define VTIF_PUT 0x01	//Put one character to virtual terminal (v) | c:character
#define VTIF_WRITE 0x02	//Write a string (v) | S:string

/*		EVERYTHING NOW GOES THROUGH VTIF_WRITE
#define VTIF_WRITEHEX 0x03	//Write a number in hexadecimal (v) | i:number
#define VTIF_WRITEDEC 0x04	//Write a number in decimal (v) | j:number, J:number
*/

#define VTIF_READLINE 0x05	//Reads a line from virtual terminal (S) : b:show?
#define VTIF_GETKEYPRESS 0x06	//Get a keypress from virtual terminal (*) : i:flags
								//Takes two flags : 1<<0 = show, 1<<1 = block
/*		EVERYTHING NOW GOES THROUGH VTIF_WRITE
#define VTIF_SETCOLOR 0x10	//Set text color (v) | i:foreground_color, i:backgrond_color
#define VTIF_SETCSRLINE 0x11	//Set cursor line (v) | i:line
#define VTIF_SETCSRCOL 0x12	//Set cursor column (v) | i:column
#define VTIF_LOCATE 0x13	//Sets cursor position (v) | i:line, i:column
*/
#define VTIF_ISBOXED 0x13	//Is VT boxed ? (b) | no arguments

#define VTIF_GETHEIGHT 0x1A	//Get VT width (i) | no arguments
#define VTIF_GETWIDTH 0x1B	//Get VT height (i) | no arguments

//		*****			MELON VT SPECIFIC ESCAPE COMMANDS      **
#define MVT_ESC "\032"

struct mvt_esc_cmd_t {
	int cmd;
	int a, b;
};

#define MVTCMD0(name, command) \
	inline mvt_esc_cmd_t name() { \
		mvt_esc_cmd_t ret;	ret.cmd = command;	return ret;	}

#define MVTCMD1(name, command) \
	inline mvt_esc_cmd_t name(int a) { \
		mvt_esc_cmd_t ret;	ret.cmd = command;	ret.a = a;	return ret;	}

#define MVTCMD2(name, command) \
	inline mvt_esc_cmd_t name(int a, int b) { \
		mvt_esc_cmd_t ret;	ret.cmd = command;	ret.a = a;	ret.b = b;	return ret;	}

#define MVTCMD_CLEAR 0x100
#define MVTCMD_SCROLL 0x101
#define MVTCMD_SETCOLOR 0x110
#define MVTCMD_SETBGCOLOR 0x111
#define MVTCMD_SETFGCOLOR 0x112
#define MVTCMD_MOVECSR 0x120
#define MVTCMD_SETCSRLIN 0x121
#define MVTCMD_SETCSRCOL 0x122
#define MVTCMD_HIDECSR 0x130
#define MVTCMD_SHOWCSR 0x131

namespace MVT {
	MVTCMD0(clear, MVTCMD_CLEAR);
	MVTCMD0(scroll, MVTCMD_SCROLL);
	MVTCMD2(setcolor, MVTCMD_SETCOLOR);
	MVTCMD1(setbgcolor, MVTCMD_SETBGCOLOR);
	MVTCMD1(setfgcolor, MVTCMD_SETFGCOLOR);
	MVTCMD2(movecsr, MVTCMD_MOVECSR);
	MVTCMD1(setcsrlin, MVTCMD_SETCSRLIN);
	MVTCMD1(setcsrcol, MVTCMD_SETCSRCOL);
	MVTCMD0(hidecsr, MVTCMD_HIDECSR);
	MVTCMD0(showcsr, MVTCMD_SHOWCSR);
}

#endif
