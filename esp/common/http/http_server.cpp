/*
 * HttpServer.cpp
 *
 *  Created on: 27 nov. 2016
 *      Author: gilles
 */

#include "http_server.h"

HttpServer::HttpServer () : server ( HTTP_PORT ) {
}

void HttpServer::begin ( void ) {
	server.begin () ;
	welcome () ;
}
void HttpServer::welcome ( void ) {
	Serial1.print ( "Use this URL to connect: " ) ;
	Serial1.print ( "http://" ) ;
	Serial1.print ( WiFi.softAPIP () ) ;
	Serial1.println ( "/" ) ;
}
bool HttpServer::clientAvailable ( void ) {
	// Check if a client has connected
	client = server.available () ;
	return ( client ) ;
}
bool HttpServer::process ( void ) {

	if ( clientAvailable () && client.available () ) {
		// Read the first line of the request
		String request = client.readStringUntil ( '\r' ) ;
		Serial.println ( request ) ;
		client.flush () ;

		// Match the request
		int value = LOW;

		if ( request.indexOf ( " /LED=ON" ) != -1 ) {
			Utilities::blinkLed ( 1 , 100 ) ;
			value = HIGH ;
		}

		if ( request.indexOf ( " /LED=OFF" ) != -1 ) {
			Utilities::blinkLed ( 2 , 100 ) ;
			value = LOW ;
		}

		// Return the response
		client.println ( "HTTP/1.1 200 OK" ) ;
		client.println ( "Content-Type: text/html" ) ;
		client.println ( "" ) ; //  do not forget this one
		client.println ( "<!DOCTYPE HTML>" ) ;
		client.println ( "<html>" ) ;

		client.print("Led pin is now: " ) ;

		if ( value == HIGH ) {
			client.print ( "On" ) ;
		} else {
			client.print ( "Off" ) ;
		}
		client.println ( "<br><br>" ) ;
		client.println ( "Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 ON<br>" ) ;
		client.println ( "Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 2 OFF<br>" ) ;
		client.println ( "</html>" ) ;

	}
	return true ;
}
