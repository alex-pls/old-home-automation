/** @file    WindowsMobile.cpp
 *  @author  Alessandro Polo
 *  @version $Id: WindowsMobileImpl.cpp 698 2009-10-09 12:35:25Z alex $
 *  @brief
 * File containing methods for the wosh::devices::WindowsMobile class.
 * The header for this class can be found in WindowsMobile.h, check that file
 * for class description.
 ****************************************************************************/
/* Copyright (c) 2007-2009, WOSH - Wide Open Smart Home 
 * by Alessandro Polo - OpenSmartHome.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSmartHome.com WOSH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Alessandro Polo ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Alessandro Polo BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

 #include "WindowsMobileImpl.h"
 #include <core/Logger.h>

 #include <QtDebug>

#include "WindowsMobile.Common.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VoiceCallEx {
	public:
		VoiceCall* ptr;
		DWORD HCALL;

	public:

		VoiceCallEx()
		 {
			this->ptr = new VoiceCall();
			this->HCALL = 0;
		 }

		virtual ~VoiceCallEx()
		 {
			 if ( this->ptr != NULL )
				delete this->ptr;
			 this->ptr = NULL;
		 }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// declarations from WindowsMobile.Device.cpp
int GetPhoneNumber( std::string& phone_number, unsigned int nLineNumber );
int GetDeviceIDs( std::string& device_id_1, std::string& device_id_2 );
int GetExTAPIInfo( std::map<std::string, std::string>& fields );
int GetPowerState( std::string& power_state );

// declarations from WindowsMobile.POOM.cpp
int getPOOM_Version( string& poom_version );

// declarations from WindowsMobile.Messaging.cpp
int SendSMS( const std::string& recipent, const std::string& message, bool default_service = true, const std::string& server = "", bool send_confirmation = false );
int waitIncomingSms( long timeout );
typedef void (*MethodTapi_SmsNotify_Callback)(const struct sms_info& sms );
int monitorSms_stop();
int monitorSms_start( MethodTapi_SmsNotify_Callback callback );

// declarations from WindowsMobile.Dialing.cpp
typedef void (*MethodTapi_CallChanged_Callback)(const struct call_info& call );
int monitorCall_stop();
int monitorCall_start( MethodTapi_CallChanged_Callback callback );
int getLinesNumber();
int DialNumber( const std::string& phone_number );


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace wosh;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

WindowsMobileImpl* WindowsMobileImpl::WindowsMobile_instance = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MethodTapi_SmsNotify_Callback
void callback_SmsNotify( const struct sms_info& sms )
 {
	 if ( WindowsMobileImpl::WindowsMobile_instance == NULL ) {
		 printf("ERROR - callback_SmsNotify [WindowsMobile_instance=NULL]\n");
		 return;
	  }
	 if ( WindowsMobileImpl::WindowsMobile_instance->wmobileListener == NULL ) {
		 printf("ERROR - callback_SmsNotify [wmobileListener=NULL]\n");
		 return;
	  }

	printf("callback_SmsNotify.\n");
	WindowsMobileImpl::WindowsMobile_instance->wmobileListener->phone_SmsMessage(WindowsMobileImpl::WindowsMobile_instance, sms);
 }

// MethodTapi_CallChanged_Callback
void callback_CallChanged( const struct call_info& call )
 {
	 if ( WindowsMobileImpl::WindowsMobile_instance == NULL ) {
		 printf("ERROR - callback_CallChanged [WindowsMobile_instance=NULL]\n");
		 return;
	  }

	if ( call.line < 0 ) {
		 printf("ERROR - callback_CallChanged [call.line < 0]\n");
		 return;
	  }
	WindowsMobileImpl::WindowsMobile_instance->linesStatus.mutex().lockForWrite();
	if ( WindowsMobileImpl::WindowsMobile_instance->linesStatus.size_() < call.line ) {
		 printf("ERROR - callback_CallChanged [lines.size() < call.line]\n");
		 return;
	  }

	printf("callback_CallChanged.\n");

	VoiceCallEx* vCall = WindowsMobileImpl::WindowsMobile_instance->linesStatus.item_(call.line);
	vCall->HCALL = call._HCALL;

	switch(call.state) {
		case call_info::IDLE: {
			vCall->ptr->clear();
			vCall->ptr->setID(Utilities::randomLong());
			vCall->ptr->setIdle();
			break;
		 }
		case call_info::RINGING: {
			vCall->ptr->setIncomingCall();
			vCall->ptr->setRinging();
			break;
		 }
		case call_info::CONNECTED: {
			vCall->ptr->setCalledID(WindowsMobileImpl::getPhoneNumber());
			vCall->ptr->setCallerID(call.phone_number);
			vCall->ptr->setConnected();
			break;
		 }
		case call_info::DISCONNECTED: {
			vCall->ptr->clear();
			vCall->ptr->setDisconnected();
			break;
		 }
		default:			break;
	 }

	 if ( WindowsMobileImpl::WindowsMobile_instance->wmobileListener == NULL ) {
		WindowsMobileImpl::WindowsMobile_instance->linesStatus.mutex().unLock();
		 printf("ERROR - callback_CallChanged [wmobileListener=NULL]\n");
		 return;
	  }

	 WindowsMobileImpl::WindowsMobile_instance->wmobileListener->phone_CallState(WindowsMobileImpl::WindowsMobile_instance, vCall->ptr);

	WindowsMobileImpl::WindowsMobile_instance->linesStatus.mutex().unLock();

 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS

WindowsMobileImpl::WindowsMobileImpl()
 {
	WindowsMobileImpl::WindowsMobile_instance = this;
	this->wmobileListener = NULL;
	this->log = NULL;
 }

WindowsMobileImpl::~WindowsMobileImpl()
 {
	WindowsMobileImpl::WindowsMobile_instance = NULL;
 }

//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
WRESULT WindowsMobileImpl::startMonitoringSms()
 {
	WRESULT ret = 0;
	if ( log != NULL )
		log->log( Logger::VERBOSE, ":startMonitoringSms() : Initializing SMS Monitor.." );
	ret = monitorSms_start( callback_SmsNotify );
	if ( WSUCCEEDED(ret) ) {
		if ( log != NULL )
			log->log( Logger::INFO, ":startMonitoringSms() : Initialized SMS Monitor" );
	 }
	else {
		if ( log != NULL )
			log->log( Logger::CRITICAL, ":startMonitoringSms() : FAILED Initializing SMS Monitor ERROR#%i", ret );
	 }
	return ret;
 }

WRESULT WindowsMobileImpl::stopMonitoringSms()
 {
	WRESULT ret = 0;
	if ( log != NULL )
		log->log( Logger::VERBOSE, ":stopMonitoringSms(): Stopping SMS Monitor.." );
	ret = monitorSms_stop();
	if ( WSUCCEEDED(ret) ) {
		if ( log != NULL )
			log->log( Logger::INFO, ":stopMonitoringSms() : Initialized SMS Monitor" );
	 }
	else {
		if ( log != NULL )
			log->log( Logger::CRITICAL, ":stopMonitoringSms() : FAILED Stopping SMS Monitor ERROR#%i", ret );
	 }
	return ret;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

WRESULT WindowsMobileImpl::startMonitoringCall()
 {
	WRESULT ret = 0;
	if ( log != NULL )
		log->log( Logger::VERBOSE, ":startMonitoringCall(): Initializing CALL Monitor.." );
	ret = monitorCall_start( callback_CallChanged );
	if ( WSUCCEEDED(ret) ) {
		int lines = this->getPhoneLinesNumber();
		for (int i=0; i<lines; i++)
			this->linesStatus.add( new VoiceCallEx() );
		if ( this->wmobileListener != NULL )
			this->wmobileListener->phone_CallState(WindowsMobileImpl::WindowsMobile_instance, NULL );
		if ( log != NULL )
			log->log( Logger::INFO, ":startMonitoringCall(): Initializated of CALL Monitor, %i lines", lines );
	 }
	else {
		if ( log != NULL )
			log->log( Logger::CRITICAL, ":startMonitoringCall(): Initialization of CALL Monitor Returned %i", ret );
	 }

	return ret;
 }

WRESULT WindowsMobileImpl::stopMonitoringCall()
 {
	WRESULT ret = 0;
	if ( log != NULL )
		log->log( Logger::VERBOSE, ":stopMonitoringCall(): Stopping CALL Monitor.." );
	ret = monitorCall_stop();
	if ( WSUCCEEDED(ret) ) {
		if ( log != NULL )
			log->log( Logger::INFO, ":stopMonitoringCall() : Initialized CALL Monitor" );
	 }
	else {
		if ( log != NULL )
			log->log( Logger::CRITICAL, ":stopMonitoringCall() : FAILED Stopping CALL Monitor ERROR#%i", ret );
	 }
	return ret;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////// EVENTS

WRESULT WindowsMobileImpl::refreshContacts()
 {


	return WRET_OK;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WindowsMobileImpl::setWinMobileListener( IWindowsMobileListener* Listener )
 {
	MutexLocker mL(&configMux);
	this->wmobileListener = Listener;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////// STATIC Standalone

WRESULT WindowsMobileImpl::sendSms( const string& recipent, const string& message, bool default_service, const std::string& server, bool send_confirmation )
 {
	// feature from WindowsMobile.Messaging.cpp
	int ret = SendSMS( recipent, message, default_service, server, send_confirmation );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

WRESULT WindowsMobileImpl::dialNumber( const string& phone_number )
 {
	// feature from WindowsMobile.Dialing.cpp
	int ret = DialNumber( phone_number );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int WindowsMobileImpl::getPhoneLinesNumber() const
 {
	return getLinesNumber();
 }

WRESULT WindowsMobileImpl::getPhoneNumber( std::string& phone_number, int line )
 {
	// feature from WindowsMobile.Device.cpp
	int ret = GetPhoneNumber( phone_number, (unsigned int)line );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

std::string WindowsMobileImpl::getPhoneNumber( int line )
 {
	string phone_number = "";
	int ret = GetPhoneNumber( phone_number, (unsigned int)line );
	return phone_number;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

WRESULT WindowsMobileImpl::getDeviceInformations( std::map<std::string, std::string>& fields )
 {
	// methods from WindowsMobile.Device.cpp
	std::string device_id_1;
	std::string device_id_2;
	int ret = GetDeviceIDs( device_id_1, device_id_2 );
	fields["DeviceId#1"] = device_id_1;
	fields["DeviceId#2"] = device_id_1;

	std::string poom_version;
	ret += getPOOM_Version( poom_version );
	fields["PoomVersion"] = poom_version;

	std::string power_state;
	ret += GetPowerState( power_state );
	fields["PowerState"] = power_state;

	ret += GetExTAPIInfo( fields );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
