/*
 * TrackerSerialComm.h
 *
 *  Created on: 20 mars 2016
 *      Author: gmsj
 */

#ifndef _TRACKER_SERIALCOMM_H_
#define _TRACKER_SERIALCOMM_H_

#ifdef ARDUINO_PLATFORM

#include "Tracker_communication.h"

class TrackerSerialComm : public TrackerCommunication
{
public:
	TrackerSerialComm () {}
	virtual ~TrackerSerialComm () {}

	inline bool  begin			  ( long serialLineRate ) 		{ return ( Serial.begin( serialLineRate ) ) ;		}
	inline int   receive 		  ( char * data , int length ) 	{ return ( Serial.readBytes( data , length ) ) ;	}
	inline void  send 			  ( char * data , int length ) 	{ Serial.write ( (const uint8_t *)data, length ) ; 	}
	inline void  sendPacket		  ( char * data , int length ) 	{ Serial.write ( (const uint8_t *)data, length ) ; 	}
	inline void  endPacket		  ( void ) 						{ 												 	}
	inline 	int  availableForSend ( void )						{ return Serial.availableForWrite () ;			 	}
	inline int 	 available		  ( void ) 						{ return ( Serial.available () ) ;				 	}
	inline void  print   		  ( const char * data ) 		{ Serial.print ( data ) ;   					 	}
	inline void  println 		  ( const char * data ) 		{ Serial.println ( data ) ; 					 	}
};
#endif ARDUINO_PLATFORM
#endif /* _TRACKER_SERIALCOMM_H_ */
