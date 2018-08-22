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

// ***************************************************************************
// MAIN.CPP
//
// Simple SMS Message Sender
//
//

#include "WindowsMobile.Common.h"


#include <sms.h>	// required lib: sms.lib
int SendSMS(BOOL bSendConfirmation, BOOL bUseDefaultSMSC, LPCTSTR lpszSMSC, LPCTSTR lpszRecipient, LPCTSTR lpszMessage);


typedef void (*MethodTapi_SmsNotify_Callback)(const struct sms_info& sms );
struct sms_info {
	std::string alias;
	std::string phone_number;
	std::string subject;
	std::string body;
	long timestamp;
	int priority;
};





// must be like "39347123456"
int SendSMS( const std::string& recipent, const std::string& message, bool default_service = true, const std::string& server = "", bool send_confirmation = false )
 {
#ifndef UNICODE
	LPTSTR gpszRecipient = (TCHAR*)recipent.c_str();
	LPTSTR gpszMessage = (TCHAR*)message.c_str();
	LPTSTR gpszServer = (LPTSTR)server.c_str();
#else
	std::wstring wstr_rec = std::wstring(recipent.begin(), recipent.end());
	LPTSTR gpszRecipient = (LPTSTR)wstr_rec.c_str();

	std::wstring wstr_msg = std::wstring(message.begin(), message.end());
	LPTSTR gpszMessage = (LPTSTR)wstr_msg.c_str();

	std::wstring wstr_srv = std::wstring(server.begin(), server.end());
	LPTSTR gpszServer = (LPTSTR)wstr_srv.c_str();
#endif

	int ret = SendSMS( send_confirmation, default_service, gpszServer, gpszRecipient, gpszMessage);

	return ret;
 }










/*
int GetSmsPhoneNumber( string& phone_number )
(
HRESULT SmsGetPhoneNumber ( const SMS_ADDRESS* psmsaAddress );

}
*/


// ***************************************************************************
// Function Name: SendSMS
// 
// Purpose: Send an SMS Message
//
// Arguments: none
//
// Return Values: none
//
// Description:
//	Called after everything has been set up, this function merely opens an
//	SMS_HANDLE and tries to send the SMS Message.
 
