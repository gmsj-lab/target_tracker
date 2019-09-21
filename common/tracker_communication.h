/*
 * TrackerCommunication.h
 *
 *  Created on: 14 mars 2016
 *      Author: gmsj
 */

#ifndef _TRACKERCOMMUNICATION_H_
#define _TRACKERCOMMUNICATION_H_

#include <stdint.h>
#include <stddef.h>

// Interface class
class TrackerCommunication {
public:
	TrackerCommunication () {};
	virtual ~TrackerCommunication () {} ;

	virtual void 	setConfig		 ( int configParam )	 					= 0 ;
	virtual bool 	begin			 ( void )				 					= 0 ;
	virtual int  	availableForSend ( void ) 									= 0 ;
//	virtual void    send    		 ( const uint8_t * data , size_t length )	= 0 ;
//	virtual void    sendPacket		 ( const uint8_t * data , size_t length )  	= 0 ;
	virtual int     getSendBuffer  	 ( char ** buf , int requestedSize )		= 0 ; // Optimized API
	virtual void    send 			 ( int length )				 				= 0 ; // Optimized API
	virtual void 	send			 ( void ) 									= 0 ; // flush send for udp
//	virtual void    endPacket		 ( void ) 									= 0 ;

	virtual int 	available		 ( void ) 									= 0 ;
	virtual int 	receive 		 ( uint8_t * data , size_t length ) 		= 0 ;
	virtual int    	getReceiveBuffer ( char ** data )			  				= 0 ; // Optimized API: the user should call receive(length) to free the buffer
	virtual void 	receive 		 ( int length ) 			  				= 0 ; // Optimized API

	virtual void 	print  			 ( const char * data ) 						= 0 ;
	virtual void 	println    		 ( const char * data ) 						= 0 ;
};

#endif /* _TRACKERCOMMUNICATION_H_ */
