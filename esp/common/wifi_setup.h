/*
 * WifiSetup.h
 *
 *  Created on: 9 avr. 2016
 *      Author: gmsj
 */

#ifndef _WIFISETUP_H_
#define _WIFISETUP_H_

#include "target_tracker.h"
#include "ESP8266WiFi.h"

#define __TRACE				false
#define PRINTLN_DEBUG(x)	if ( __TRACE ) Serial1.println( x )
#define PRINT_DEBUG(x)		if ( __TRACE ) Serial1.print( x )

const char * statusMsg [ ]= { "IDLE_STATUS" , "NO_SSID_AVAIL", "SCAN_COMPLETED", "CONNECTED" ,
							  "CONNECT_FAILED", "CONNECTION_LOST", "DISCONNECTED", "UNKNOWN CAUSE" } ;

class WifiSetup {
private:
	IPAddress 			localIPadress ;
	IPAddress 			gatewayIPadress ;
	IPAddress 			subnet ;
	ESP8266WiFiAPClass 	ap ;
public:
	WifiSetup () {
	} ;
	virtual ~WifiSetup () {} ;

	void connect ( const char * ssid , const char * password , bool accessPoint , const char * ip )
	{
		if ( accessPoint ) {
			wifiAccessPointSetup ( ssid , password , ip ) ;
		} else {
			connectSTA ( ssid , password ) ;
		}
		PRINTLN_DEBUG( "Connected" ) ;
	}
	void wifiAccessPointSetup ( const char * ssid , const char * password , const char * ip )
	{
		subnet.fromString ( "255,255,255,0" ) ;
		gatewayIPadress.fromString 	( ip ) ;
		localIPadress.fromString 	( ip ) ;


//		WiFi.setAutoConnect ( false ) ;
        WiFi.mode ( WIFI_AP ) ;
		if ( ! WiFi.softAP ( ssid, password, 11,  0 ) ) {
			PRINT_DEBUG ( "WifiSetup: Error setting up the wifi access point " ) ;
		}

		if ( ! ap.softAPConfig ( localIPadress, gatewayIPadress,  subnet ) ) {
			PRINT_DEBUG ( "WifiSetup: Error configuring IP @ for the wifi access point " ) ;
		}
		delay ( 1000 ) ;
		IPAddress myIP = WiFi.softAPIP () ;

			PRINT_DEBUG ("AP IP address: ") ;
			PRINTLN_DEBUG ( myIP ) ;
		WiFi.printDiag ( Serial1 ) ;

		PRINT_DEBUG ( "Access point started. SSID: " ) ;
		PRINTLN_DEBUG ( ssid ) ;
		printStatus () ;
		PRINTLN_DEBUG( "Connected to wifi in Access point mode" ) ;
	}

	void connectSTA ( const char * ssid , const char * password )
	{
		int status = WL_IDLE_STATUS;

		// attempt to connect to Wifi network:
		while ( status != WL_CONNECTED )
		{
			PRINT_DEBUG ( "Attempting to connect to SSID: " ) ;
			PRINTLN_DEBUG ( ssid ) ;
			// Connect to WPA/WPA2 network.
			status = WiFi.begin ( ssid , password ) ;
			if( status > 6 ) status = 7 ;
			// wait for connection:
			delay( 1000 ) ;
			PRINT_DEBUG( "Status :" ) ; PRINTLN_DEBUG( statusMsg [ status ] ) ; printStatus () ;
			WiFi.printDiag ( Serial1 ) ;
		}
		PRINTLN_DEBUG ( "Connected to wifi in station mode" ) ;
	}
	void printStatus ( void )
	{
	  PRINT_DEBUG ( "SSID: ") ; PRINTLN_DEBUG ( WiFi.SSID () ) ;

	  IPAddress ip = WiFi.localIP () ;
	  PRINT_DEBUG ( "IP Address: " ) ; PRINTLN_DEBUG ( ip ) ;

	  long rssi = WiFi.RSSI();
	  PRINT_DEBUG ( "signal strength (RSSI):" ) ; PRINT_DEBUG ( rssi ) ; PRINTLN_DEBUG ( " dBm" ) ;
	}
};

#endif /* _WIFISETUP_H_ */
