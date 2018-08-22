/**
 * @class   wosh::devices::WindowsMobile
 * @brief   
 *
 *
 *
 *
 ****************************************************************************
 * @version	$Id: WindowsMobile.h 21 2009-03-04 15:36:59Z alex $
 * @author	Alessandro Polo
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

#ifndef __WOSH_WindowsMobile_H__
 #define __WOSH_WindowsMobile_H__

 #include <woshDefs.h>
 #include <core/List.h>
 #include <core/Mutex.h>
 #include <core/ThreadWorker.h>

 #include <framework/VoiceCall.h>
 #include <framework/ContactInfo.h>

 #include <map>

struct call_info {
	int line;
	std::string phone_number;
	std::string alias;
	int state;
};

struct sms_info {
	std::string alias;
	std::string phone_number;
	std::string subject;
	std::string body;
	long timestamp;
	int priority;
};



using namespace std;
namespace wosh {
 namespace devices {

class WindowsMobile;


class IWindowsMobileListener  {
	public:
//		virtual void modem_ConnectionState( WindowsMobile* modem, ModemInterface::CONNECTION_STATE state ) = 0;
//		virtual void modem_StatsUpdate( WindowsMobile* modem, const pppStats& stats ) = 0;
		virtual void phone_SmsMessage( WindowsMobile* modem, const struct sms_info& info ) = 0;
		virtual void phone_CallState( WindowsMobile* modem, const struct call_info& info ) = 0;

	public:
		virtual ~IWindowsMobileListener() { }
};


class WindowsMobile : public ThreadWorker  {

	public:

		WindowsMobile();

		virtual ~WindowsMobile();


		WRESULT startMonitoringSms();
		WRESULT stopMonitoringSms();

		WRESULT startMonitoringCall();
		WRESULT stopMonitoringCall();


		static WRESULT sendSms( const string& recipent, const string& message, bool default_service = true, const std::string& server = "", bool send_confirmation = false );

		static WRESULT dialNumber( const string& phone_number );


		static WRESULT getPhoneNumber( std::string& phone_number, int line = 1 );

		static WRESULT getDeviceInformations( std::map<std::string, std::string>& fields );


/** @name Setters
 * @{
 ******************************************************************************/
 	public:

		void setModemListener( IWindowsMobileListener* listener );

//@}
/** @name Getters
 * @{
 ******************************************************************************/
 	public:

		inline IWindowsMobileListener* getModemListener()			{ return this->modemListener; }

		WRESULT refreshContacts();


//@}
/** @name Modem Interface
 * @{
 ******************************************************************************/
	public:


//@}
/** @name Internal methods
 * @{
 ******************************************************************************/
	protected:

		/**
		 * @brief   Perform periodic checkConnected() to update stats.
		 * @see     running
		 * @see     autoRefreshFrequency
		 */
		virtual void runThread();


//@}

	protected:

		IWindowsMobileListener* modemListener;				///< callbacks for stats (such as WindowsMobileBundle*)
		Mutex configMux;								///< Mutex of configuration (previous vars)

		List<ContactInfo> contacts;
		List<VoiceCall> linesStatus; // 1 line supported


		friend void callback_SmsNotify( const struct sms_info& sms );
		friend void callback_CallChanged( const struct call_info& call );

}; // class def

 }; // namespace devices
}; // namespace wosh

#endif //__WOSH_WindowsMobile_H__