int SendSMS(BOOL bSendConfirmation, BOOL bUseDefaultSMSC, LPCTSTR lpszSMSC, LPCTSTR lpszRecipient, LPCTSTR lpszMessage)
{
	SMS_HANDLE smshHandle;
	SMS_ADDRESS smsaSource;
	SMS_ADDRESS smsaDestination;
	TEXT_PROVIDER_SPECIFIC_DATA tpsd;
	SMS_MESSAGE_ID smsmidMessageID;

	// try to open an SMS Handle
	if(FAILED(SmsOpen(SMS_MSGTYPE_TEXT, SMS_MODE_SEND, &smshHandle, NULL)))
	{
//		MessageBox(NULL,
//					(LPCTSTR)LoadString(ghInstance, IDS_ERROR_SMSOPEN, 0, 0), 
//					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_ERROR, 0, 0),
//					MB_OK | MB_ICONERROR);
		return -1001;
	}

	// Create the source address
	if(!bUseDefaultSMSC)
	{
		smsaSource.smsatAddressType = SMSAT_INTERNATIONAL;
		_tcsncpy(smsaSource.ptsAddress, lpszSMSC, SMS_MAX_ADDRESS_LENGTH);
	}

	// Create the destination address
	smsaDestination.smsatAddressType = SMSAT_INTERNATIONAL;
	_tcsncpy(smsaDestination.ptsAddress, lpszRecipient, SMS_MAX_ADDRESS_LENGTH);

	// Set up provider specific data
    memset(&tpsd, 0, sizeof(tpsd));
	tpsd.dwMessageOptions = bSendConfirmation ? PS_MESSAGE_OPTION_STATUSREPORT : PS_MESSAGE_OPTION_NONE;
	tpsd.psMessageClass = PS_MESSAGE_CLASS1;
	tpsd.psReplaceOption = PSRO_NONE;
	tpsd.dwHeaderDataSize = 0;

	int ret = -1;
	// Send the message, indicating success or failure
	if(SUCCEEDED(SmsSendMessage(smshHandle, ((bUseDefaultSMSC) ? NULL : &smsaSource), 
								 &smsaDestination, NULL, (PBYTE) lpszMessage, 
								 _tcslen(lpszMessage) * sizeof(TCHAR), (PBYTE) &tpsd, 
								 sizeof(TEXT_PROVIDER_SPECIFIC_DATA), SMSDE_OPTIMAL, 
								 SMS_OPTION_DELIVERY_NONE, &smsmidMessageID)))
	{
//		MessageBox(NULL,
//					(LPCTSTR)LoadString(ghInstance, IDS_SMSSENT, 0, 0), 
//					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_SUCCESS, 0, 0),
//					MB_OK);
		ret = 0;
	}
	else
	{
//		MessageBox(NULL,
//					(LPCTSTR)LoadString(ghInstance, IDS_ERROR_SMSSEND, 0, 0), 
//					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_ERROR, 0, 0),
//					MB_OK | MB_ICONERROR);
		ret = -100;
	}

	// clean up
	VERIFY(SUCCEEDED(SmsClose(smshHandle)));
	
	return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "cemapi.h" // lib: cemapi.lib

IMAPISession *Session = NULL;
ULONG Connection = 12345;
int iEventSms = 0;
MethodTapi_SmsNotify_Callback notifyCB = NULL;

HRESULT GetPropsExample(IMessage * pMsg) 
{
    HRESULT           hr = E_FAIL;
    SPropValue * rgprops = NULL;
    ULONG       rgTags[] = {3, PR_SENDER_EMAIL_ADDRESS, PR_SUBJECT, PR_IMPORTANCE};
    ULONG         cCount = 0;

    // Get the message's properties.
    hr = pMsg->GetProps((LPSPropTagArray) rgTags, MAPI_UNICODE, &cCount, &rgprops);


    // Access the properties.
    if (SUCCEEDED(hr)) 
    {        
        // Check that the ulPropTag member of each property value is of the property type requested, and that it does not have a value of PT_ERROR.
        if (rgprops[0].ulPropTag == PR_SENDER_EMAIL_ADDRESS) 
            DEBUGMSG(TRUE, (L"From: %s \r\n", rgprops[0].Value.lpszW));

        if (rgprops[1].ulPropTag == PR_SUBJECT) 
            DEBUGMSG(TRUE, (L"Subject: %s \r\n", rgprops[1].Value.lpszW));

        if (rgprops[2].ulPropTag == PR_IMPORTANCE) 
            DEBUGMSG(TRUE, (L"Importance: %d \r\n", rgprops[2].Value.ul));

		sms_info sms;

//		std::size_t found = from.find("\"");
//		size_t found2 = from.find(">");
//		if ( found != std::string::npos )
//			sms.alias = from.substr(found+1, found2);

		std::string from = getStdStringFrom( rgprops[0].Value.lpszW );
		std::size_t found = from.find_last_of(" <");
		if ( found != std::string::npos )
			sms.phone_number = from.substr(found+1);
		found = sms.phone_number.find_last_of(">");
		if ( found != std::string::npos )
			sms.phone_number = sms.phone_number.substr(0, found);

		sms.body = getStdStringFrom( rgprops[1].Value.lpszW );
//		sms.priority = (int)rgprops[2].Value.ul;

        // Free the returned SPropValue structure.
        MAPIFreeBuffer(rgprops);

		if ( notifyCB != NULL )
			(notifyCB)(sms);
    }

	//hr = pMsg->SetReadFlag(CLEAR_READ_FLAG )

    return hr;
}

struct SMS_INTERCEPTOR : public IMAPIAdviseSink {
	public:
		SMS_INTERCEPTOR()
		 {
		 }

		virtual ULONG OnNotify(ULONG NNotif,LPNOTIFICATION Notifications)
		 {
//MessageBox(0,L"SMS received!",0,0);
			printf("EVENT : OnNotify !" );
			for(UINT x=0;x<NNotif;x++) {
				switch(Notifications[x].ulEventType) {
					case fnevNewMail:
						printf("SMS - incomning\n");
						break;
					case fnevObjectCreated:
						printf("SMS - obj. created\n");
						break;
					case fnevObjectDeleted:
						printf("SMS - obj. deleted\n");
						break;
					case fnevObjectModified: {
						printf("SMS - obj. modified\n");
						
						if ( ++iEventSms < 4 )
							break;
						
						printf("SMS - READING ...");
						if ( Notifications[x].info.obj.lpEntryID == NULL )
							printf("SMS - obj. modified ERROR : objnot.lpEntryID == NULL\n");

						ULONG ulObjType = 0; 
						IMessage* pMessage = NULL;
						
						HRESULT hr = Session->OpenEntry( Notifications[x].info.obj.cbEntryID, Notifications[x].info.obj.lpEntryID, NULL, 0, &ulObjType, (LPUNKNOWN*)&pMessage);
					//	HRESULT hr = Session->OpenEntry( objnot.cbEntryID, objnot.lpEntryID, NULL, 0, &ulObjType, (LPUNKNOWN*)pMessage); 
						if (hr != S_OK) {
							if (hr == MAPI_E_NO_ACCESS )
								printf("ERROR: OpenEntry failed: MAPI_E_NO_ACCESS \n");
							else if (hr == MAPI_E_NOT_FOUND )
								printf("ERROR: OpenEntry failed: MAPI_E_NOT_FOUND \n");
							else if (hr == MAPI_E_UNKNOWN_ENTRYID )
								printf("ERROR: OpenEntry failed: MAPI_E_UNKNOWN_ENTRYID \n");
							else
								printf("ERROR: OpenEntry failed.\n");
							return -1;
						 }
						else if ( pMessage == NULL ) {
							printf("SUCCEDED: OpenEntry but NULL MESSAGE!!\n");
						 }
						else {
							printf("SUCCEDED: OpenEntry .\n");
							if ( ulObjType == MAPI_MESSAGE )
								printf("SUCCEDED: OpenEntry returned MAPI_MESSAGE.\n");

							hr = GetPropsExample(pMessage);
							if (hr != S_OK) {
								printf("ERROR: GetPropsExample failed.\n");
								return -1;
							 }
							pMessage->Release();
						}
						break;
						}
					case fnevObjectMoved:
						printf("SMS - obj. moved\n");
						break;
					case fnevObjectCopied:
						printf("SMS - obj. copied\n");
						break;
					case fnevSearchComplete:
						printf("SMS -search complete\n");
						break;
					case fnevTableModified:
						printf("SMS - table modified\n");
						break;
					case fnevStatusObjectModified:
						printf("SMS - stato ogg. modificato\n");
						break;

					default:
						printf("SMS - Unknown notify\n");
						break;
				 }
			 }
			return NO_ERROR;
		 }

		virtual HRESULT QueryInterface(const IID &id,LPVOID *ptr)
		 {
			UNREFERENCED_PARAMETER(id);
			UNREFERENCED_PARAMETER(ptr);
			return S_OK;
		 }
		virtual ULONG AddRef(void)		{ return 0; }
		virtual ULONG Release(void)		{ return 0; }

};

SMS_INTERCEPTOR *Sms = NULL;

int monitorSms_start( MethodTapi_SmsNotify_Callback callback )
 {
	notifyCB = callback;

	HRESULT hr;
	hr = MAPIInitialize(NULL);

	if (hr != S_OK) { // MAPIInitialize failed.
		printf("ERROR: MAPIInitialize failed.\n");
		return -1;
	 }
		// MAPILogonEx( 0, L"CreaRe Daemon", NULL, MAPI_EXTENDED | MAPI_NEW_SESSION, &Session );
	hr = MAPILogonEx(0, NULL, NULL, 0, (LPMAPISESSION *)&Session);
	if (hr != S_OK) {
		// MAPILogonEx failed.
		printf("ERROR: MAPILogonEx failed.\n");
		MAPIUninitialize();
		return -2;
	 }

//HERE
	Sms = new SMS_INTERCEPTOR;
	hr = Session->Advise(0,NULL, 
							fnevNewMail | fnevObjectCreated | fnevObjectDeleted | fnevObjectModified | fnevObjectMoved |
							fnevObjectCopied | fnevSearchComplete | fnevTableModified | fnevStatusObjectModified,
							Sms,&Connection);

/*
	hr = Session->OpenMsgStore(0, 3,&entryID,NULL,MAPI_BEST_ACCESS | MDB_NO_DIALOG, &Store);
	if (hr != S_OK) {
		Session->Logoff(0,0,0);
		Session->Release();
		MAPIUninitialize();
		return -3;
	 }
*/
	if (hr  != S_OK) {
		Session->Logoff(0,0,0);
		
		Session->Release();
		MAPIUninitialize();
		return -4;
	 }

	printf("SUCCEDED: monitorSms_START .\n");
	return 0;
 }

int monitorSms_stop()
 {
	HRESULT hr;
	hr = Session->Logoff(0, 0, 0);
	if (hr != S_OK) {
		// Logoff failed.
		printf("ERROR: Logoff failed.\n");
		Session->Release();
		MAPIUninitialize();
		return -5;
	 }

	Session->Unadvise(Connection);
	Session->Release();
	MAPIUninitialize();

	printf("SUCCEDED: monitorSms_STOP.\n");
	return 0;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int waitIncomingSms( long timeout ) // non va, perche qualcuno ha gia aperto l'handle
 {
    SMS_ADDRESS smsaDestination;
    TEXT_PROVIDER_SPECIFIC_DATA tpsd;
    SMS_HANDLE smshHandle;
	HRESULT hr;
	int ret = 0;
	HANDLE hRead = CreateEvent (NULL, FALSE, FALSE, NULL);

	hr = SmsOpen(SMS_MSGTYPE_TEXT, SMS_MODE_RECEIVE, &smshHandle, &hRead);
	// try to open an SMS Handle
	if( FAILED(hr) ) {
		//printf ("SmsOpen fail %x %d ERROR  \r\n", hr, GetLastError());
		return -1001;
	}

	// Wait for message to come in.
	int rc = WaitForSingleObject (hRead, timeout);//INFINITE
	if (rc != WAIT_OBJECT_0) {
		SmsClose (smshHandle);
		//printf ("Sms SmsClose() -NO Msg \r\n");
		return 0;
	 }

	memset (&smsaDestination, 0, sizeof (smsaDestination));
	DWORD dwSize, dwRead = 0;

	hr = SmsGetMessageSize (smshHandle, &dwSize);

	if (hr != ERROR_SUCCESS) {
		dwSize = 1024;
		SmsClose (smshHandle);
		//printf ("Sms SmsClose() -ERROR \r\n");
		return -1002;
	 }   

	char *pMessage = (char *)malloc (dwSize+1);
	memset (&tpsd, 0, sizeof (tpsd));
	hr = SmsReadMessage (smshHandle, NULL, &smsaDestination, NULL,
							(PBYTE)pMessage, dwSize,
							(PBYTE)&tpsd, sizeof(TEXT_PROVIDER_SPECIFIC_DATA),
							&dwRead );

	if (hr == ERROR_SUCCESS) {
		

MessageBox(NULL, TEXT("Message"), TEXT("Message"), MB_OK);

//		CString outputStr;
//		outputStr.Format(L"dst=%s,msg=%s",smsaDestination.ptsAddress,pMessage);
//		AfxMessageBox(outputStr);
		ret = 0;
	 }
	else {
		free (pMessage);
		ret = -101;
	 }

	SmsClose (smshHandle);

	return ret;
}


 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* OK but unused

#include <atlbase.h>
#include <cemapi.h>
#include <mapiutil.h>
#include <mapidefs.h>
 
 #define CHR(x) if (FAILED(x)) { hr = x; goto Error; }

HRESULT DisplayMessageStores()
{
    HRESULT hr;
    CComPtr<IMAPITable> ptbl;
    CComPtr<IMAPISession> pSession;
    SRowSet *prowset = NULL;
    SPropValue  *pval = NULL;
    SizedSPropTagArray (1, spta) = { 1, PR_DISPLAY_NAME };
   
    // Log onto MAPI
    hr = MAPILogonEx(0, NULL, NULL, 0, static_cast<LPMAPISESSION *>(&pSession));
    CHR(hr); // CHR will goto Error if FAILED(hr)
   
    // Get the table of accounts
    hr = pSession->GetMsgStoresTable(0, &ptbl);
    CHR(hr);
   
    // set the columns of the table we will query
    hr = ptbl->SetColumns ((SPropTagArray *) &spta, 0);
    CHR(hr);
   
    while (TRUE)
    {
        // Free the previous row
        FreeProws (prowset);
        prowset = NULL;
 
        hr = ptbl->QueryRows (1, 0, &prowset);
        if ((hr != S_OK) || (prowset == NULL) || (prowset->cRows == 0))
        {
            break;
        }
 
        ASSERT (prowset->aRow[0].cValues == spta.cValues);
        pval = prowset->aRow[0].lpProps;
 
        ASSERT (pval[0].ulPropTag == PR_DISPLAY_NAME);
 
        MessageBox(NULL, pval[0].Value.lpszW, TEXT("Message Store"), MB_OK);
    }
 
    pSession->Logoff(0, 0, 0);
 
Error:
    FreeProws (prowset);
    return hr;
}


http://blogs.msdn.com/windowsmobile/archive/2007/04/23/practical-use-of-mapi.aspx

*/



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* NO

#include <atlbase.h>
#include <cemapi.h>
#include <mapiutil.h>
#include <mapidefs.h>


class MAPIAdviseSink : public IMAPIAdviseSink
{
	public:
		MAPIAdviseSink();
		virtual ULONG OnNotify(ULONG cNotif, LPNOTIFICATION lpNotifications);
		virtual ULONG AddRef() { return ++refs; }
		virtual HRESULT QueryInterface( REFIID iid, void ** ppvObject) { return E_NOTIMPL; }
		virtual ULONG Release();
	
		HRESULT prevResult;
	private:
		ULONG refs;
};

MAPIAdviseSink *pAdviseSink;

MAPIAdviseSink::MAPIAdviseSink(): refs(0)
{
}

ULONG MAPIAdviseSink::OnNotify(ULONG cNotif, LPNOTIFICATION lpNotifications)
{
	//ToDO: rewrite here...

	MessageBox(NULL, TEXT("new message in"), TEXT("Message Store"), MB_OK);

	HRESULT hr;
	ICEMAPISession * pSession = NULL;

	hr = MAPIInitialize(NULL);
	hr = MAPILogonEx(0, NULL, NULL, 0, (LPMAPISESSION *)&pSession);

	static const SizedSPropTagArray (2, spta) = { 2, PR_DISPLAY_NAME, PR_ENTRYID };

	SRowSet *prowset = NULL;
	CComPtr ptbl;
	CComPtr pStore;

	// Get the table of accounts
	hr = pSession->GetMsgStoresTable(0, &ptbl);

	// set the columns of the table we will query
	hr = ptbl->SetColumns((SPropTagArray *) &spta, 0);

	while (TRUE)
	{
		// Free the previous row
		FreeProws (prowset);
		prowset = NULL;

		hr = ptbl->QueryRows (1, 0, &prowset);
		if ((hr != S_OK) || (prowset == NULL) || (prowset->cRows == 0))
			return -1;

		ASSERT (prowset->aRow[0].cValues == spta.cValues);
		SPropValue *pval = prowset->aRow[0].lpProps;

		ASSERT (pval[0].ulPropTag == PR_DISPLAY_NAME);
		ASSERT (pval[1].ulPropTag == PR_ENTRYID);

		if (!_tcscmp(pval[0].Value.lpszW, TEXT("SMS")))
		{
			// Get the Message Store pointer
			hr = pSession->OpenMsgStore(0, pval[1].Value.bin.cb, (LPENTRYID)pval[1].Value.bin.lpb, 0, 0, &pStore);

			static const SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { PR_MESSAGE_DELIVERY_TIME, TABLE_SORT_DESCEND } };
			static const SizedSPropTagArray (3, spta) = { 3, PR_SENDER_NAME, PR_SUBJECT, PR_MESSAGE_DELIVERY_TIME };
			HRESULT hr = S_OK;
			LPENTRYID pEntryId = NULL;
			ULONG cbEntryId = 0;
			CComPtr pFolder;
			CComPtr ptbl;
			ULONG ulObjType = 0;
			SRowSet *prowset = NULL;

			// Get the inbox folder
			hr = pStore->GetReceiveFolder(NULL, MAPI_UNICODE, &cbEntryId, &pEntryId, NULL);

			// 2 we have the entryid of the inbox folder, let's get the folder and messages in it
			hr = pStore->OpenEntry(cbEntryId, pEntryId, NULL, 0, &ulObjType, (LPUNKNOWN*)&pFolder);

			// 3 From the IMAPIFolder pointer, obtain the table to the contents
			hr = pFolder->GetContentsTable(0, &ptbl);

			// 4 Sort the table that we obtained. This is determined by the sortOrderSet variable
			hr = ptbl->SortTable((SSortOrderSet *)&sortOrderSet, 0);

			// 5 Set the columns of the table we will query. The columns of each row are determined by spta
			hr = ptbl->SetColumns ((SPropTagArray *) &spta, 0);

			// now get first message in the table
			// Free the previous row
			FreeProws (prowset);
			prowset = NULL;

			hr = ptbl->QueryRows (1, 0, &prowset);
			if ((hr != S_OK) || (prowset == NULL) || (prowset->cRows == 0))
				return -1;

			ASSERT (prowset->aRow[0].cValues == spta.cValues);
			SPropValue *pval = prowset->aRow[0].lpProps;

			// 6 Get the three properties we need: Sender name, Subject, and Delvery time.
			ASSERT (pval[0].ulPropTag == PR_SENDER_NAME);
			ASSERT (pval[1].ulPropTag == PR_SUBJECT);
			ASSERT (pval[2].ulPropTag == PR_MESSAGE_DELIVERY_TIME);

			LPCTSTR pszSender = pval[0].Value.lpszW;
			LPCTSTR pszSubject = pval[1].Value.lpszW;
			SYSTEMTIME st = {0};
			FileTimeToSystemTime(&pval[2].Value.ft, &st);

			// 7 Pass the parameters to a function to archive (this function is not written)
			//hr = AppendToFile(pszFilename, pszSender, pszSubject, st);
	MessageBox(NULL, pszSubject, TEXT("Message Store"), MB_OK);

		}
	}

	hr = pSession->Logoff(0, 0, 0);

	pSession->Release();
	pSession = NULL;
	MAPIUninitialize();

	MessageBox(NULL, lpNotifications->info.newmail.lpszMessageClass, TEXT("Message Store"), MB_OK);

	return 0;
}

ULONG MAPIAdviseSink::Release()
{
	return 0;
}

 
 
void doasda ()
{

HRESULT hr;
SRowSet *prowset = NULL;
CComPtr ptbl;
CComPtr pStore;

//---------------------
//IMAPIAdviseSink *pAdviseSink = NULL;
//ULONG m_ulAdviseConnection;

////HrThisThreadAdviseSink(new MyMsgStoreAdviseSink(),&msgStoreAdviseSink);

//hr = pSession->Advise(0, NULL, fnevNewMail | fnevObjectMoved , (LPMAPIADVISESINK)pAdviseSink, &m_ulAdviseConnection);
//
//if(hr == S_OK)
//{
// LPNOTIFICATION lpNotifications = NULL;
// ULONG res = pAdviseSink->OnNotify(0, lpNotifications);
// if(res != 0)
// AfxMessageBox(L"pAdviseSink->OnNotify FAIL");
// AfxMessageBox(L"pass");

// pAdviseSink->Release();
//}else{
// AfxMessageBox(L"pSession->Advise Fail"); 
}







*/












