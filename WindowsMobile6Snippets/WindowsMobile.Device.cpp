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

#include <map>



HRESULT SHGetPhoneNumber( LPTSTR szNumber, UINT cchNumber, UINT nLineNumber );

#include <pm.h>
int GetPowerState( std::string& power_state );


#include <tapi.h>
#include <tsp.h>
#define TAPI_API_LOW_VERSION    0x00020000
#define TAPI_API_HIGH_VERSION   0x00020000

#include <GetDeviceUniqueId.h>
HRESULT GetDeviceIDs( BYTE* g_bDeviceID1, BYTE* g_bDeviceID2, DWORD& g_cbDeviceID1, DWORD& g_cbDeviceID2 );
 HRESULT HexString (const BYTE value, LPTSTR pszBuffer, const DWORD cbBufferSize, DWORD *pcbAdded);
 HRESULT DeviceID2String (const BYTE * const bDeviceID, const DWORD cbDeviceID, const LPTSTR pszIDAsString, const DWORD cbIDAsString);

#define EXT_API_LOW_VERSION     0x00010000
#define EXT_API_HIGH_VERSION    0x00010000
#include <extapi.h>

int GetExTAPIInfo( std::map<std::string, std::string>& fields );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetPhoneNumber( std::string& phone_number, unsigned int nLineNumber )
 {
	#define MAX_LOADSTRING 	25
	TCHAR szNumber[MAX_LOADSTRING];
	memset(szNumber, 0, MAX_LOADSTRING);
	HRESULT ret = SHGetPhoneNumber(szNumber, MAX_LOADSTRING, nLineNumber);
	if ( ret == E_FAIL)
		return -1001;
	else if ( ret != S_OK)
		return -101;
	
#ifndef UNICODE
	phone_number.assign( szNumber );
#else
	ConvertToString(szNumber, phone_number);
#endif
		
	return 0;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetDeviceIDs( std::string& device_id_1, std::string& device_id_2 )
 {
	// Buffers to hold the two device IDs we are going to generate
	BYTE g_bDeviceID1[GETDEVICEUNIQUEID_V1_OUTPUT];
	BYTE g_bDeviceID2[GETDEVICEUNIQUEID_V1_OUTPUT];
	memset(g_bDeviceID1, 0, GETDEVICEUNIQUEID_V1_OUTPUT);
	memset(g_bDeviceID2, 0, GETDEVICEUNIQUEID_V1_OUTPUT);
	// Lengths of the returned device IDs
	DWORD g_cbDeviceID1 = GETDEVICEUNIQUEID_V1_OUTPUT;
	DWORD g_cbDeviceID2 = GETDEVICEUNIQUEID_V1_OUTPUT;

	HRESULT ret = GetDeviceIDs( g_bDeviceID1, g_bDeviceID2, g_cbDeviceID1, g_cbDeviceID2 );
	if ( ret == E_FAIL)
		return -1001;
	else if ( ret != S_OK)
		return -101;

    TCHAR szDeviceID[(GETDEVICEUNIQUEID_V1_OUTPUT * 2) + 1];

    if (SUCCEEDED (DeviceID2String (g_bDeviceID1, g_cbDeviceID1, szDeviceID, ARRAYSIZE (szDeviceID)))) {
#ifndef UNICODE
	device_id_1.assign( szNumber );
#else
	ConvertToString(szDeviceID, device_id_1);
#endif
	 }
    if (SUCCEEDED (DeviceID2String (g_bDeviceID2, g_cbDeviceID2, szDeviceID, ARRAYSIZE (szDeviceID)))) {
#ifndef UNICODE
	device_id_2.assign( szNumber );
#else
	ConvertToString(szDeviceID, device_id_2);
#endif
	 }

	return 0;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetPowerState( std::string& power_state ) 
{
    TCHAR szState[MAX_PATH];
    DWORD dwState;

    if (ERROR_SUCCESS != GetSystemPowerState(szState, MAX_PATH, &dwState)) 
		return -1001;
#ifndef UNICODE
	power_state.assign( szState );
#else
	ConvertToString(szState, power_state);
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define CAPS_BUFFER_SIZE    512

/////////////////////////////////////////////////////////////////////////////
// Function: SHGetPhoneNumber
// szNumber - Out Buffer for the phone number
// cchNumber - size of sznumber in characters
// nLineNumber - In which phone line (1 or 2) to get the number for
/////////////////////////////////////////////////////////////////////////////
HRESULT SHGetPhoneNumber(LPTSTR szNumber, UINT cchNumber, UINT nLineNumber)
{
    HRESULT  hr = E_FAIL;
    LRESULT  lResult = 0;
    HLINEAPP hLineApp;
    DWORD    dwNumDevs;  //number of line devices
    DWORD    dwAPIVersion = TAPI_API_HIGH_VERSION;
    LINEINITIALIZEEXPARAMS liep;

    DWORD dwTAPILineDeviceID;
    const DWORD dwAddressID = nLineNumber - 1;

    liep.dwTotalSize = sizeof(liep);
    liep.dwOptions   = LINEINITIALIZEEXOPTION_USEEVENT;

    //initialize line before accessing
    if (SUCCEEDED(lineInitializeEx(&hLineApp, 0, 0, TEXT("ExTapi_Lib"), &dwNumDevs, &dwAPIVersion, &liep)))
    {

        BYTE* pCapBuf = NULL;
        DWORD dwCapBufSize = CAPS_BUFFER_SIZE;
        LINEEXTENSIONID  LineExtensionID;
        LINEDEVCAPS*     pLineDevCaps = NULL;
        LINEADDRESSCAPS* placAddressCaps = NULL;

        pCapBuf = new BYTE[dwCapBufSize];
	   // EXIT_ON_NULL(pCapBuf);
		if ( pCapBuf == NULL ) {
			lineShutdown(hLineApp);
			return E_OUTOFMEMORY;
		}

        pLineDevCaps = (LINEDEVCAPS*)pCapBuf;
        pLineDevCaps->dwTotalSize = dwCapBufSize;

        // Get TSP Line Device ID
        dwTAPILineDeviceID = 0xffffffff;
        for (DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumDevs ; dwCurrentDevID++)
        {
            //ensure TAPI, service provider, and application are all using the same versions
            if (0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, TAPI_API_LOW_VERSION, TAPI_API_HIGH_VERSION,
                &dwAPIVersion, &LineExtensionID))
            {
                lResult = lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, pLineDevCaps);

                //increase buffer size if too small to hold the device capabilities
                if (dwCapBufSize < pLineDevCaps->dwNeededSize)
                {
                    delete[] pCapBuf;
                    dwCapBufSize = pLineDevCaps->dwNeededSize;
                    pCapBuf = new BYTE[dwCapBufSize];
                   // EXIT_ON_NULL(pCapBuf);
					if ( pCapBuf == NULL ) {
						lineShutdown(hLineApp);
						return E_OUTOFMEMORY;
					}

                    pLineDevCaps = (LINEDEVCAPS*)pCapBuf;
                    pLineDevCaps->dwTotalSize = dwCapBufSize;

                    lResult = lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, pLineDevCaps);
                }
                //lResult of 0 means the device capabilities were successfully returned
                if ((0 == lResult) &&
                    (0 == _tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), CELLTSP_LINENAME_STRING)))
                {
                    dwTAPILineDeviceID = dwCurrentDevID;
                    break;
                }
            } 
        } 

        placAddressCaps = (LINEADDRESSCAPS*)pCapBuf;
        placAddressCaps->dwTotalSize = dwCapBufSize;

        lResult = lineGetAddressCaps(hLineApp, dwTAPILineDeviceID, dwAddressID, dwAPIVersion, 0, placAddressCaps);

        //increase buffer size if too small to hold the address capabilities
        if (dwCapBufSize < placAddressCaps->dwNeededSize)
        {
            delete[] pCapBuf;
            dwCapBufSize = placAddressCaps->dwNeededSize;
            pCapBuf = new BYTE[dwCapBufSize];
		   // EXIT_ON_NULL(pCapBuf);
			if ( pCapBuf == NULL ) {
				lineShutdown(hLineApp);
				return E_OUTOFMEMORY;
			}
            placAddressCaps = (LINEADDRESSCAPS*)pCapBuf;
            placAddressCaps->dwTotalSize = dwCapBufSize;

            lResult = lineGetAddressCaps(hLineApp, dwTAPILineDeviceID, dwAddressID, dwAPIVersion, 0, placAddressCaps);
        }
        //lResult of 0 means the address capabilities were successfully returned
        if (0 == lResult)
        {
            if (szNumber)
            {
                szNumber[0] = TEXT('\0');

               // EXIT_ON_FALSE(0 != placAddressCaps->dwAddressSize);
				if ( 0 == placAddressCaps->dwAddressSize ) {
					lineShutdown(hLineApp);
					return E_FAIL;
				}
                // A non-zero dwAddressSize means a phone number was found
                ASSERT(0 != placAddressCaps->dwAddressOffset);
                PWCHAR tsAddress = (WCHAR*)(((BYTE*)placAddressCaps)+placAddressCaps->dwAddressOffset);

                StringCchCopy(szNumber, cchNumber, tsAddress);
            }

            hr = S_OK;
        } 

        delete[] pCapBuf;
    } // End if ()

    lineShutdown(hLineApp);

    return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 *    Call the GetDeviceUniqueID function twice using different
 *    Application Specific Data to demonstrate that different
 *    values are returned. This allows different applications to
 *    have a device ID that can be used to identify a device. If 
 *    two (or more) applications need to use the same device ID then
 *    they need to use the same Application Specific Data when calling
 *    GetDeviceUniqueID.
 */
