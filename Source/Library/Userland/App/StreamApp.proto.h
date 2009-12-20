#ifndef DEF_STREAMAPP_PROTO_H
#define DEF_STREAMAPP_PROTO_H

#include "ShellApp.proto.h"

#include <IStream.proto.h>
#include <OStream.proto.h>

/*
 * This class implements basic utilities for apps that simply take some input, process it and output something.
 * Examples : cat, grep, ...
 */

class StreamApp : public ShellApp {
	protected:
	
	IStream *in;
	OStream *out;
	
	public:
	StreamApp(const String& name, const String& desc);
	~StreamApp();

	virtual void init();
};

#endif
