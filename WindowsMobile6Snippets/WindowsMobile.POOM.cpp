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

#define INITGUID

#include <pimstore.h> // required lib: pimstore.lib




int getPOOM_Version( std::string& poom_version )
 {
	HRESULT hr;

	IPOutlookApp * polApp = NULL;
	BSTR      pwszVersion = NULL;
		
	// Initialize COM.
	CoInitializeEx(NULL, 0);

	// Create an instance of the main application object (IApplication).
	hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_INPROC_SERVER, IID_IPOutlookApp, (LPVOID*)&polApp);
		
	// Log on to Outlook Mobile.
	hr = polApp->Logon(NULL);

	// Get the version, and display it in a message box.
	polApp->get_Version(&pwszVersion);

	poom_version = getStdStringFrom( pwszVersion );

//	MessageBox(NULL, pwszVersion, TEXT("POOM Version"), MB_SETFOREGROUND | MB_OK);

	polApp->SysFreeString(pwszVersion);

	// Log off and release the Application object.
	polApp->Logoff();
	polApp->Release();

	return 0;
 }
