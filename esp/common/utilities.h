/*
 * Utilities.h
 *
 *  Created on: 24 sept. 2016
 *      Author: gmsj
 */

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include "utils.h"
#include "Arduino.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"

#define __CLASS__			 strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define TRACE(x)			( {if (trace) { Serial1.print( __CLASS__) ;  Serial1.print( "::" ); Serial1.print( __FUNCTION__) ;  Serial1.println( x ) ; } } )
#define TRACE_PARAM(x,y)	( {if (trace) { Serial1.print( __CLASS__) ;  Serial1.print( "::" ); Serial1.print( __FUNCTION__) ;  Serial1.print( ": " ) ; Serial1.print( x ) ; Serial1.println( y ) ; } } )
#define TRACE_PARAM_(x,y)	( {if (trace) { Serial1.print( __CLASS__) ;  Serial1.print( "::" ); Serial1.print( __FUNCTION__) ;  Serial1.print( ": " ) ; Serial1.print( x ) ; Serial1.print( y ) ; } } )
#define MIN(x,y)	   		(( (x) < (y) ) ? (x) : (y) )



class Utilities {
public:
#if US_TIMER_UTILITIES
#define MICROSEC_UNIT	   0
	static ETSTimer 	MyHwTimer ;

	static void timerInterrupt ( void * ) {};
	static void initTimer ( void ) {
	  //system_timer_reinit () ;
		ets_timer_setfn ( & MyHwTimer, Utilities::timerInterrupt, NULL ) ;
		ets_timer_arm_new ( & MyHwTimer, 200, 1, MICROSEC_UNIT ) ;
	}
#endif
	static void ledSwitch () {
		static int toggle = LOW ;
		pinMode 	 ( LED_BUILTIN, OUTPUT ) ;
		digitalWrite ( LED_BUILTIN, toggle ) ;
		toggle = ( toggle == HIGH ) ? LOW : HIGH ;
	}
	static void blinkLed ( uint8_t howMany , uint16_t howLong ) {

		pinMode ( LED_BUILTIN, OUTPUT ) ;
		for ( int n = 0 ; n < howMany ; n ++  ) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(howLong);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(howLong);
		}
		digitalWrite(LED_BUILTIN, HIGH);
	}
};

#endif /* _UTILITIES_H_ */
