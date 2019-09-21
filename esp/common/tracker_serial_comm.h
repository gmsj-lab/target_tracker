/*
 * TrackerSerialComm.h
 *
 *  Created on: 20 mars 2016
 *      Author: gilles
 */

#ifndef _TRACKERSERIALCOMM_IT_H_
#define _TRACKERSERIALCOMM_IT_H_

#define MAX_RX_LENGTH	4000

#include "tracker_communication.h"
#include "arduino.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"
#include "uart.h"
#include "uart_register.h"

extern size_t nbMaxRxSerial ;
class TrackerSerialComm: public TrackerCommunication
{
private:
	long 		serialRate ;

	inline void uart_config 				( void ) ;
	inline int 	readBytes 					( uint8_t * data , size_t length ) ;
	inline void flushBytes 					( void ) ;

public:
	static bool rx_overrun ;

	TrackerSerialComm () 										{ serialRate = 0 ; 								}
	virtual ~TrackerSerialComm () {}

	static int   getRxSerialBufferMax ( void ) 					{ return nbMaxRxSerial ;						}

	void 		 setConfig		  ( int serialRate )			{ this->serialRate = serialRate ; 				}
	bool  		 begin			  ( void ) ;
	int          ICACHE_RAM_ATTR receive 		  ( uint8_t * data , size_t length ) ;
	int 	 	 ICACHE_RAM_ATTR available		  ( void ) ;
	inline void  send 			  ( const uint8_t * data , size_t length ) 	{ Serial.write ( data, length ) ;	}
	inline void  send			  ( void ) 						{ 												}
	inline void  flush			  ( void ) 						{ 												}
	inline void  sendPacket		  ( const uint8_t * data , size_t length ) 	{ Serial.write ( data, length ) ; 	}
	inline void  endPacket		  ( void ) 						{ 												}
	inline int   availableForSend ( void )						{ return Serial.availableForWrite () ;			}
	inline void  print   		  ( const char * data ) 		{ Serial.print ( data ) ;   					}
	inline void  println 		  ( const char * data ) 		{ Serial.println ( data ) ; 					}

	int     	 getSendBuffer    ( char ** buf , int requestedSize ) { return 0 ; 								} // Optimized API
	void    	 send 			  ( int length )				{ 												} // Optimized API

	int    		 getReceiveBuffer ( char ** data )				{ return 0 ; 									} // Optimized API
	void 		 receive 		  ( int length ) 				{ 												} // Optimized API
};

#endif /* _TRACKERSERIALCOMM_IT_H_ */
