#ifndef DEF_THREAD_IFACE_H
#define DEF_THREAD_IFACE_H

#define THIF_OBJTYPE 0x21

//S = static, GET = get, C = current, TH = thread
#define THIF_SGETCTH 0	//Get current thread (R) | no arguments

#define THIF_SLEEP 0x01	//Make thread sleep (v) | i:time in msecs
#define THIF_FINISH 0x02	//Make thread finish (v) | i:return_value

#endif
