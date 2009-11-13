#ifndef DEF_VITRUALTERMINAL_IFACE_H
#define DEF_VITRUALTERMINAL_IFACE_H

#define VTIF_OBJTYPE 0x10

//S = static, GET = get, PR = process, IN/OUT : in/out, VT = virtualterminal
#define VTIF_SGETPRINVT 6
#define VTIF_SGETPROUTVT 7

#define VTIF_PUT 0x01
#define VTIF_WRITEHEX 0x02
#define VTIF_WRITEDEC 0x03
#define VTIF_WRITE 0x04

#define VTIF_READLINE 0x05
#define VTIF_GETKEYPRESS 0x06	//Takes two flags : 1<<0 = show, 1<<1 = block

#define VTIF_SETCOLOR 0x10
#define VTIF_SETCSRLINE 0x11
#define VTIF_SETCSRCOL 0x12
#define VTIF_ISBOXED 0x13

#define VTIF_GETHEIGHT 0x1A
#define VTIF_GETWIDTH 0x1B
#define VTIF_LOCATE 0x1C	//Takes line, col and sets cursor position

#endif
