/** @file    WindowsMobile.cpp
 *  @author  Alessandro Polo
 *  @version $Id: Modem.Dsl.cpp 21 2009-03-04 15:36:59Z alex $
 *  @brief
 * File containing methods for the wosh::devices::WindowsMobile class.
 * The header for this class can be found in WindowsMobile.h, check that file
 * for class description.
 ****************************************************************************/
/* Copyright (c) 2009, WOSH - Wide Open Smart Home 
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

 #include "WindowsMobile.h"
 #include <core/Logger.h>

 #include <QtDebug>

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
int DialNumber( const std::string& phone_number );

namespace wosh {
 namespace devices {


WindowsMobile* WindowsMobile_instance = NULL;

// MethodTapi_SmsNotify_Callback
void callback_SmsNotify( const struct sms_info& sms )
 {
	 if ( WindowsMobile_instance == NULL ) {
		 printf("ERROR - callback_SmsNotify [WindowsMobile_instance=NULL]\n");
		 return;
	  }
	 if ( WindowsMobile_instance->modemListener == NULL ) {
		 printf("ERROR - callback_SmsNotify [modemListener=NULL]\n");
		 return;
	  }

	printf("callback_SmsNotify.\n");
	WindowsMobile_instance->modemListener->phone_SmsMessage(WindowsMobile_instance, sms);
 }


// MethodTapi_CallChanged_Callback
void callback_CallChanged( const struct call_info& call )
 {
	 if ( WindowsMobile_instance == NULL ) {
		 printf("ERROR - callback_SmsNotify [WindowsMobile_instance=NULL]\n");
		 return;
	  }
	 if ( WindowsMobile_instance->modemListener == NULL ) {
		 printf("ERROR - callback_SmsNotify [modemListener=NULL]\n");
		 return;
	  }

	printf("callback_CallChanged.\n");
	//WindowsMobile_instance->modemListener->phone_CallState(WindowsMobile_instance, call);
 }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS

WindowsMobile::WindowsMobile() : ThreadWorker("wosh.Device.WindowsMobile.WorkerThread")
 {
	WindowsMobile_instance = this;
	this->modemListener = NULL;
 }

WindowsMobile::~WindowsMobile()
 {
	this->running = false;

	WindowsMobile_instance = NULL;

	 if ( isThreadRunning() ) {
		quitThread();
	 }

 }

//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////// THREAD RUN

WRESULT WindowsMobile::startMonitoringSms()
 {
	ASSERT( this->log != NULL )
	int ret = 0;
	log->log( Logger::VERBOSE, " worker:runThread(): Initializing SMS Monitor.." );
	ret = monitorSms_start( callback_SmsNotify );
	log->log( Logger::INFO, " worker:runThread(): Initialization of SMS Monitor Returned %i", ret );
	return ret;
 }

WRESULT WindowsMobile::stopMonitoringSms()
 {
	ASSERT( this->log != NULL )
	int ret = 0;
	log->log( Logger::VERBOSE, " worker:runThread(): Stopping SMS Monitor.." );
	ret = monitorSms_stop();
	log->log( Logger::INFO, " worker:runThread(): Stop of SMS Monitor Returned %i", ret );
	return ret;
 }

WRESULT WindowsMobile::startMonitoringCall()
 {
	ASSERT( this->log != NULL )
	int ret = 0;
	log->log( Logger::VERBOSE, " worker:runThread(): Initializing CALL Monitor.." );
	ret = monitorCall_start( callback_CallChanged );
	log->log( Logger::INFO, " worker:runThread(): Initialization of CALL Monitor Returned %i", ret );
	return ret;
 }

WRESULT WindowsMobile::stopMonitoringCall()
 {
	ASSERT( this->log != NULL )
	int ret = 0;
	log->log( Logger::VERBOSE, " worker:runThread(): Stopping CALL Monitor.." );
	ret = monitorCall_stop();
	log->log( Logger::INFO, " worker:runThread(): Stop of CALL Monitor Returned %i", ret );
	return ret;
 }

void WindowsMobile::runThread()
 {
	this->running = true;

	ASSERT( this->log != NULL )

	int ret = 0;

	// cant be called from this thread.. BUT from WINMAIN thread!
//	startMonitoringSms();

//	startMonitoringCall():

	while( this->running ) {
		Thread::sleepForMSec(500);
	 }

//	stopMonitoringCall();

//	stopMonitoringSms();

 }

////////////////////////////////////////////////////////////////////////////////////////////////// THREAD RUN
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////// EVENTS

WRESULT WindowsMobile::refreshContacts()
 {


	return WRET_OK;
 }



////////////////////////////////////////////////////////////////////////////////////////////////////// EVENTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////// MODEM METHODS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////// MODEM METHODS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WindowsMobile::setModemListener( IWindowsMobileListener* Listener )
 {
	MutexLocker mL(&configMux);
	this->modemListener = Listener;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////// STATIC Standalone

WRESULT WindowsMobile::getPhoneNumber( std::string& phone_number, int line )
 {
	int ret = GetPhoneNumber( phone_number, (unsigned int)line );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

WRESULT WindowsMobile::getDeviceInformations( std::map<std::string, std::string>& fields )
 {
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

WRESULT WindowsMobile::sendSms( const string& recipent, const string& message, bool default_service, const std::string& server, bool send_confirmation )
 {
	int ret = SendSMS( recipent, message, default_service, server, send_confirmation );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

WRESULT WindowsMobile::dialNumber( const string& phone_number )
 {
	int ret = DialNumber( phone_number );
	if ( ret == 0 )
		return WRET_OK;
	return WRET_ERR_INTERNAL;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

 }; // namespace devices
}; // namespace wosh



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//


