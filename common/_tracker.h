/*
 * _tracker.h
 *
 *  Created on: 8 sept. 2019
 *      Author: gilles
 */

#ifndef TARGET_TRACKER_COMMON__TRACKER_H_
#define TARGET_TRACKER_COMMON__TRACKER_H_

#include "tracker.h"

class _TargetTracker
{
private:
	// Constructor private. non-copyable, non-assignable.
	_TargetTracker ( Tracker & ) ;
	_TargetTracker & operator = ( const Tracker & ) ;
public:
	// Create the tracker (singleton)
	_TargetTracker ( const char * targetName , bool timeTagging )
		{
		Tracker & tracker = Tracker::getTracker () ;
		tracker.setName 		( targetName ) ;
		tracker.setTimeTagging 	( timeTagging ) ;
		}

	virtual ~_TargetTracker () {}

	virtual bool process 		( void ) 					{ return Tracker::getTracker ().process () ; 				}
	void 		 setTimeTagging ( const bool timeTagging )  { Tracker::getTracker ().setTimeTagging ( timeTagging ) ; 	}
	void  		 print 			( const char * data )		{ Tracker::getTracker ().print ( data ) ;					}
	void  		 println 		( const char * data )		{ Tracker::getTracker ().println ( data ) ;					}
} ;

#endif /* TARGET_TRACKER_COMMON__TRACKER_H_ */
