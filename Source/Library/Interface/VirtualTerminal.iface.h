#ifndef DEF_VITRUALTERMINAL_IFACE_H
#define DEF_VITRUALTERMINAL_IFACE_H

#define VTIF_OBJTYPE 0x10

//S = static, GET = get, PR = process, IN/OUT : in/out, VT = virtualterminal
#define VTIF_SGETPRINVT 6	//Get process input virtual terminal (R) | no arguments
#define VTIF_SGETPROUTVT 7	//Get process output virtual terminal (R) | no arguments

#define VTIF_PUT 0x01	//Put one character to virtual terminal (v) | c:character
#define VTIF_WRITEHEX 0x02	//Write a number in hexadecimal (v) | i:number
#define VTIF_WRITEDEC 0x03	//Write a number in decimal (v) | j:number, J:number
#define VTIF_WRITE 0x04	//Write a string (v) | S:string

#define VTIF_READLINE 0x05	//Reads a line from virtual terminal (S) : b:show?
#define VTIF_GETKEYPRESS 0x06	//Get a keypress from virtual terminal (*) : i:flags
								//Takes two flags : 1<<0 = show, 1<<1 = block

#define VTIF_SETCOLOR 0x10	//Set text color (v) | i:foreground_color, i:backgrond_color
#define VTIF_SETCSRLINE 0x11	//Set cursor line (v) | i:line
#define VTIF_SETCSRCOL 0x12	//Set cursor column (v) | i:column
#define VTIF_ISBOXED 0x13	//Is VT boxed ? (b) | no arguments

#define VTIF_GETHEIGHT 0x1A	//Get VT width (i) | no arguments
#define VTIF_GETWIDTH 0x1B	//Get VT height (i) | no arguments
#define VTIF_LOCATE 0x1C	//Sets cursor position (v) | i:line, i:column

#endif
