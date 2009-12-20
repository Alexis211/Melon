#ifndef DEF_SB_NS_H
#define DEF_SB_NS_H

//StatusBar namespace :)
#include <VTManager/SimpleVT.class.h>

namespace SB {
	void init();	//Setup status bar with default 80col configuration
	void reinit();	//Setup status bar querying Disp:: for text screen width
	void gomulti();	//Launch new thread for status bar
	void progress(const String& s);	//Update progress
	void message(const String& s);	//Set a message

}

#endif
