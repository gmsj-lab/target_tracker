/*
* Tracker.h
*
*  Created on: 18 mars 2016
*      Author: gilles
*/
#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "utils.h"
#include "tracker_communication.h"
#include "tracker_complement.h"
#include "dynamic_array.h"
#include "fifo.h"

#define HOST_MSG_SIZE_MAX	200

class TargetAttribute ;

class Group
{
public:
	uint8_t 	 id ;
	const char * name ;
};

typedef DynamicArray < TargetAttribute * > AttributesArray ;
typedef DynamicArray < Group 			 > GroupArray ;
typedef	Fifo 		 < uint8_t 			 > ChangedIdFifo ;

class Tracker
{

private:
	static Tracker *		trackerInstance ;
	TrackerCommunication *	comm ;
	TrackerCommunication *	comm1 ;
	TrackerCommunication *	comm2 ;
	TrackerComplement	 *	complement ;
	const char * 			name ;
	uint8_t 				nbOfGroups ;
	uint8_t 				nbOfAttributes ;
	bool 					attributeListChanged ;
	GroupArray 				groups ;
	AttributesArray 		attributes ;
	ChangedIdFifo 			changedIdFifo ;
	uint32_t				timeTag ;
	bool 					timeTagging ;
	unsigned int 			currentLength ;
	unsigned long 			nbUpdateMsgSent ;
    char                    rxBuf [ HOST_MSG_SIZE_MAX ] ;

	inline void 	processHostCommands		( void ) ;
	inline void 	parseCommand			( char command [] , int length ) ;
	inline void 	parseUpdateMessage  	( char command [] , int length ) ;
	inline void 	processTelnetMessage	( char message [] , unsigned int length ) ;
	inline void 	sendDiscoverMessage		( void ) ;
	inline void  	getDiscoverMessage		( char buf [] ) ;
	inline uint8_t 	getAttributeString		( TargetAttribute & targetAttribute, char buf [] ) ;
	inline uint8_t  insertTimeTag			( char buf[] , uint32_t timeTag ) ;
	inline uint8_t  insertSequenceNumber	( char buf[] , uint32_t timeTag ) ;
	inline void 	sendUpdateMsg			( void ) ;
	inline TargetAttribute & attribute		( const uint8_t id ) { return ( * attributes [ id ] ) ; } ;

	// non-copyable, non-assignable.
	Tracker ( Tracker & ) ;
	Tracker & operator = ( const Tracker & ) ;
	Tracker () ;

public:
	virtual ~Tracker () ;

	static Tracker & getTracker 			( void )  ;
	inline void  	 setAttributeIdChanged	( const uint8_t id ) 	{ changedIdFifo.push ( id ) ; } ;
	void 		 	 setCommunicationMedia1	( TrackerCommunication * comm ) ;
	void 		 	 setCommunicationMedia2 ( TrackerCommunication * comm ) ;
	void 		 	 setComplement			( TrackerComplement * complement ) ;
	void 		 	 setName				( const char * name ) ;
	void 		 	 setTimeTagging			( const bool timeTagging ) ;
	void 		 	 addAttribute			( TargetAttribute & attribute ) ;
	bool 			 process 				( void ) ;
	bool 			 begin					( void ) ;
	inline void  	 print   				( const char * data ) 	{ comm->print ( data ) ; 	}
	inline void 	 println 		  		( const char * data ) 	{ comm->println ( data ) ; 	}
	void 			 softwareReboot			( void ) ;
	void 			 sendSoftwareReprogAck 	( void ) ;
};

#endif /* _TRACKER_H_ */
