//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#include "WindowsMobile.Common.h"

#include <astdtapi.h>
// required lib: cellcore.lib 

// tapi stuff
#include "stringutils.h"


typedef void (*MethodTapi_CallChanged_Callback)(const struct call_info& call );





int DialNumber_( LPTSTR phone_number );








int DialNumber( const std::string& phone_number )
 {
	//TCHAR	gszDefaultNum[] = TEXT("0123456789");
	//LPTSTR gpszPhoneNum = gszDefaultNum;
#ifndef UNICODE
	LPTSTR gpszPhoneNum = (TCHAR*)phone_number.c_str();
#else
	std::wstring widestr = std::wstring(phone_number.begin(), phone_number.end());
	LPTSTR gpszPhoneNum = (LPTSTR)widestr.c_str();
#endif
	return DialNumber_( gpszPhoneNum );
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int DialNumber_( LPTSTR phone_number )
{
	LONG lResult;
	lResult = tapiRequestMakeCall(phone_number, NULL, NULL, NULL);
	return (int)lResult;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// example from http://nah6.com/~itsme/cvs-xdadevtools/itsutils/leds/tsttapi.cpp

MethodTapi_CallChanged_Callback call_changed_callback = NULL;
call_info call;

HLINEAPP hLineApp;
bool g_bOriginating;
bool g_echo_active;

DWORD dwNumLines = 0;
DWORD dwAPIVersion = TAPI_CURRENT_VERSION;
DWORD dwExtVersion = 0;

HLINE g_hLine;
bool voice= false;

//bool v32= false;


// check following
char *tapierrors[]= {
	"ALLOCATED",  "BADDEVICEID",  "BEARERMODEUNAVAIL",  "tapierror_0004",  "CALLUNAVAIL",  "COMPLETIONOVERRUN",  "CONFERENCEFULL",  "DIALBILLING",  "DIALDIALTONE",  "DIALPROMPT",  "DIALQUIET",  "INCOMPATIBLEAPIVERSION",  "INCOMPATIBLEEXTVERSION",  "INIFILECORRUPT",  "INUSE",  "INVALADDRESS",  "INVALADDRESSID",  "INVALADDRESSMODE",  "INVALADDRESSSTATE",  "INVALAPPHANDLE",  "INVALAPPNAME",  "INVALBEARERMODE",  "INVALCALLCOMPLMODE",  "INVALCALLHANDLE",  "INVALCALLPARAMS",  "INVALCALLPRIVILEGE",  "INVALCALLSELECT",  "INVALCALLSTATE",  "INVALCALLSTATELIST",  "INVALCARD",  "INVALCOMPLETIONID",  "INVALCONFCALLHANDLE",  "INVALCONSULTCALLHANDLE",  "INVALCOUNTRYCODE",  "INVALDEVICECLASS",  "INVALDEVICEHANDLE",  "INVALDIALPARAMS",  "INVALDIGITLIST",  "INVALDIGITMODE",  "INVALDIGITS",  "INVALEXTVERSION",  "INVALGROUPID",  "INVALLINEHANDLE",  "INVALLINESTATE",  "INVALLOCATION",  "INVALMEDIALIST",  "INVALMEDIAMODE",  "INVALMESSAGEID",  "tapierror_0030",  "INVALPARAM",  "INVALPARKID",  "INVALPARKMODE",  "INVALPOINTER",  "INVALPRIVSELECT",  "INVALRATE",  "INVALREQUESTMODE",  "INVALTERMINALID",  "INVALTERMINALMODE",  "INVALTIMEOUT",  "INVALTONE",  "INVALTONELIST",  "INVALTONEMODE",  "INVALTRANSFERMODE",  "LINEMAPPERFAILED",  "NOCONFERENCE",  "NODEVICE",  "NODRIVER",  "NOMEM",  "NOREQUEST",  "NOTOWNER",  "NOTREGISTERED",  "OPERATIONFAILED",  "OPERATIONUNAVAIL",  "RATEUNAVAIL",  "RESOURCEUNAVAIL",  "REQUESTOVERRUN",  "STRUCTURETOOSMALL",  "TARGETNOTFOUND",  "TARGETSELF",  "UNINITIALIZED",  "USERUSERINFOTOOBIG",  "REINIT",  "ADDRESSBLOCKED",  "BILLINGREJECTED",  "INVALFEATURE",  "NOMULTIPLEINSTANCE",  "INVALAGENTID",  "INVALAGENTGROUP",  "INVALPASSWORD",  "INVALAGENTSTATE",  "INVALAGENTACTIVITY",  "DIALVOICEDETECT"
 };

#define ERRSTRING(rc) ( rc==0?"OK" : ((DWORD)rc)<0x80000001 ? "strangecode" : ((DWORD)rc-0x80000001)>=(sizeof(tapierrors)/sizeof(*tapierrors)) ? "largecode" : tapierrors[rc-0x80000001] )

char *tapimsgs[] = {"LINE_ADDRESSSTATE", "LINE_CALLINFO", "LINE_CALLSTATE", "LINE_CLOSE", "LINE_DEVSPECIFIC", "LINE_DEVSPECIFICFEATURE", "LINE_GATHERDIGITS", "LINE_GENERATE", "LINE_LINEDEVSTATE", "LINE_MONITORDIGITS", "LINE_MONITORMEDIA", "LINE_MONITORTONE", "LINE_REPLY", "LINE_REQUEST", "PHONE_BUTTON", "PHONE_CLOSE", "PHONE_DEVSPECIFIC", "PHONE_REPLY", "PHONE_STATE", "LINE_CREATE", "PHONE_CREATE", "LINE_AGENTSPECIFIC", "LINE_AGENTSTATUS", "LINE_APPNEWCALL", "LINE_PROXYREQUEST", "LINE_REMOVE", "PHONE_REMOVE" };
#define MSGSTRING(msg) ( (msg<0 || msg>=(sizeof(tapimsgs)/sizeof(*tapimsgs))) ? "unknown_msg" : tapimsgs[msg] )

char *ownertype[]= { "-", "NONE", "MONITOR", "NONE|MONITOR", "OWNER", "OWNER|NONE", "OWNER|MONITOR", "OWNER|MONITOR|NONE" };
#define OWNERSTRING(o) ( (o<0 || o>=(sizeof(ownertype)/sizeof(*ownertype))) ? "unknown_owner" : ownertype[o] )

std::Wstring tapistring(ByteVector& bv, DWORD ofs, DWORD size)
{
	if (size>1 && bv[ofs+1])
		return ToWString(stringformat("ch_%d:", size)+std::string((char*)(vectorptr(bv)+ofs), (char*)(vectorptr(bv)+ofs+size)));
	else
		return ToWString(stringformat("wc_%d:", size))+std::Wstring((WCHAR*)(vectorptr(bv)+ofs), (WCHAR*)(vectorptr(bv)+ofs+size));
}
std::string tapistringlist(ByteVector& bv, DWORD ofs, DWORD size)
{
    StringList l;
    std::string s;
    while (size) {
        if (bv[ofs])
            s += (char)bv[ofs];
        else {
            l.push_back(s);
            s.erase();
        }

        ofs +=2;
        size-=2;
    }
    return JoinStringList(l, ";");
}
std::Wstring tapidata(ByteVector& bv, DWORD ofs, DWORD size)
{
	return ToWString(hexdump(vectorptr(bv)+ofs, size));
}

std::string BearerModesString(DWORD dw)
{
	StringList l;
	if (dw&LINEBEARERMODE_VOICE           ) l.push_back("VOICE");
	if (dw&LINEBEARERMODE_SPEECH          ) l.push_back("SPEECH");
	if (dw&LINEBEARERMODE_MULTIUSE        ) l.push_back("MULTIUSE");
	if (dw&LINEBEARERMODE_DATA            ) l.push_back("DATA");
	if (dw&LINEBEARERMODE_ALTSPEECHDATA   ) l.push_back("ALTSPEECHDATA");
	if (dw&LINEBEARERMODE_NONCALLSIGNALING) l.push_back("NONCALLSIGNALING");
	if (dw&LINEBEARERMODE_PASSTHROUGH     ) l.push_back("PASSTHROUGH");
	if (dw&LINEBEARERMODE_RESTRICTEDDATA  ) l.push_back("RESTRICTEDDATA");
	if (dw&~0xff) l.push_back(stringformat("bearermode_%08lx", dw&~0xff));

	return JoinStringList(l,",");
}
std::string MediaModesString(DWORD dw)
{
	StringList l;
	if (dw&LINEMEDIAMODE_UNKNOWN         ) l.push_back("UNKNOWN");
	if (dw&LINEMEDIAMODE_INTERACTIVEVOICE) l.push_back("INTERACTIVEVOICE");
	if (dw&LINEMEDIAMODE_AUTOMATEDVOICE  ) l.push_back("AUTOMATEDVOICE");
	if (dw&LINEMEDIAMODE_DATAMODEM       ) l.push_back("DATAMODEM");
	if (dw&LINEMEDIAMODE_G3FAX           ) l.push_back("G3FAX");
	if (dw&LINEMEDIAMODE_TDD             ) l.push_back("TDD");
	if (dw&LINEMEDIAMODE_G4FAX           ) l.push_back("G4FAX");
	if (dw&LINEMEDIAMODE_DIGITALDATA     ) l.push_back("DIGITALDATA");
	if (dw&LINEMEDIAMODE_TELETEX         ) l.push_back("TELETEX");
	if (dw&LINEMEDIAMODE_VIDEOTEX        ) l.push_back("VIDEOTEX");
	if (dw&LINEMEDIAMODE_TELEX           ) l.push_back("TELEX");
	if (dw&LINEMEDIAMODE_MIXED           ) l.push_back("MIXED");
	if (dw&LINEMEDIAMODE_ADSI            ) l.push_back("ADSI");
	if (dw&LINEMEDIAMODE_VOICEVIEW       ) l.push_back("VOICEVIEW");
	if (dw&~0x7fff) l.push_back(stringformat("mediamode_%08lx", dw&~0x7fff));

	return JoinStringList(l, ",");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool dropcall(HCALL hCall)
{
	LONG rc= lineSetCallPrivilege(hCall, LINECALLPRIVILEGE_OWNER);
	if ( rc<0 )
		printf("WM6:Dialing:dropcall() : ERROR: lineSetCallPrivilege(%08lx, owner) : %s\n", hCall, ERRSTRING(rc));
	else
		printf("WM6:Dialing:dropcall() : reqid=%08lx - callpriv set to owner\n", rc);
	printf("WM6:Dialing:dropcall() : dropping call %08lx\n", hCall);
	rc= lineDrop(hCall, NULL, 0);
	if ( rc<0 ) {
		printf("WM6:Dialing:dropcall() : ERROR: lineDrop(%08lx) : %s\n", hCall, ERRSTRING(rc));
		return false;
	 }
	printf("WM6:Dialing:dropcall() : reqid=%08lx lineDrop(%08lx) - ok\n", rc, hCall);
	return true;
}
bool deallocatecall(HCALL hCall)
{
	printf("WM6:Dialing:deallocating() : call %08lx\n", hCall);
	LONG rc= lineDeallocateCall(hCall);
	if ( rc<0 ) {
		printf("WM6:Dialing:deallocating() : ERROR lineDeallocateCall(%08lx) : %s\n", hCall, ERRSTRING(rc));
		return false;
	 }
	printf("WM6:Dialing:deallocating() : reqid=%08lx: call-%08lx deallocated\n", hCall, rc);
	MessageBeep(MB_ICONASTERISK);
	return true;
}
bool answercall(HCALL hCall)
{
	printf("WM6:Dialing:answercall() : answering call %08lx\n", hCall);
	LONG rc= lineAnswer(hCall, NULL, 0);
	if ( rc<0 ) {
		printf("WM6:Dialing:answercall() : ERROR lineAnswer: %s\n", ERRSTRING(rc));
		return false;
	 }
	printf("WM6:Dialing:answercall() : reqid=%08lx: call answered\n", rc);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void genTone(HCALL hCall)
{
	LINEGENERATETONE lgt;
	lgt.dwFrequency = 10;
	lgt.dwCadenceOn = 100;
	lgt.dwCadenceOff = 2000;
	lgt.dwVolume = 0x0000FFFF;

	long lErr = 0;
	///////////////////
	lErr = lineGenerateTone(hCall, LINETONEMODE_CUSTOM,0,1,&lgt);
	///////////////////
	lErr = lineGenerateTone(hCall,LINETONEMODE_BEEP,-1,0,NULL);
	////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID FAR PASCAL WM_lineCallbackFunc( 
  DWORD hDevice, 
  DWORD dwMsg, 
  DWORD dwCallbackInstance, 
  DWORD dwParam1, DWORD dwParam2, DWORD dwParam3
)
 {
	long ret=0;
	call._HCALL = dwParam2; // refresh HCALL

	switch(dwMsg) {
		case LINE_APPNEWCALL: { //  handle to a call is available
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_APPNEWCALL : hLine=%08lx %s inst_%08lx : addrid=%08lx hCall=%08lx priv=%s\n",
						hDevice, MSGSTRING(dwMsg), dwCallbackInstance, 
						dwParam1, dwParam2, OWNERSTRING(dwParam3));

				call.state = call_info::IDLE;
				long ret = lineSetCallPrivilege((HCALL)dwParam2, LINECALLPRIVILEGE_OWNER);
if ( call_changed_callback != NULL )
	(call_changed_callback)(call);
				break;
			}
		case LINE_CALLINFO: {
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CALLINFO\n");
//				printf("msg hCall=%08lx %s inst_%08lx : state=%s unused:%d,%d\n",
//						hDevice, MSGSTRING(dwMsg), dwCallbackInstance, 
//						CallInfoStateString(dwParam1).c_str(), dwParam2, dwParam3);
//if (dwParam1!=LINECALLINFOSTATE_NUMMONITORS && dwParam1!=LINECALLINFOSTATE_NUMOWNERINCR && dwParam1!=LINECALLINFOSTATE_NUMOWNERDECR)
//	readinfo((HCALL)hDevice, dwParam1);

				if ( dwParam1 == LINECALLINFOSTATE_CALLERID ) { // CallerID is available
					LINECALLINFO *lpCallInfo = NULL;
/* UNDONE :*/
/* Very dirty allocation of memory! */
/* It might cause bugs, please change this */
/* Make this a function*/
					lpCallInfo = (LINECALLINFO *)malloc(sizeof(LINECALLINFO)+1024/* ! */);
					memset(lpCallInfo, 0, sizeof(LINECALLINFO)+1024);

					lpCallInfo->dwTotalSize = sizeof(LINECALLINFO)+1024;

					lineGetCallInfo( (HCALL)call._HCALL, lpCallInfo);

					if (lpCallInfo->dwTotalSize < lpCallInfo->dwNeededSize) {
						lpCallInfo = (LINECALLINFO *)realloc(lpCallInfo, lpCallInfo->dwNeededSize);
						lineGetCallInfo( (HCALL)call._HCALL , lpCallInfo);
					 }

					LPSTR pID = (LPSTR)lpCallInfo; // addr of the structure
					pID += lpCallInfo->dwCallerIDOffset;

//					TCHAR CallerID[TAPIMAXDESTADDRESSSIZE];
//					memset(CallerID,0,TAPIMAXDESTADDRESSSIZE);
//					memcpy((char*)CallerID, pID, lpCallInfo->dwCallerIDSize);
//					call.phone_number = getStdStringFrom(CallerID);

char szCallerID [ TAPIMAXDESTADDRESSSIZE ] = "";
//memset(szCallerID,0,TAPIMAXDESTADDRESSSIZE);
strncpy_s ( szCallerID, TAPIMAXDESTADDRESSSIZE ,
(char*)(((BYTE)lpCallInfo)+(lpCallInfo->dwCallerIDOffset)), lpCallInfo->dwCallerIDSize );
call.phone_number = std::string(szCallerID);


call.state = call_info::RINGING;

					printf("WM6:Dialing:lineCallbackFunc(dwMsg) The ID is: %s\n", call.phone_number.c_str() );

if ( call_changed_callback != NULL )
	(call_changed_callback)(call);

				 }


				break;
			}
		case LINE_CALLSTATE: {
//            printf("msg hCall=%08lx %s inst_%08lx : state=%s detail=%s priv=%s\n",
//                hDevice, MSGSTRING(dwMsg), dwCallbackInstance, 
//                CallStateString(dwParam1).c_str(), CallDetailString(dwParam1, dwParam2).c_str(), OWNERSTRING(dwParam3));

				if ( dwParam1 == LINECALLSTATE_OFFERING ) {
					printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CALLSTATE/OFFERING \n");
//OK,works answercall( (HCALL)call._HCALL );

call.state = call_info::RINGING;
if ( call_changed_callback != NULL )
	(call_changed_callback)(call);

				 }
				else if ( dwParam1 == LINECALLSTATE_CONNECTED ) { // A connection established
					printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CALLSTATE/CONNECTED \n");
					// Start listening to touchtones from user
					ret = lineMonitorDigits( (HCALL)call._HCALL, LINEDIGITMODE_DTMF);

call.state = call_info::CONNECTED;
if ( call_changed_callback != NULL )
	(call_changed_callback)(call);

					if (ret == LINEERR_INVALCALLHANDLE ) {
						printf("WM6:Dialing:lineMonitorDigits(dwMsg) : FAILED %i\n", ret);
						return;
					}
				 }
				else if ( dwParam1 == LINECALLSTATE_DISCONNECTED ) { // The remote party has disconnected from the call
					printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CALLSTATE/DISCONNECTED \n");

call.state = call_info::DISCONNECTED;
if ( call_changed_callback != NULL )
	(call_changed_callback)(call);

					if ( !g_bOriginating )
						deallocatecall((HCALL)hDevice);
					g_echo_active= false;
				 }
				else {
					printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CALLSTATE/UNKNOWN-%l\n", dwParam1);
				}

				break;
			}
		case LINE_MONITORDIGITS: { // The other party pressed a digit
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_MONITORDIGITS\n");
				if ( dwParam2 == LINEDIGITMODE_DTMF ) {
					char tone = LOBYTE(dwParam1);
					printf("WM6:Dialing:lineCallbackFunc(dwMsg) : %i\n", (int)tone );
				}
				break;
			}
		case LINE_MONITORTONE: {
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_MONITORTONE\n");
				// ?
				break;
			}
		case LINE_LINEDEVSTATE: {
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_LINEDEVSTATE\n");
				if ( dwParam1 == LINEDEVSTATE_RINGING ) // Ringing, please pick up the phone!
					;

				break;
			}
		case LINE_CLOSE: { // should we shutdown?
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_CLOSE\n");

				break;
			}
		case LINE_REPLY: { // No! probably an error occurred.
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : LINE_REPLY\n");
//            printf("msg  %s inst_%08lx : reqid=%08lx err=%s unused:%d,%d\n",
//                MSGSTRING(dwMsg), dwCallbackInstance, 
//                dwParam1, ERRSTRING(dwParam2), hDevice, dwParam3);
				break;
			}
		default: { // Unknown state?
				printf("WM6:Dialing:lineCallbackFunc(dwMsg) : UNKNOWN\n");

				break;
			}
	 }

 }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool devopen(int dwDeviceID)
{
    LINEEXTENSIONID extid;
#define TAPI_MIN_VERSION 0x00010003
    LONG rc = lineNegotiateAPIVersion(hLineApp, dwDeviceID, TAPI_MIN_VERSION, dwAPIVersion, &dwAPIVersion, &extid);
    if (rc<0) {
        printf("WM6:Dialing:devopen() : ERROR: lineNegotiateAPIVersion(%08lx, %08lx) : %s\n", hLineApp, dwDeviceID, ERRSTRING(rc));
        return false;
    }
    printf("WM6:Dialing:devopen() : reqid=%08lx apiver-%d: %08lx ext: %08lx %08lx %08lx %08lx\n", rc, dwDeviceID, dwAPIVersion, 
            extid.dwExtensionID0, extid.dwExtensionID1, extid.dwExtensionID2, extid.dwExtensionID3);
    return true;
}
bool get_devcaps(int dwDeviceID)
{
    ByteVector bv; bv.resize(sizeof(LINEDEVCAPS));
    LINEDEVCAPS*dc= (LINEDEVCAPS*)vectorptr(bv);
    dc->dwTotalSize= bv.size();
    LONG rc= lineGetDevCaps(hLineApp, dwDeviceID, dwAPIVersion, dwExtVersion, dc);
    if (rc<0 && rc!=LINEERR_STRUCTURETOOSMALL) {
        printf("ERROR1: lineGetDevCaps(%08lx, %08ld, %08lx, %08lx) : %s\n", hLineApp, dwDeviceID, dwAPIVersion, dwExtVersion, ERRSTRING(rc));
        return false;
    }
    else if (rc==LINEERR_STRUCTURETOOSMALL || dc->dwNeededSize > dc->dwTotalSize) {
        printf("%08lx lineGetDevCaps#1: total=%04x need=%04x use=%04x\n", rc, dc->dwTotalSize, dc->dwNeededSize, dc->dwUsedSize);
        bv.resize(dc->dwNeededSize);
        dc= (LINEDEVCAPS*)vectorptr(bv);
        dc->dwTotalSize= bv.size();
        rc= lineGetDevCaps(hLineApp, dwDeviceID, dwAPIVersion, dwExtVersion, dc);
        if (rc<0) {
            printf("ERROR2: lineGetDevCaps(%08lx, %08ld, %08lx, %08lx) : %s\n", hLineApp, dwDeviceID, dwAPIVersion, dwExtVersion, ERRSTRING(rc));
            return false;
        }
    }

    printf("reqid=%08lx lineGetDevCaps(%d)\n", rc, dwDeviceID);
    printf("dc.dwTotalSize = %08lx\n", dc->dwTotalSize);
    printf("dc.dwNeededSize = %08lx\n", dc->dwNeededSize);
    printf("dc.dwUsedSize = %08lx\n", dc->dwUsedSize);
    printf("dc.dwProviderInfo: %ls\n", tapistring(bv, dc->dwProviderInfoOffset, dc->dwProviderInfoSize).c_str());
    printf("dc.dwSwitchInfo: %ls\n", tapistring(bv, dc->dwSwitchInfoOffset, dc->dwSwitchInfoSize).c_str());
    printf("dc.dwPermanentLineID = %08lx\n", dc->dwPermanentLineID);
    printf("dc.dwLineName: %ls\n", tapistring(bv, dc->dwLineNameOffset, dc->dwLineNameSize).c_str());
    printf("dc.dwStringFormat = %08lx\n", dc->dwStringFormat);
    printf("dc.dwAddressModes = %08lx\n", dc->dwAddressModes);
    printf("dc.dwNumAddresses = %08lx\n", dc->dwNumAddresses);
    printf("dc.dwBearerModes = %s\n", BearerModesString(dc->dwBearerModes).c_str());
    printf("dc.dwMaxRate = %08lx\n", dc->dwMaxRate);
    printf("dc.dwMediaModes = %s\n", MediaModesString(dc->dwMediaModes).c_str());
    printf("dc.dwGenerateToneModes = %08lx\n", dc->dwGenerateToneModes);
    printf("dc.dwGenerateToneMaxNumFreq = %08lx\n", dc->dwGenerateToneMaxNumFreq);
    printf("dc.dwGenerateDigitModes = %08lx\n", dc->dwGenerateDigitModes);
    printf("dc.dwMonitorToneMaxNumFreq = %08lx\n", dc->dwMonitorToneMaxNumFreq);
    printf("dc.dwMonitorToneMaxNumEntries = %08lx\n", dc->dwMonitorToneMaxNumEntries);
    printf("dc.dwMonitorDigitModes = %08lx\n", dc->dwMonitorDigitModes);
    printf("dc.dwGatherDigitsMinTimeout = %08lx\n", dc->dwGatherDigitsMinTimeout);
    printf("dc.dwGatherDigitsMaxTimeout = %08lx\n", dc->dwGatherDigitsMaxTimeout);
    printf("dc.dwMedCtlDigitMaxListSize = %08lx\n", dc->dwMedCtlDigitMaxListSize);
    printf("dc.dwMedCtlMediaMaxListSize = %08lx\n", dc->dwMedCtlMediaMaxListSize);
    printf("dc.dwMedCtlToneMaxListSize = %08lx\n", dc->dwMedCtlToneMaxListSize);
    printf("dc.dwMedCtlCallStateMaxListSize = %08lx\n", dc->dwMedCtlCallStateMaxListSize);
    printf("dc.dwDevCapFlags = %08lx\n", dc->dwDevCapFlags);
    printf("dc.dwMaxNumActiveCalls = %08lx\n", dc->dwMaxNumActiveCalls);
    printf("dc.dwAnswerMode = %08lx\n", dc->dwAnswerMode);
    printf("dc.dwRingModes = %08lx\n", dc->dwRingModes);
    printf("dc.dwLineStates = %08lx\n", dc->dwLineStates);
    printf("dc.dwUUIAcceptSize = %08lx\n", dc->dwUUIAcceptSize);
    printf("dc.dwUUIAnswerSize = %08lx\n", dc->dwUUIAnswerSize);
    printf("dc.dwUUIMakeCallSize = %08lx\n", dc->dwUUIMakeCallSize);
    printf("dc.dwUUIDropSize = %08lx\n", dc->dwUUIDropSize);
    printf("dc.dwUUISendUserUserInfoSize = %08lx\n", dc->dwUUISendUserUserInfoSize);
    printf("dc.dwUUICallInfoSize = %08lx\n", dc->dwUUICallInfoSize);
    //LINEDIALPARAMS  MinDialParams;
    //LINEDIALPARAMS  MaxDialParams;
    //LINEDIALPARAMS  DefaultDialParams;
    printf("dc.dwNumTerminals = %08lx\n", dc->dwNumTerminals);
    printf("dc.dwTerminalCaps: %ls\n", tapistring(bv, dc->dwTerminalCapsOffset, dc->dwTerminalCapsSize).c_str());
    printf("dc.dwTerminalTextEntrySize = %08lx\n", dc->dwTerminalTextEntrySize);
    printf("dc.dwTerminalText: %ls\n", tapistring(bv, dc->dwTerminalTextOffset, dc->dwTerminalTextSize).c_str());
    printf("dc.dwDevSpecific: %ls\n", tapidata(bv, dc->dwDevSpecificOffset, dc->dwDevSpecificSize).c_str());

    printf("dc.dwLineFeatures = %08lx\n", dc->dwLineFeatures);                                 // TAPI v1.4

    printf("dc.dwSettableDevStatus = %08lx\n", dc->dwSettableDevStatus);                            // TAPI v2.0
    printf("dc.dwDeviceClasses: %hs\n", tapistringlist(bv, dc->dwDeviceClassesOffset, dc->dwDeviceClassesSize).c_str());
    //GUID        PermanentLineGuid;                              // TAPI v2.2
    printf("dc.dwAddressTypes = %08lx\n", dc->dwAddressTypes);                                 // TAPI v3.0
    //GUID        ProtocolGuid;                                   // TAPI v3.0
    printf("dc.dwAvailableTracking = %08lx\n", dc->dwAvailableTracking);                            // TAPI v3.0
    return true;
}
bool lineopen(int dwDeviceID)
{
	printf("WM6:Dialing:lineopen() : opening line\n");
	LONG rc;
	rc = lineOpen(hLineApp, dwDeviceID, &g_hLine, dwAPIVersion, dwExtVersion, dwDeviceID+0x1000, 
				LINECALLPRIVILEGE_MONITOR|LINECALLPRIVILEGE_OWNER, voice?LINEMEDIAMODE_INTERACTIVEVOICE:LINEMEDIAMODE_DATAMODEM, NULL);
	if ( rc<0 ) {
		printf("WM6:Dialing:lineopen() : ERROR: lineOpen(%08lx, %08lx, ?, %08lx, %08lx) : %s\n", hLineApp, dwDeviceID, dwAPIVersion, dwExtVersion, ERRSTRING(rc));
		return false;
	 }
	printf("WM6:Dialing:lineopen() : reqid=%08lx lineopen(%d): hline=%08lx\n", rc, dwDeviceID, g_hLine);

//  rc= lineNegotiateExtVersion(hLineApp, dwDeviceID, dwAPIVersion, 0x00000000, 0x80000000, &dwExtVersion);
//  if (rc<0) {
//      printf("lineNegotiateExtVersion(%08lx, %08lx, %08lx) : %s\n", hLineApp, dwDeviceID, dwAPIVersion, ERRSTRING(rc));
//      return false;
//  }

//  printf("%08lx extversion=%08lx\n", rc, dwExtVersion);

    return true;
}
bool lineclose()
{
	printf("WM6:Dialing:lineclose() : closing line\n");
	LONG rc= lineClose(g_hLine);
	if ( rc<0 )
		printf("WM6:Dialing:lineclose() : ERROR: lineClose(%08lx) : %s\n", g_hLine, ERRSTRING(rc));
	g_hLine= NULL;
	printf("WM6:Dialing:lineclose() : reqid=%08lx lineClose - ok\n", rc);
	return true;
}

int getLinesNumber()
 {
	return (int)dwNumLines;
 }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tapiopen(HINSTANCE hInstance)
{
    LINEINITIALIZEEXPARAMS    LineInitializeExParams = {
        sizeof(LINEINITIALIZEEXPARAMS),    //dwTotalSize
        0,                                 //dwNeededSize
        0,                                 //dwUsedSize
        LINEINITIALIZEEXOPTION_USEHIDDENWINDOW,   //dwOptions
        0,                                 //Handles
        0                                  //dwCompletionKey
    };

    printf("WM6:Dialing:tapiopen() : opening tapi\n");
    LONG rc = lineInitializeEx(
            &hLineApp,
            hInstance,
            WM_lineCallbackFunc, //lineCallbackFunc,
            L"SPcore",
            &dwNumLines,
            &dwAPIVersion,
            &LineInitializeExParams
            );
    if ( rc<0 ) {
        printf("WM6:Dialing:tapiopen() : ERROR: lineInitializeEx(%08lx): %x\n", hInstance, ERRSTRING(rc));
        return false;
    }
    //hTapiEvent = LineInitializeExParams.Handles.hEvent;

    printf("WM6:Dialing:tapiopen() : reqid=%08lx lineinit - ok : h=%08lx n=%d  api=%08lx  ev=%08lx\n", rc, hLineApp, dwNumLines, dwAPIVersion, LineInitializeExParams.Handles.hEvent);
    return true;
}

bool tapiclose()
{
    printf("WM6:Dialing:tapiclose() : closing tapi\n");
    LONG rc= lineShutdown(hLineApp);
    if (rc<0)
        printf("WM6:Dialing:tapiclose() : ERROR: lineShutdown(%08lx) : %s\n", hLineApp, ERRSTRING(rc));
    hLineApp= NULL;
    printf("WM6:Dialing:tapiclose() : reqid=%08lx lineShutdown - ok\n", rc);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int monitorCall_stop()
 {
	tapiclose();
	return 0;
 }

int monitorCall_start( MethodTapi_CallChanged_Callback callback )
 {
	if ( !tapiopen(NULL) ) {
		return 1;
	 }

	call_changed_callback = callback;

	DWORD dwDeviceID= 0;
    if ( !devopen(dwDeviceID) ) {
		tapiclose();
		return -1;
	}

	get_devcaps(dwDeviceID);

	if ( !lineopen(dwDeviceID) ) {
		tapiclose();
		return -1;
	}

	return 0;
 }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
MORE INFO.

- lineInitializeEx.
- lineOpen
- Wait for incoming call
- lineAnswer
- lineGetID to retrieve wave in and wave out IDs.
- Possibly lineMonitorDigits if you want to detect caller DTMF.
- waveOut* and waveIn* to play and record to/from the caller.
- When you are done, lineClose and lineShutdown. 

http://nah6.com/~itsme/cvs-xdadevtools/itsutils/leds/
http://nah6.com/~itsme/cvs-xdadevtools/itsutils/leds/tsttapi.cpp

*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
