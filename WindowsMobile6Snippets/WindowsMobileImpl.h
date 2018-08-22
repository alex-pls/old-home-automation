/**
 * @class   wosh::devices::WindowsMobile
 * @brief   
 *
 *
 *
 *
 ****************************************************************************
 * @version	$Id: WindowsMobileImpl.h 540 2009-09-23 01:27:50Z alex $
 * @author	Alessandro Polo
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

#ifndef __WOSH_WindowsMobile_H__
 #define __WOSH_WindowsMobile_H__

 #include <woshDefs.h>
 #include <core/List.h>
 #include <core/Mutex.h>
 #include <core/Logger.h>


 #include "IWindowsMobileListener.h"


using namespace std;
using namespace wosh;

class VoiceCallEx;

class WindowsMobileImpl  {

	public:

		/**
		 * @brief   Default constructor. Init vars and static instance (reference).
		 * @see     WindowsMobileImpl::WindowsMobile_instance
		 */
		WindowsMobileImpl();

		/**
		 * @brief   Deconstructor. If thread is running, it will be stopped.
		 */
		virtual ~WindowsMobileImpl();


	public:

		WRESULT startMonitoringSms();
		WRESULT stopMonitoringSms();

		WRESULT startMonitoringCall();
		WRESULT stopMonitoringCall();

		void setWinMobileListener( IWindowsMobileListener* listener );
		void setLogger( Logger* logger )								{ this->log = logger; }

		inline IWindowsMobileListener* getWinMobileListener()			{ return this->wmobileListener; }

		int getPhoneLinesNumber() const;


/** @name Standalone methods
 * @{
 ******************************************************************************/
 	public:

		static WRESULT sendSms( const string& recipent, const string& message,
								bool default_service = true, const std::string& server = "", bool send_confirmation = false );

		static WRESULT dialNumber( const string& phone_number );


		static WRESULT getPhoneNumber( std::string& phone_number, int line = 1 );
		static std::string getPhoneNumber( int line = 1 );

		static WRESULT getDeviceInformations( std::map<std::string, std::string>& fields );

//@}
/** @name Internal methods
 * @{
 ******************************************************************************/
	protected:

		WRESULT refreshContacts();

//@}
/** @name Internal vars
 * @{
 ******************************************************************************/
	protected:

		IWindowsMobileListener* wmobileListener;	///< callbacks for stats (such as WindowsMobileBundle*)
		Mutex configMux;							///< Mutex of configuration (previous vars)

		List<ContactInfo*> contacts;				///< contacts saved on the phone/SIM
		List<VoiceCallEx*> linesStatus;				///< supported lines

		Logger* log;

//@}
/** @name callbacks methods
 * @{
 ******************************************************************************/
	protected:
		friend void callback_SmsNotify( const struct sms_info& sms );
		friend void callback_CallChanged( const struct call_info& call );

//@}
	private:
		static WindowsMobileImpl* WindowsMobile_instance;


}; // class def


#endif //__WOSH_WindowsMobile_H__
