/*
 * EspTcpDriver.h
 *
 *  Created on: 7 oct. 2016
 *      Author: gmsj
 */

#ifndef _ESPTCPDRIVER_H_
#define _ESPTCPDRIVER_H_

#include <stdint.h>
#include <stddef.h>
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "lwip/stats.h"
#include "espconn.h"

#define SERVER_TIMEOUT					7000 	// Max : 7200
#define NB_MAX_CLIENTS				   	   2

class TcpConnectionClient {
public:
	uint8_t 				clientNumber ;
	esp_tcp 				esp_tcp_conn ;
	struct espconn 			conn ;
	struct espconn_packet 	info ;

	virtual void connectCallback 	( void ) {} ;
	virtual void reconnectCallback 	( sint8 err ) {} ;
	virtual void disconnectCallback ( void ) {} ;
	virtual void sendCallback	 	( void ) {} ;
	virtual void writeCallback	 	( void ) {} ;
	virtual void receiveCallback 	( char *data , uint16_t length ) {} ;

	virtual ~TcpConnectionClient () {} ;
};

class EspTcpDriver {

private:
	static uint8_t 				nextClientNumber ;
	static TcpConnectionClient  *clientTable [ NB_MAX_CLIENTS ] ;

	inline static TcpConnectionClient * getClient ( espconn * conn ) {
		return ( TcpConnectionClient * ) conn->reverse ;
	}
	static void ICACHE_RAM_ATTR tcp_connect_callback ( void *arg ) {
		espconn * conn = (struct espconn *) arg ;

		if ( getClient ( conn ) ) {

			if ( espconn_set_opt ( conn , ESPCONN_NODELAY | ESPCONN_REUSEADDR | ESPCONN_KEEPALIVE ) != ESPCONN_OK ) { //ESPCONN_NODELAY | | ESPCONN_COPY
				// error
			}
			else {
				uint32_t keeplive ;

				//set keepalive: 75s = 60 + 5*3
				keeplive = 60 ;
				espconn_set_keepalive		( conn, ESPCONN_KEEPIDLE, &keeplive ) ;
				keeplive = 5 ;
				espconn_set_keepalive		( conn, ESPCONN_KEEPINTVL, &keeplive ) ;
				keeplive = 3 ; //try times
				espconn_set_keepalive		( conn, ESPCONN_KEEPCNT, &keeplive ) ;

				espconn_regist_recvcb		( conn , tcp_receive_callback ) ;
				espconn_regist_sentcb		( conn , tcp_send_callback ) ;
				espconn_regist_write_finish	( conn , tcp_write_callback ) ;

				getClient ( conn )->connectCallback () ;
			}
		}
	}
	static void ICACHE_RAM_ATTR tcp_disconnect_callback ( void *arg ) {
		espconn * conn = (struct espconn *) arg ;
		if ( getClient ( conn ) ) getClient ( conn )->disconnectCallback () ;
	}
	static void ICACHE_RAM_ATTR tcp_reconnect_callback ( void *arg , sint8 err ) {
		espconn * conn = (struct espconn *) arg ;
		if ( getClient ( conn ) ) getClient ( conn )->disconnectCallback () ;
	}
	static void ICACHE_RAM_ATTR tcp_receive_callback ( void *arg , char *data , unsigned short length ) {
		espconn * conn = (struct espconn *) arg ;
		if ( getClient ( conn ) ) getClient ( conn )->receiveCallback ( data , length ) ;
	}
	static void ICACHE_RAM_ATTR tcp_send_callback ( void *arg ) {
		espconn * conn = (struct espconn *) arg ;
		if ( getClient ( conn ) ) getClient ( conn )->sendCallback () ;
	}
	static void ICACHE_RAM_ATTR tcp_write_callback ( void *arg ) {
		espconn * conn = (struct espconn *) arg ;
		if ( getClient ( conn ) ) getClient ( conn )->writeCallback () ;
	}
public:

	static int ICACHE_RAM_ATTR begin ( long port, TcpConnectionClient * client ) {

		if ( nextClientNumber < NB_MAX_CLIENTS ) {
			clientTable [ nextClientNumber ]	= client ;

			memset ( & client->esp_tcp_conn , 0 , sizeof ( client->esp_tcp_conn ) ) ;

			client->clientNumber 				= nextClientNumber ++ ;
			client->conn.type 					= ESPCONN_TCP ;
			client->conn.state 					= ESPCONN_NONE ;
			client->conn.proto.tcp 				= & client->esp_tcp_conn ;
			client->conn.proto.tcp->local_port 	= port ;
			client->conn.reverse				= (void *) client ;

			if ( nextClientNumber == 1 ) espconn_tcp_set_max_con 	( NB_MAX_CLIENTS ) ;
			espconn_tcp_set_max_con_allow	( & client->conn , 1 ) ;

			espconn_regist_connectcb 	( & client->conn , tcp_connect_callback 	) ;
			espconn_regist_disconcb		( & client->conn , tcp_disconnect_callback 	) ;
			espconn_regist_reconcb   	( & client->conn , tcp_reconnect_callback 	) ;

			int status = espconn_accept ( & client->conn ) ;
			espconn_regist_time 	 	( & client->conn , SERVER_TIMEOUT , 0 ) ;

			return status ;
		}
		else {
			return ESPCONN_MAXNUM ;
		}
	}
} ;

#endif /* _ESPTCPDRIVER_H_ */