HRESULT GetDeviceIDs( BYTE* g_bDeviceID1, BYTE* g_bDeviceID2, DWORD& g_cbDeviceID1, DWORD& g_cbDeviceID2)
{
    HRESULT            hr;

    // Get the first device id

    // Application specific data
    // {8D552BD1-E232-4107-B72D-38B6A4726439}
    const GUID     bApplicationData1  = { 0x8d552bd1, 0xe232, 0x4107, { 0xb7, 0x2d, 0x38, 0xb6, 0xa4, 0x72, 0x64, 0x39 } };
    const DWORD    cbApplicationData1 = sizeof (bApplicationData1);

    g_cbDeviceID1 = GETDEVICEUNIQUEID_V1_OUTPUT;
    hr = GetDeviceUniqueID (reinterpret_cast<LPBYTE>(const_cast<LPGUID>(&bApplicationData1)), 
                             cbApplicationData1, 
                             GETDEVICEUNIQUEID_V1, 
                             g_bDeviceID1, 
                             &g_cbDeviceID1);
    
    if (SUCCEEDED (hr))
    {
        // Get a second ID to verify that they are different for
        // different Application data

        // Application specific data
        // {C5BEC46D-2A43-4200-BBB7-5FF14097954D}
        const GUID     bApplicationData2  = { 0xc5bec46d, 0x2a43, 0x4200, { 0xbb, 0xb7, 0x5f, 0xf1, 0x40, 0x97, 0x95, 0x4d } };
        const DWORD    cbApplicationData2 = sizeof (bApplicationData2);

        g_cbDeviceID2 = GETDEVICEUNIQUEID_V1_OUTPUT;
        hr = GetDeviceUniqueID (reinterpret_cast<LPBYTE>(const_cast<LPGUID>(&bApplicationData2)), 
                                 cbApplicationData2, 
                                 GETDEVICEUNIQUEID_V1, 
                                 g_bDeviceID2, 
                                 &g_cbDeviceID2);
    }

    return hr;
}


