

#ifndef __WOSH_WindowsMobile_Common_H__
 #define __WOSH_WindowsMobile_Common_H__

#define INITGUID

#include <windows.h>

#include <string>

//#include <aygshell.h>


#define ARRAYSIZE(s) (sizeof(s) / sizeof(s[0]))


void ConvertToString(const wchar_t* Src, std::string &target);


std::string getStdStringFrom( TCHAR* Src );


struct call_info {
	int line;
	std::string phone_number;
	std::string alias;

	DWORD _HCALL;
	
	enum CALL_STATE {
		IDLE = 0,
		RINGING = 2,
		CONNECTED = 16,
		DISCONNECTED = 32
	};
	
	CALL_STATE state;
};



#endif __WOSH_WindowsMobile_Common_H__