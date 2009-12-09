Descriptions of functions are defined in the .iface.h files.

The ????_OBJTYPE defines represent the class identifier for that class.
The ????_S* defines are IDs for function syscalls : they do not apply to an object
All the other defines are IDs for method syscalls, that apply to one objec of that type

The description of a function/method is formatted as follows :
//<role of function/method> (<return type>[:<what is returned>]) | [<arg type>:<arg name> [, ...] ]
The return type and the arguments type can be one of :
- v : void (for return type)
- b : bool (0 = false, anything = true)
- c : a character
- S : a pointer to a String object
- i : an integer
- I : a pointer to a u64int
- j/J : lower/higher half of a u64int
- r : an identifier for any ressource object
- R : an identifier for a ressource object, but of same class
- * : a pointer to some space (u8int*)
