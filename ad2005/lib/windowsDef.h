#ifndef WINDOWSDEF_H
#define WINDOWSDEF_H

#if defined(WIN32) || defined(WINDOWS) || defined(_WIN32) || defined(__WIN32__)

#ifndef WIN32
#define WIN32
#endif 

#include <windows.h>
#include <direct.h>

#define getcwd		_getcwd
#define	strcasecmp	_stricmp
#define	strncasecmp	_strnicmp
#endif


#endif
