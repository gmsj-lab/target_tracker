#ifndef TARGET_TRACKER_ESP8266_H
#define TARGET_TRACKER_ESP8266_H
/*
 *  Target Tracker esp8266
 *
 *  Created on: September 02, 2016
 *  Updated on: -
 *      by Gilles Monzat de Saint Julien (GMSJ)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
 
// Define all possible tracker flavours
#include "tracker_flavours.h"

// Define tracker flavour
#define BOARD_TYPE 				ESP_8266
#define TELNET_PROTOCOL_TYPE 	_TCP

// Tracker flavour specific includes
#include "tracker_config.h"

// Base class
#include "_tracker.h"

//---------------------------------------------------------------------------
//                              ESP Tracker class
//---------------------------------------------------------------------------
class TargetTracker : public _TargetTracker
{
private:

	WifiSetup wifi ;

	TrackerSerialComm 	serialComm ;
	TrackerUdpComm  	udpComm ;
	TrackerTcpComm  	tcpComm ;

	// Constructor private. non-copyable, non-assignable.
	TargetTracker ( Tracker & ) ;
	TargetTracker & operator = ( const Tracker & ) ;

	bool beginUdp ( const int localUdpPort ) {
		udpComm.setConfig ( localUdpPort ) ;
		Tracker::getTracker ().setCommunicationMedia1 ( & udpComm ) ;
		return Tracker::getTracker ().begin () ;
	}
	bool beginTcp ( const int localTcpPort ) {
		tcpComm.setConfig ( localTcpPort ) ;
		Tracker::getTracker ().setCommunicationMedia1 ( & tcpComm ) ;
		return Tracker::getTracker ().begin () ;
	}
	bool beginUdpAndTcp ( const int localUdpPort , const int localTcpPort ) {
		udpComm.setConfig ( localUdpPort ) ;
		Tracker::getTracker ().setCommunicationMedia1 ( & udpComm ) ;
		tcpComm.setConfig ( localTcpPort ) ;
		Tracker::getTracker ().setCommunicationMedia2 ( & tcpComm ) ;
		return Tracker::getTracker ().begin () ;
	}
	void beginWifi ( const char * ssid , const char * password , bool accessPoint , const char * ip = NULL ) {
		wifi.connect ( ssid , password , accessPoint , ip ) ;
		delay ( 500 ) ;
	}
	bool beginTelnet ( const uint32_t serialLineRate , const uint32_t localPort ) {
		#if   TELNET_PROTOCOLE_TYPE == _UDP
			TrackerComplement * trackerTelnet = new TrackerUdpTelnet () ;
		#else
			TrackerComplement * trackerTelnet = new TrackerTcpTelnet () ;
		#endif
		Tracker::getTracker ().setComplement ( trackerTelnet ) ;
		trackerTelnet->begin ( serialLineRate , localPort ) ;
		return true ;
	}

public:

	TargetTracker ( const char * targetName , bool timeTagging = true ) : _TargetTracker ( targetName , timeTagging = true ) {}
	virtual ~TargetTracker () {}

	// Option 1: Start Tracker on ftdi line only
	bool begin ( const int ftdiLineRate ) {
		serialComm.setConfig ( ftdiLineRate ) ;
		Tracker::getTracker ().setCommunicationMedia1 ( & serialComm ) ;
		return Tracker::getTracker ().begin () ;
	}
	// Option 2: (for ESP only) Start Tracker wifi with udp only, tcp only or udp & tcp tracker connection plus optionnal telnet connection (on UDP or TCP by #define)
	bool begin ( const char * ssid , const char * password , bool accessPoint , const char * ip,
				 const int localUdpPort , const int localTcpPort,
				 const uint32_t serialLineRate , const uint32_t localTelnetPort ) {
		bool status = false ;
		beginWifi ( ssid , password , accessPoint , ip ) ;

		if ( ( localUdpPort != 0 ) && ( localTcpPort != 0 ) ) {
			status = beginUdpAndTcp ( localUdpPort , localTcpPort ) ;
		}
		else if ( localUdpPort != 0 ) {
			status = beginUdp ( localUdpPort ) ;
		}
		else if ( localTcpPort != 0 ) {
			status = beginTcp ( localTcpPort ) ;
		}
		if ( localTelnetPort != 0 ) {
			beginTelnet ( serialLineRate , localTelnetPort ) ;
		}
		return status ;
	}
} ;

#endif /* TARGET_TRACKER_ESP8266_H */

