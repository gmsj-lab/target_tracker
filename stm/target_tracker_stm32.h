#ifndef TARGET_TRACKER_STM32_H
#define TARGET_TRACKER_STM32_H
/*
 *  Target Tracker stm32
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
#define BOARD_TYPE 	STM_32

// Tracker flavour specific includes
#include "usart_driver.h"
#include "extern_calls.h"
#include "tracker_serial_comm.h"

// Base class
#include "_tracker.h"

// External "C" calls declaration
#ifdef __cplusplus
 extern "C" {
#endif
extern void softwareReprogrammingReq  	( void ) ;
extern void softwareRebootReq 			( void ) ;
#ifdef __cplusplus
}
#endif
// External "C++" calls declaration
extern void delay ( long delay ) ;

//---------------------------------------------------------------------------
//                              STM_32 Tracker class
//---------------------------------------------------------------------------
class TargetTracker : public _TargetTracker
{
private:

	TrackerSerialComm 	uartComm1 ;
	TrackerSerialComm 	uartComm2 ;

	// Constructor private. non-copyable, non-assignable.
	TargetTracker ( Tracker & ) ;
	TargetTracker & operator = ( const Tracker & ) ;

public:

	TargetTracker ( const char * targetName , bool timeTagging = true ) : _TargetTracker ( targetName , timeTagging = true ) {}
	virtual ~TargetTracker () {}

	// Start Tracker on one or two UART
	bool begin ( const int uart1Number , const int uart2Number = 0 ) {
		if ( uart1Number != 0 ) {
			uartComm1.setConfig ( uart1Number ) ;
			Tracker::getTracker ().setCommunicationMedia1 ( & uartComm1 ) ;
		}
		if ( uart2Number != 0 ) {
			uartComm2.setConfig ( uart2Number ) ;
			Tracker::getTracker ().setCommunicationMedia2 ( & uartComm2 ) ;
		}
		return Tracker::getTracker ().begin () ;
	}
} ;

#endif /* TARGET_TRACKER_STM32_H */

