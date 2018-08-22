/**
 * @class   wosh::WindowsMobile
 * @brief   S
 *
 * This bundle is meant to run ON a Windows Mobile 6 device.


http://nah6.com/~itsme/cvs-xdadevtools/itsutils/leds/


 *
 *
 * Requirements:
 *  - You need Visual Studio 2008 (probably will wosk with 2005 too)
 *  - Windows Mobile SDK 6
 *  - QT Source for Windows (and build on your system)
 *  - Build WINCESRV target using 'wince.pri', executing 'qmake -tp vc' and building with VS
 *
 *
 * Installation on UNIX
 *  Steps:
 *  - Setup RNDIS driver
 *  - Configure Network
 *  - Install and start wincesv on device
 *
 *
 * @section Setup RNDIS driver
 * Verify USB link: plug device and type 'lsusb' to ensure device has been found.
 * Goto http://www.synce.org and follow instructions (reported here too)
 *
 * Bundle has beed tested using usb-rndis-lite-0.11.tar.gz (provided in /extras)
 * Make sure you have linux-headers installed (same release as current image).
 * Unplug device.
 *
 * @code
 *  $ tar zxf usb-rndis-lite-0.11.tar.gz
 *  $ cd usb-rndis-lite-0.11/
 *  $ make
 *  $ sudo ./clean.sh
 *  $ sudo make install
 *  $ cd ..
 * @endcode
 *
 * Make sure module has been installed and loaded executing 'lsmod | grep rndis'
 * Plug device, type \c dmesg (it should show you success/failure log)
 *
 * Type 'ifconfig -a' to list ALL interfaces, default name is \c rndis0 but it may
 * be different, check the HWaddress to recognize it and activace DHCP client:
 * 
 * @code
 *  $ dhclient3 rndis0
 * @endcode
 * 
 * Check again with \c ifconfig now it should be listed without '-a' and with a valid
 * IP (usually 169.254.2.2).
 * Confirm connection pinging device: ping 169.254.2.2
 *
 *----------------------------------------------------------------------------------
 *
 * in /etc/network/interfaces add:
 * @code
 *  iface rndis0 inet dhcp
 *  auto rndis0
 * @endcode
 *
 *
 *
 * @ingroup Devices
 ****************************************************************************
 * @version $Id: WindowsMobileBundle.h 739 2009-10-14 05:32:27Z alex $
 * @author  Alessandro Polo
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

#ifndef __WOSH_Bundles_Devices_WindowsMobile_H__
 #define __WOSH_Bundles_Devices_WindowsMobile_H__

 #include <core/BundleGeneric.h>

 #include <framework/communication/TextMessage.h>

 #include <interfaces/Device.h>
 #include <interfaces/devices/Phone.h>
 #include <interfaces/services/Communicator.h>

 #include "IWindowsMobileListener.h"

using namespace std;
namespace wosh {
 namespace devices {

 #define _WindowsMobile_NAME					"WindowsMobile"
 #define _WindowsMobile_VERSION					"0.6"

 #define _WindowsMobile_KEY_Manufacturer		"Manufacturer"
 #define _WindowsMobile_KEY_Model				"Model"
 #define _WindowsMobile_KEY_Revision			"Revision"
 #define _WindowsMobile_KEY_SerialNumber		"SerialNumber"
 #define _WindowsMobile_KEY_SubscriberNumber	"SubscriberNumber"
 #define _WindowsMobile_KEY_					""

 #define _WindowsMobile_METHOD_send_sms			"send_sms"
 #define _WindowsMobile_METHOD_dial_call		"dial_call"
 #define _WindowsMobile_METHOD_udpdate_info		"udpdate"

 #define _WindowsMobile_METHOD_					""


class WindowsMobileBundle :	public BundleGeneric, 
							public virtual wosh::interfaces::Device,
							public virtual wosh::interfaces::devices::Phone,
							public virtual wosh::interfaces::services::Communicator,
							public virtual IWindowsMobileListener  {

	public:

		/**
		 * @brief   Default constructor.
		 */
		WindowsMobileBundle();

		/**
		 * @brief   Deconstructor. If bundle is running, it will be stopped.
		 */
		virtual ~WindowsMobileBundle();


/** @name Bundle Interface
 * @{
 ******************************************************************************/
	public:
		virtual WRESULT bundleStart();
		virtual WRESULT bundleStop();

		virtual WRESULT bundleDiagnostic();

	public:

		virtual void busMessage( const Message& message, Bus* source );

//@}
/** @name Device Interface
 * @{
 ******************************************************************************/
	public:

		virtual bool isDeviceLinked() const					{ return (this->devState == wosh::interfaces::Device::LINKED); }
		virtual bool isDeviceUnLinked() const				{ return (this->devState == wosh::interfaces::Device::UNLINKED); }
		virtual bool isDeviceBroken() const					{ return (this->devState == wosh::interfaces::Device::BROKEN); }

		virtual wosh::interfaces::Device::DEVICE_STATE getDeviceState() const		{ return this->devState; }

//@}
/** @name Phone Interface
 * @{
 ******************************************************************************/
	public:
		virtual bool isPhoneConnected() const				{ return (this->connState == wosh::interfaces::devices::Phone::CONNECTED); }

		virtual bool isPhoneDisconnected() const			{ return (this->connState == wosh::interfaces::devices::Phone::DISCONNECTED); }

		virtual wosh::interfaces::devices::Phone::CONNECTION_STATE getConnectionState() const	{ return this->connState; }

//@}
/** @name Communicator Interface
 * @{
 ******************************************************************************/
	public:
		virtual bool isUserReachable( const string& wosh_username );

		virtual WRESULT sendMessageToUser( const string& wosh_username, const string& message );

		virtual WRESULT sendMessageTo( const string& user_phonenumber, const string& message );

		virtual WRESULT notify( const Notification* notification );

//@}
/** @name WindowsMobile Events
 * @{
 ******************************************************************************/
 	public:
//		virtual void modem_ConnectionState( WindowsMobile* modem, ModemInterface::CONNECTION_STATE state );
//		virtual void modem_StatsUpdate( WindowsMobile* modem, const pppStats& stats );

		virtual void phone_CallState( WindowsMobileImpl* modem, const VoiceCall* info );
		virtual void phone_SmsMessage( WindowsMobileImpl* modem, const struct sms_info& info );

//@}
/** @name Internal methods
 * @{
 ******************************************************************************/
	protected:

		Response* mmDoSendSms( const Message* messageRequest );
		Response* mmDoDialCall( const Message* messageRequest );
		Response* mmDoSendMessageToUser( const Message* requestMessage );

		WRESULT updateInfos();
		std::string getPhoneNumberFromUsername( const string& wosh_username );
		std::string getUsernameFromPhoneNumber( const string& phone_number );


		void raiseEvent( Fact* fact );

//@}
/** @name Internal vars
 * @{
 ******************************************************************************/
	protected:
		BusConnector BusDevices;										///< connector to Devices bus

		WindowsMobileImpl* winWorker;									///< real implementation

		wosh::interfaces::devices::Phone::CONNECTION_STATE connState;	///< current state of the modem
		wosh::interfaces::Device::DEVICE_STATE devState;				///< current state of device

//@}


}; // class def

 }; // namespace devices
}; // namespace wosh

#endif //__WOSH_Bundles_Devices_WindowsMobile_H__
