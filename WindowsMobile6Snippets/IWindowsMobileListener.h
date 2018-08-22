/**
 * @class   IWindowsMobileListener
 * @brief   
 *
 *
 *
 *
 ****************************************************************************
 * @version	$Id: IWindowsMobileListener.h 647 2009-10-03 01:43:48Z alex $
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

#ifndef __WOSH_IWindowsMobileListener_H__
 #define __WOSH_IWindowsMobileListener_H__

 #include <woshDefs.h>

 #include <framework/communication/VoiceCall.h>
 #include <framework/communication/ContactInfo.h>


using namespace std;
using namespace wosh;
using namespace wosh::communication;

class WindowsMobileImpl;


struct sms_info {
	std::string alias;
	std::string phone_number;
	std::string subject;
	std::string body;
	long timestamp;
	int priority;
};



class IWindowsMobileListener  {

	public:

//		virtual void modem_ConnectionState( WindowsMobile* modem, ModemInterface::CONNECTION_STATE state ) = 0;
//		virtual void modem_StatsUpdate( WindowsMobile* modem, const pppStats& stats ) = 0;
		virtual void phone_SmsMessage( WindowsMobileImpl* modem, const struct sms_info& info ) = 0;
		virtual void phone_CallState( WindowsMobileImpl* modem, const VoiceCall* call_line ) = 0;

	public:

		virtual ~IWindowsMobileListener() { }


}; // class def


#endif //__WOSH_IWindowsMobileListener_H__