HRESULT HexString (const BYTE value, LPTSTR pszBuffer, const DWORD cbBufferSize, DWORD *pcbAdded)
{
    static const TCHAR HexLookup[16] =  {_T('0'), _T('1'), _T('2'), _T('3'),
                                         _T('4'), _T('5'), _T('6'), _T('7'),
                                         _T('8'), _T('9'), _T('A'), _T('B'),
                                         _T('C'), _T('D'), _T('E'), _T('F')
                                        };
    HRESULT hr;
    if (cbBufferSize < 3) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        *pszBuffer++ = HexLookup[(value >> 4) & 0x0F];
        *pszBuffer++ = HexLookup[value & 0x0F];
        *pszBuffer = _T('\0');
        *pcbAdded = 2;
        hr = S_OK;
    }
    return hr;
}

// Generate printable version of device ID
HRESULT DeviceID2String (const BYTE * const bDeviceID, const DWORD cbDeviceID, const LPTSTR pszIDAsString, const DWORD cbIDAsString)
{
    HRESULT hr;
    LPTSTR    pszOutput            = pszIDAsString;
    DWORD    cbOutputRemaining    = cbIDAsString;
    DWORD    i;
    DWORD    cbAdded;

    for (i = 0; i < cbDeviceID; ++i) {
        hr = HexString (bDeviceID[i], pszOutput, cbOutputRemaining, &cbAdded);
        if (FAILED (hr)) {
            break;
        }
        cbOutputRemaining -= cbAdded;
        pszOutput += cbAdded;
    }
    return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ***************************************************************************
// Function Name: GetTSPLineDeviceID
//
// Purpose: To get a TSP Line Device ID
//
// Arguments:
//  hLineApp = the HLINEAPP returned by lineInitializeEx
//  dwNumberDevices = also returned by lineInitializeEx
//  dwAPIVersionLow/High = min version of TAPI that we need
//  psTSPLineName = "Cellular Line"
//
// Return Values: Current Device ID
//
// Description:
//  This function returns the device ID of a named TAPI TSP.  The Device ID is 
//  used in the call to lineOpen

DWORD GetTSPLineDeviceID(const HLINEAPP hLineApp, 
                         const DWORD dwNumberDevices, 
                         const DWORD dwAPIVersionLow, 
                         const DWORD dwAPIVersionHigh, 
                         const TCHAR* const psTSPLineName)
{
    DWORD dwReturn = 0xffffffff;
    for(DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumberDevices ; dwCurrentDevID++)
    {
        DWORD dwAPIVersion;
        LINEEXTENSIONID LineExtensionID;
        if(0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, 
                                        dwAPIVersionLow, dwAPIVersionHigh, 
                                        &dwAPIVersion, &LineExtensionID)) 
        {
            LINEDEVCAPS LineDevCaps;
            LineDevCaps.dwTotalSize = sizeof(LineDevCaps);
            if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                                   dwAPIVersion, 0, &LineDevCaps)) 
            {
                BYTE* pLineDevCapsBytes = new BYTE[LineDevCaps.dwNeededSize];
                if(0 != pLineDevCapsBytes) 
                {
                    LINEDEVCAPS* pLineDevCaps = (LINEDEVCAPS*)pLineDevCapsBytes;
                    pLineDevCaps->dwTotalSize = LineDevCaps.dwNeededSize;
                    if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                                           dwAPIVersion, 0, pLineDevCaps)) 
                    {
                        if(0 == _tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), 
                                        psTSPLineName)) 
                        {
                            dwReturn = dwCurrentDevID;
                        }
                    }
                    delete[]  pLineDevCapsBytes;
                }
            }
        }
    }
    return dwReturn;
}

