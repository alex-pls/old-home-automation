
#include "WindowsMobile.Common.h"

void ConvertToString(const wchar_t* Src, std::string &target)
{
	char* buffer = new char[wcslen(Src)+1];
	memset(buffer, 0, wcslen(Src)+1 );
	wcstombs(buffer, Src, wcslen(Src));
	target.assign( buffer );
	delete [] buffer;
}



std::string getStdStringFrom( TCHAR* Src )
 {
#ifndef UNICODE
	return std::string(Src);
#else
	std::string dest;
	ConvertToString(Src, dest);
	return dest;
#endif
 }


/*

        TCHAR szManufacturer[80]; 
        TCHAR szModel[80]; 
        TCHAR szRevision[80];
        TCHAR szSerialNumber[80];
        TCHAR szSubscriberNumber[80];
    IDS_CANTGETINFO         "Cannot get ExTAPI information."
    IDS_FRIENDLYNAME        "ExTAPI Sample"
    IDS_UNAVAILABLE         "[UNAVAILABLE]"
    IDS_EXTAPILINEINFO      "ExTAPI Line Info"
        LoadString(g_hInstance, IDS_MANUFACTURER, szManufacturer, ARRAYSIZE(szManufacturer)); 
        LoadString(g_hInstance, IDS_MODEL, szModel, ARRAYSIZE(szModel)); 
        LoadString(g_hInstance, IDS_REVISION, szRevision, ARRAYSIZE(szRevision)); 
        LoadString(g_hInstance, IDS_SERIALNUMBER, szSerialNumber, ARRAYSIZE(szSerialNumber)); 
        LoadString(g_hInstance, IDS_SUBSCRIBERNUMBER, szSubscriberNumber, ARRAYSIZE(szSubscriberNumber)); 

        _sntprintf(szMBString, ARRAYSIZE(szMBString),
                   TEXT("%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s\n"),
                   szManufacturer, tsManufacturer, 
                   szModel, tsModel, 
                   szRevision, tsRevision, 
                   szSerialNumber, tsSerialNumber, 
                   szSubscriberNumber, tsSubscriberNumber);


*/
















