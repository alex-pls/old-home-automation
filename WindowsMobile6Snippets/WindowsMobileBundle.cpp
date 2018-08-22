/** @file    WindowsMobileBundle.cpp
 *  @author  Alessandro Polo
 *  @version $Id: WindowsMobileBundle.cpp 740 2009-10-14 06:26:14Z alex $
 *  @brief
 * File containing methods for the wosh::WindowsMobileBundle class.
 * The header for this class can be found in WindowsMobileBundle.h, check that file
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

 #include "WindowsMobileBundle.h"

 #include "WindowsMobileImpl.h"

 #include <core/MethodsDef.h>
 #include <core/Fact.h>
 #include <core/ThreadManager.h>
 #include <core/UserManager.h>


using namespace std;
namespace wosh {
 namespace devices {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS

WindowsMobileBundle::WindowsMobileBundle() : BundleGeneric()
 {
	BundleGeneric::setName( _WindowsMobile_NAME );
	BundleGeneric::setType( "wosh::devices::Phone.WindowsMobile" );
	Properties.setProperty( _Bundle_KEY_Version, _WindowsMobile_VERSION );
	Interfaces.add( _Device_TYPE );
	Interfaces.add( wosh::interfaces::services::Communicator::getInterfaceName() );
//	Interfaces.add( ModemInterface::getInterfaceName() );

Log.setLevel( Logger::VERBOSE );

	Log.log( Logger::VERBOSE, " Configuring WindowsMobile worker.." );
	this->winWorker = new WindowsMobileImpl();
	this->winWorker->setLogger( &Log );
	this->winWorker->setWinMobileListener(this);

	this->connState = wosh::interfaces::devices::Phone::UNKNOWN;
	this->devState = wosh::interfaces::Device::UNKNOWN;

	Log.log( Logger::VERBOSE, " Setting default properties and permissions.." );

	Properties.setProperty( _WindowsMobile_KEY_Manufacturer,		"", Permission( Permission::Read ) );
	Properties.setProperty( _WindowsMobile_KEY_Model,				"", Permission( Permission::Read ) );
	Properties.setProperty( _WindowsMobile_KEY_Revision,			"", Permission( Permission::Read ) );
	Properties.setProperty( _WindowsMobile_KEY_SerialNumber,		"", Permission( Permission::Read ) );
	Properties.setProperty( _WindowsMobile_KEY_SubscriberNumber,	"", Permission( Permission::Read ) );
//	Properties.setProperty( ,	"", Permission( Permission::Read ) );

	Properties.setProperty( _Phone_KEY_PhoneNumber,	"", Permission( Permission::Read ) );
	Properties.setProperty( _Phone_KEY_PhoneLinesNumber, 0, Permission( Permission::Read ) );

	Log.log( Logger::VERBOSE, " Registering methods.." );
	MethodMessageResponse* mmSendMessageToUser = new MethodMessageResponse( _Communicator_METHOD_sendmessage, "send message to (wosh) username" );
	mmSendMessageToUser->setMethod( this, (MethodMessageResponsePtr)&WindowsMobileBundle::mmDoSendMessageToUser );
	mmSendMessageToUser->getPermission().setMask( Permission::RX, Permission::RX, Permission::RX );
	Methods.registerMethod(mmSendMessageToUser);

	MethodMessageResponse* mmSendSms = new MethodMessageResponse( _WindowsMobile_METHOD_send_sms, "send sms message" );
	mmSendSms->setMethod( this, (MethodMessageResponsePtr)&WindowsMobileBundle::mmDoSendSms );
	mmSendSms->getPermission().setMask( Permission::RX, Permission::RX, Permission::Read );
	Methods.registerMethod(mmSendSms);
	MethodMessageResponse* mmDialCall = new MethodMessageResponse( _WindowsMobile_METHOD_dial_call, "dial a call" );
	mmDialCall->setMethod( this, (MethodMessageResponsePtr)&WindowsMobileBundle::mmDoDialCall );
	mmDialCall->getPermission().setMask( Permission::RX, Permission::RX, Permission::Read );
	Methods.registerMethod(mmDialCall);
	MethodWRESULT* mmUpdateInfos = new MethodWRESULT( _WindowsMobile_METHOD_udpdate_info, "Update Device informations" );
	mmUpdateInfos->setMethod( this, (MethodWRESULTPtr)&WindowsMobileBundle::updateInfos );
	mmUpdateInfos->getPermission().setMask( Permission::RX, Permission::RX, Permission::Read );
	Methods.registerMethod(mmUpdateInfos);

	BusDevices.setMessageHandler(this);
	WRESULT bus_connected = BusDevices.connect( "wosh.Bus.Devices" );
	if ( WFAILED(bus_connected) )
		Log.log( Logger::CRITICAL, ":WindowsMobileBundle() : FAILED#%i Connecting Bus (wosh.Bus.Devices)..", bus_connected );

	setBundleState(Bundle::CREATED, false);
 }


WindowsMobileBundle::~WindowsMobileBundle()
 {
	Log.log( Logger::INFO, " Destroying.." );

	if ( isBundleRunning() ) {
		Log.log( Logger::WARNING, "~WindowsMobileBundle() : Destroying while Bundle is running! Trying to stop.." );
		bundleStop();
	 }

	delete this->winWorker; this->winWorker = NULL;

	Log.log( Logger::VERBOSE, ":~WindowsMobileBundle() : Destroyed." );
 }

//////////////////////////////////////////////////////////////////////////////////////////////// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////// BUNDLE CONTROL

WRESULT WindowsMobileBundle::bundleStart()
 {
	if ( !BundleGeneric::bundleValidate_StartStop(Bundle::STARTING) )
		return WRET_ERR_WRONG_STATE;

	updateInfos();

	WRESULT ret = WRET_OK;

	// must be called within WINMAIN thread!
	ret += this->winWorker->startMonitoringSms();
	ret += this->winWorker->startMonitoringCall();

	setBundleState( Bundle::STARTED );
	//ret = BundleGeneric::start_SynchThread( this->winWorker );
	// BUNDLE-STATE (STARTED) will be updated async by WORKER, through call: IThreadListener::thread_event()

	return ret;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

WRESULT WindowsMobileBundle::bundleStop()
 {
	if ( !BundleGeneric::bundleValidate_StartStop(Bundle::STOPPING) )
		return WRET_ERR_WRONG_STATE;

	WRESULT ret = WRET_OK;
	
	// must be called within WINMAIN thread?
	ret += this->winWorker->stopMonitoringSms();
	ret += this->winWorker->stopMonitoringCall();

	setBundleState( Bundle::STOPPED );
	//ret = BundleGeneric::stop_SynchThread( this->winWorker );
	// BUNDLE-STATE (STOPPED) will be updated async by WORKER, through call: IThreadListener::thread_event()

	return ret;
 }

WRESULT WindowsMobileBundle::bundleDiagnostic()
 {
	Log.log( Logger::VERBOSE, ":bundleDiagnostic() : Starting WindowsMobileBundle-Diagnostic" );

	WRESULT ret = BundleGeneric::bundleDiagnostic();


	Log.log( Logger::INFO, ":bundleDiagnostic() : Finished WindowsMobileBundle-Diagnostic [%i]", ret );
	return ret;
 }

////////////////////////////////////////////////////////////////////////////////////////////// BUNDLE CONTROL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////// PROCESS BUS MESSAGES

void WindowsMobileBundle::busMessage( const Message& message, Bus* source )
 {
	BundleGeneric::busMessage( message, source );

	if ( !message.getContent()->isNotification() )
		return;


	///@todo eval notification
	notify( message.getContent()->asNotification() );
 }

//////////////////////////////////////////////////////////////////////////////////////// PROCESS BUS MESSAGES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////// GENERATED EVENTS

void WindowsMobileBundle::raiseEvent( Fact* fact )
 {
	Message* msg_event = new Message();
	msg_event->setSource( this );
	msg_event->setContent( fact );
	msg_event->setDestinationBroadcast();
	///@todo must define micro and macro routing strategies!
	// BusDevices.postMessage(msg_event);
	BusCore.postMessage(msg_event);
 }

//////////////////////////////////////////////////////////////////////////////////////////// GENERATED EVENTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////// PROPERTY EVENT

////////////////////////////////////////////////////////////////////////////////////////////// PROPERTY EVENT
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////// WM6 EVENTS

void WindowsMobileBundle::phone_CallState( WindowsMobileImpl* modem, const VoiceCall* call_info )
 {
	(void)modem;
	if ( call_info == NULL ) {
		int lines = this->winWorker->getPhoneLinesNumber();
		Properties.setProperty( _Phone_KEY_PhoneLinesNumber, lines );
		return;
	 }
	Log.log( Logger::VERBOSE, ":phone_CallState() CALL#%i from %s", call_info->getID(), call_info->getCallerID().c_str() );

	Fact* fact = new Fact( _Phone_EVENT_call_incoming, call_info->clone() );
	this->raiseEvent(fact);
 }

void WindowsMobileBundle::phone_SmsMessage( WindowsMobileImpl* modem, const struct sms_info& info )
 {
	(void)modem;
	std::string username = getUsernameFromPhoneNumber(info.phone_number);

	if ( username.size() == 0 ) {
		Log.log( Logger::WARNING, ":phone_SmsMessage() SMS from UNKNOWN PHONE %s : '%s'", info.phone_number.c_str(), info.body.c_str() );
	 }
	else
		Log.log( Logger::VERBOSE, ":phone_SmsMessage() SMS from %s [%s] : '%s'", info.phone_number.c_str(), username.c_str(), info.body.c_str() );

	///@todo EVAL USER/SOURCE vs. SECURITY RULES here

	// inject TEXT_MESSAGE into WOSH
	TextMessage* txtmsg = new TextMessage();
	txtmsg->setMessageSMS();
	txtmsg->setSender_ID( info.phone_number );
	txtmsg->setSender_User( username );
	txtmsg->setRecipent_ID( Properties.getPropertyValue(_Phone_KEY_PhoneNumber).getData() );
	txtmsg->setRecipent_User( "wosh" );  // phone device is considered as a communicator, so target is always the kernel-user
	txtmsg->setMessage( info.body );

	Fact* fact = new Fact( _Communicator_EVENT_Message, txtmsg );
	this->raiseEvent(fact);
 }

////////////////////////////////////////////////////////////////////////////////////////////////// WM6 EVENTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////// WM6 METHODS

WRESULT WindowsMobileBundle::updateInfos()
 {
	std::string phone_number;
	WRESULT ret = WindowsMobileImpl::getPhoneNumber( phone_number, 1 );
	Properties.setProperty( _Phone_KEY_PhoneNumber, phone_number );

	if ( WSUCCEEDED(ret) )
		Log.log( Logger::VERBOSE, ":updateInfos() PhoneNumber#1: %s", phone_number.c_str() );
	else
		Log.log( Logger::WARNING, ":updateInfos() Error Retriving PhoneNumber#1: %s", phone_number.c_str() );

	std::map<std::string, std::string> fields;
	ret = WindowsMobileImpl::getDeviceInformations( fields );
	std::map<std::string,std::string>::const_iterator it;
	for ( it=fields.begin(); it!=fields.end(); it++ ) {
		Properties.setProperty( it->first, it->second );
	 }

	if ( WSUCCEEDED(ret) )
		Log.log( Logger::VERBOSE, ":updateInfos() Updated %i fields", fields.size() );
	else
		Log.log( Logger::WARNING, ":updateInfos() Error Retriving fields [%i]", fields.size() );

	return ret;
 }

///////////////////////////////////////////////////////////////////////////////////////////////// WM6 METHODS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////// COMMUNICATOR

bool WindowsMobileBundle::isUserReachable( const string& wosh_username )
 {
	std::string phonenumber = getPhoneNumberFromUsername( wosh_username );

	///< @bug should at least validate number
	if ( phonenumber.size() > 8 )
		return true;

	return false;
 }

WRESULT WindowsMobileBundle::sendMessageToUser( const string& wosh_username, const string& message )
 {
	WRESULT ret = WRET_OK;

	std::string phonenumber = getPhoneNumberFromUsername(wosh_username);
	if ( phonenumber.size() == 0 ) {
		Log.log( Logger::CRITICAL, "sendMessageToUser(%s, **) FAILED : Can't retrieve phonenumber of User", wosh_username.c_str() );
		return WRET_ERR_PARAM;
	}

	ret = sendMessageTo(phonenumber, message );
	return ret;
 }

WRESULT WindowsMobileBundle::sendMessageTo( const string& user_phonenumber, const string& message )
 {
	WRESULT ret = WRET_OK;

	ret = WindowsMobileImpl::sendSms( user_phonenumber, message );
	if ( WFAILED(ret) )
		Log.log( Logger::CRITICAL, "sendMessageTo(%s, **) FAILED Sending SMS, ERROR#%i", user_phonenumber.c_str(), ret );
	else
		Log.log( Logger::INFO, "sendMessageTo(%s, **) SMS message sent", user_phonenumber.c_str() );

	return ret;
 }

WRESULT WindowsMobileBundle::notify( const Notification* notification )
 {
	if ( notification == NULL ) return WRET_ERR_PARAM;
	WRESULT ret = WRET_OK;

	if ( notification->isForAnyone() || notification->getUserName().size() == 0 ) {

		// should iterate when group-select or/and ?
		ret = WRET_ERR_INTERNAL;
	 }
	else
		return sendMessageToUser(notification->getUserName(), notification->getSummary(255) );

	return ret;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string WindowsMobileBundle::getPhoneNumberFromUsername( const string& wosh_username )
 {
	bool found = UserManager::isUser(wosh_username);
	if ( !found ) {
		return "";
	 }
	return UserManager::getUserProperty(wosh_username, "PhoneMobile").getData();
 }

std::string WindowsMobileBundle::getUsernameFromPhoneNumber( const string& phone_number )
 {
	std::vector<std::string> users = UserManager::findUserByProperty( "PhoneMobile", phone_number );

	if ( users.size() == 0 )
		return "";
	return users[0];
 }


//////////////////////////////////////////////////////////////////////////////////////////////// COMMUNICATOR
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////// METHODS

Response* WindowsMobileBundle::mmDoSendSms( const Message* requestMessage )
 {
	if ( requestMessage == NULL || requestMessage->isEmpty() ) return NULL;
	if ( !requestMessage->getContent()->isRequest() ) return NULL;
	const Request* request = requestMessage->getContent()->asRequest();

	// retrieve options
	std::string recipent = "";
	std::string message = "";

	if ( request->getDataClassName() == "wosh::DataList" ) {
		const DataList* datalist_source = dynamic_cast<const DataList*>( request->getData() );
		if ( datalist_source->size() < 2 ) {
			Log.log( Logger::CRITICAL, "mmDoSendMessageToUser() : Invalid arguments" );
			return new Response( request->getMethod(), WRET_ERR_PARAM, "Wrong DataType: DataList too short (recipent; message)" );
		 }
		recipent = datalist_source->get(0).getData();
		message = datalist_source->get(1).getData();
	 }

	if ( recipent.size() < 5 )
		return new Response( request->getMethod(), WRET_ERR_PARAM, "Bad params" );

	if ( recipent[0] != '+' )
		recipent = "+" + recipent;
	if (  recipent[0] != '3' || recipent[1] != '9' )
		recipent = "39" + recipent;

	std::string owner = "";
	if ( requestMessage->getSecurityPayload() != NULL )
		owner = requestMessage->getSecurityPayload()->getUser();

	WRESULT ret = WindowsMobileImpl::sendSms( recipent, message );

	if ( WSUCCEEDED(ret) )
		return new Response( request->getMethod(), ret, "Sent!" );
	return new Response( request->getMethod(), ret, "Internal Error!" );
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Response* WindowsMobileBundle::mmDoSendMessageToUser( const Message* requestMessage )
 {
	if ( requestMessage == NULL || requestMessage->isEmpty() ) return NULL;
	if ( !requestMessage->getContent()->isRequest() ) return NULL;
	const Request* request = requestMessage->getContent()->asRequest();

	// retrieve options
	std::string username = "";
	std::string message = "";
	if ( strcmp(request->getData()->getClassName(), "wosh::DataList") == 0 ) {
		const DataList* dl = dynamic_cast<const DataList*>(request->getData());
		if ( dl->size() < 2 ) {
			Log.log( Logger::CRITICAL, "mmDoSendMessageToUser() : Invalid arguments" );
			return new Response( request->getMethod(), WRET_ERR_PARAM, "Invalid arguments [username, message]" );
		 }
		username = dl->get(0).getData();
		message = dl->get(1).getData();
	 }

	std::string sender = requestMessage->getSecurityPayload()->getUser();

	Log.log( Logger::VERBOSE, "mmDoSendMessageToUser(%s) : Sending %i chars", username.c_str(), message.size() );
	WRESULT ret = sendMessageToUser( username, message );

	if ( WSUCCEEDED(ret) )
		return new Response( request->getMethod(), ret, "Sent!" );
	return new Response( request->getMethod(), ret, "Internal Error!" );
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Response* WindowsMobileBundle::mmDoDialCall( const Message* requestMessage )
 {
	if ( requestMessage == NULL || requestMessage->isEmpty() ) return NULL;
	if ( !requestMessage->getContent()->isRequest() ) return NULL;
	const Request* request = requestMessage->getContent()->asRequest();

	// retrieve options
	std::string recipent = request->getArgument().getData();
	if ( recipent.size() < 5 )
		return new Response( request->getMethod(), WRET_ERR_PARAM, "Bad params" );

	if ( recipent[0] != '+' )
		recipent = "+" + recipent;
	if (  recipent[1] != '3' || recipent[2] != '9' )
		recipent = "39" + recipent;

	WRESULT ret = WindowsMobileImpl::dialNumber( recipent );

	return new Response( request->getMethod(), ret );
 }

///////////////////////////////////////////////////////////////////////////////////////////////////// METHODS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

 }; // namespace devices
}; // namespace wosh
