/*
 * TrackerSerialComm.h
 *
 *  Created on: 20 mars 2016
 *      Author: gmsj
 */

#ifndef _TRACKERSERIALCOMM_H_
#define _TRACKERSERIALCOMM_H_

#include <stdio.h>
#include "tracker_communication.h"
#include "usart_driver.h"

class TrackerSerialComm : public TrackerCommunication
{
private:
	UsartDriver usartDriver ;
	long uart ;
public:
	TrackerSerialComm () { uart = 0 ; }
	virtual ~TrackerSerialComm () {}

	virtual void   setConfig		( int uart )							{ this->uart = uart ; 												}
	virtual bool   begin			( void ) 		  						{ return ( usartDriver.init ( uart ) ) ;							}
	virtual int    availableForSend	( void ) 					  			{ return usartDriver.availableForSend () ; 							}
	virtual int    getSendBuffer  	( char ** buf , int requestedSize )		{ return usartDriver.getSendBuffer ( buf , requestedSize ) ;	 	}
	virtual void   send				( void ) 				  				{ 									 								}
	virtual void   send				( int length ) 				  			{ usartDriver.send ( length ) ;	 									}
	virtual void   send       		( const uint8_t * data , size_t length ){ usartDriver.send ( (char*) data , length ) ;						}
	virtual void   sendPacket  		( const uint8_t * data , size_t length ){ usartDriver.send ( (char*) data , length ) ;						}

	virtual int    available		( void ) 					  			{ return usartDriver.available () ;						 			}
	virtual int    getReceiveBuffer	( char ** data  ) 			  			{ return usartDriver.getReceiveBuffer ( data , RX_BUFFER_SIZE ) ; 	}
	virtual void   receive 		  	( int length ) 				  			{ usartDriver.receive ( length ) ; 									}
	virtual int    receive 		 	( uint8_t * data , size_t length )		{ return usartDriver.receive ( (char*) data , length ) ; 			}
	virtual void   print   		  	( const char * data ) 		  			{ printf ( "%s", data ) ;	  	 									}
	virtual void   println 		  	( const char * data ) 		  			{ printf ( "%s\n", data ) ; 	 									}
	virtual void   endPacket		( void ) 					  			{ 																	}

};
#endif /* _TRACKERSERIALCOMM_H_ */
