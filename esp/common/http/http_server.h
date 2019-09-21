/*
 * HttpServer.h
 *
 *  Created on: 27 nov. 2016
 *      Author: gilles
 */

#ifndef _HTTP_HTTPSERVER_H_
#define _HTTP_HTTPSERVER_H_

#include "../utilities.h"
#include "ESP8266WiFi.h"

#define HTTP_PORT	80

class HttpServer {
private:
	WiFiServer server ;
	WiFiClient client ;
	void welcome 			( void ) ;
	bool clientAvailable 	( void ) ;
public:
	virtual ~HttpServer () {} ;
	HttpServer () ;

	void begin 				( void ) ;
	bool process			( void ) ;
} ;

#endif /* _HTTP_HTTPSERVER_H_ */