#define HUGE_BUFFER             4096

// ***************************************************************************
// Function Name: GetExTAPIInfo
//
// Purpose: To get and display general ExTAPI information.
//
// Arguments: None
//
// Return Values: 
//  TRUE if all ExTAPI operations completed successfully, FALSE otherwise
//
// Description:
//  This function initializes ExTAPI and then calls lineGetGeneralInfo.
//  It then takes the info that lGGI returned and displays it in a MessageBox

int GetExTAPIInfo( std::map<std::string, std::string>& fields )
{
    DWORD dwNumDevs;
    DWORD dwAPIVersion = TAPI_API_HIGH_VERSION;
    LINEINITIALIZEEXPARAMS liep;
    HLINEAPP hLineApp = 0;
    HLINE hLine = 0;
    DWORD dwExtVersion;
    int bRetVal = -1001;
    LPBYTE pLineGeneralInfoBytes = NULL;
    DWORD dwTAPILineDeviceID;
    const DWORD dwMediaMode = LINEMEDIAMODE_DATAMODEM | LINEMEDIAMODE_INTERACTIVEVOICE;
    LINEGENERALINFO lviGeneralInfo;
    LPLINEGENERALINFO plviGeneralInfo;
	TCHAR* gszFriendlyAppName = TEXT("wosh::WindowsMobile");

    // set the line init params
    liep.dwTotalSize = sizeof(liep);
    liep.dwOptions = LINEINITIALIZEEXOPTION_USEEVENT;
    
    if (lineInitializeEx(&hLineApp, 0, 0, gszFriendlyAppName, 
                         &dwNumDevs, &dwAPIVersion, &liep)) {
        goto cleanup;
    }

    // get the device ID
    dwTAPILineDeviceID = GetTSPLineDeviceID(hLineApp, dwNumDevs, 
                                                  TAPI_API_LOW_VERSION, 
                                                  TAPI_API_HIGH_VERSION, 
                                                  CELLTSP_LINENAME_STRING);

    // error getting the line device ID?
    if (0xffffffff == dwTAPILineDeviceID) {
        goto cleanup;
    }

    // now try and open the line
    if(lineOpen(hLineApp, dwTAPILineDeviceID, 
                &hLine, dwAPIVersion, 0, 0, 
                LINECALLPRIVILEGE_OWNER, dwMediaMode, 0)) {
        goto cleanup;
    }

    // set up ExTAPI
    if (lineNegotiateExtVersion(hLineApp, dwTAPILineDeviceID, 
                                dwAPIVersion, EXT_API_LOW_VERSION, 
                                EXT_API_HIGH_VERSION, &dwExtVersion)) {
        goto cleanup;
    }

    // try to get the general info
    lviGeneralInfo.dwTotalSize = sizeof(lviGeneralInfo);

    // step 1: find out how much space we need
    if (lineGetGeneralInfo(hLine, &lviGeneralInfo)) 
    {
        goto cleanup;
    }

    // step 2: malloc space for all the info we need
    pLineGeneralInfoBytes = new BYTE[lviGeneralInfo.dwNeededSize];
    plviGeneralInfo = (LPLINEGENERALINFO)pLineGeneralInfoBytes;

    // step 3: call lGGI again with the appropriately sized buffer
    if(NULL != pLineGeneralInfoBytes) 
    {
        plviGeneralInfo->dwTotalSize = lviGeneralInfo.dwNeededSize;
        if (lineGetGeneralInfo(hLine, plviGeneralInfo)) 
        {
            goto cleanup;
        }
    } else 
    {
        goto cleanup;
    }

    // step 4: cast all the arguments to strings
    if( 0 < plviGeneralInfo->dwManufacturerSize )
		fields["Manufacturer"] = getStdStringFrom( (WCHAR*)(((BYTE*)plviGeneralInfo)+plviGeneralInfo->dwManufacturerOffset) );
    else
		fields["Manufacturer"] = "";

    if( 0 < plviGeneralInfo->dwModelSize )
		fields["Model"] = getStdStringFrom( (WCHAR*)(((BYTE*)plviGeneralInfo)+plviGeneralInfo->dwModelOffset) );
    else
		fields["Model"] = "";

    if( 0 < plviGeneralInfo->dwRevisionSize )
		fields["Revision"] = getStdStringFrom( (WCHAR*)(((BYTE*)plviGeneralInfo)+plviGeneralInfo->dwRevisionOffset) );
    else
		fields["Revision"] = "";

    if( 0 < plviGeneralInfo->dwSerialNumberSize )
		fields["SerialNumber"] = getStdStringFrom( (WCHAR*)(((BYTE*)plviGeneralInfo)+plviGeneralInfo->dwSerialNumberOffset) );
    else
		fields["SerialNumber"] = "";

    if( 0 < plviGeneralInfo->dwSubscriberNumberSize )
		fields["SubscriberNumber"] = getStdStringFrom( (WCHAR*)(((BYTE*)plviGeneralInfo)+plviGeneralInfo->dwSubscriberNumberOffset) );
    else
		fields["SubscriberNumber"] = "";


    bRetVal = 0;

cleanup:
    if (pLineGeneralInfoBytes) delete [] pLineGeneralInfoBytes;
    if (hLine) lineClose(hLine);
    if (hLineApp) lineShutdown(hLineApp);

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// Function Name: SetBacklightRequirement
//
// Purpose: Sets or releases the device power requirement to keep the 
//          backlight at D0
//
// Arguments:
//  IN BOOL fBacklightOn - TRUE to leave the backlight on
//
void SetBacklightRequirement(BOOL fBacklightOn)
{
    // the name of the backlight device
    TCHAR tszBacklightName[] = TEXT("BKL1:"); 

    static HANDLE s_hBacklightReq = NULL;
    
    if (fBacklightOn) 
    {
        if (NULL == s_hBacklightReq) 
        {
            // Set the requirement that the backlight device must remain
            // in device state D0 (full power)
            s_hBacklightReq = SetPowerRequirement(tszBacklightName, D0, 
                                                  POWER_NAME, NULL, 0);
            if (!s_hBacklightReq)
            {
                RETAILMSG(1, (L"SetPowerRequirement failed: %X\n", 
                              GetLastError()));
            }
        }
    } 
    else 
    {
        if (s_hBacklightReq) 
        {
            if (ERROR_SUCCESS != ReleasePowerRequirement(s_hBacklightReq))
            {
                RETAILMSG(1, (L"ReleasePowerRequirement failed: %X\n",
                              GetLastError()));
            }
            s_hBacklightReq = NULL;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
SystemParametersInfo

TCHAR szPlatform[20];
BOOL bRet = SystemParametersInfo(SPI_GETPLATFORMTYPE,sizeof(szPlatform),szPlatform,0);
if (bRet)
    MessageBox(NULL,szPlatform,L"Platform Type",MB_OK|MB_SETFOREGROUND);
else
    MessageBox(NULL,L"Failed to get platform type",L"Platform Type",MB_OK|MB_SETFOREGROUND);
	


SPI_GETBATTERYIDLETIMEOUT
Retrieves the amount of time that Windows Embedded CE will stay on with battery power before it suspends due to user inaction
The pvParam parameter points to a DWORD that returns the time in seconds. This flag is ignored if pvParam is zero.
If you disable GWES suspend management then the function will return 0.

SPI_GETPLATFORMNAME
SPI_GETPLATFORMTYPE
SPI_GETPLATFORMVERSION

SPI_GETUUID


BOOL GetSystemPowerStatusEx(
  PSYSTEM_POWER_STATUS_EX pstatus,
  BOOL fUpdate
);
SYSTEM_POWER_STATUS_EX2 status;
memset(&status,0,sizeof(status));
DWORD dwLen = sizeof(status);
DWORD dwRet = GetSystemPowerStatusEx2(&status,dwLen,TRUE); 
s

void UpdatePowerState( void ) 
{
    TCHAR szState[MAX_PATH];
    DWORD dwState;

    if (g_hSystemState) 
    {
        if (ERROR_SUCCESS == GetSystemPowerState(szState, MAX_PATH, &dwState)) 
        {
            SetWindowText(g_hSystemState, szState);
        }
    }

}

*/
















