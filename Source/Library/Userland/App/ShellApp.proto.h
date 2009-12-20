#ifndef DEF_SHELLAPP_CLASS_H
#define DEF_SHELLAPP_CLASS_H

#include <App/Application.proto.h>
#include <Binding/VirtualTerminal.class.h>

#define FT_BOOL 1
#define FT_STR 2
#define FT_INT 3

struct flag_t {
	WChar sName;
	String lName, desc;
	int type;
	String strVal;
	int intVal;
	bool boolVal;
};

class ShellApp : public Application {
	public:
	VirtualTerminal invt, outvt;
	Vector<String> args;
	Vector<flag_t> flags;
	String appName, appDesc;
	ShellApp(String name, String desc);
	~ShellApp();

	virtual void init();

	//Flag handling
	flag_t* getFlag(String name);
	void addFlag(WChar sName, String lName, String desc, int type = FT_BOOL, String deflt = "");
	String sFlag(String name);
	int iFlag(String name);
	bool bFlag(String name);
};

#endif

