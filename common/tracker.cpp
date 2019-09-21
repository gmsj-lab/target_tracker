/* Tracker.cpp
 *  Created on: 18 mars 2016
 *      Author: gmsj
 */
//#include "util.h"
#include "tracker.h"

#include "target_attribute.h"
#include "extern_calls.h"

#ifndef Arduino_h
extern uint32_t micros ( void ) ;
#endif

#define _EOF						'\0'
#define SOM							'@'
#define EOM							'#'
#define DISCOVERY_MSG				'D'
#define UPDATE_MSG					'U'
#define SOFTWARE_REBOOT_MSG			'W'
#define SOFTWARE_REPROG_MSG			'Z'
#define SOFTWARE_REPROG_ACK			'Z'
#define TARGET_NAME_TERMINATOR		'$'
#define TIMETAG_SEPARATOR			'T'
#define SEQNUM_SEPARATOR			'Q'
#define ATTRIBUTE_SEPARATOR			'*'
#define SUB_ATTRIBUTE_SEPARATOR		':'
#define VALUE_SEPARATOR				'&'
#define TYPE_SEPARATOR				'%'
#define ACCES_READ					'R'
#define DECIMAL_BASE				 10
#define ATTRIBUTE_SIZE_MAX 			200
#define DISCOVERY_MSG_SIZE			1300
#define GROUP_ID_BASE 				 80

Tracker *Tracker::trackerInstance = NULL ;

Tracker::~Tracker () {}
Tracker::Tracker ()
{
	this->complement			= NULL ;
	this->comm					= NULL ;
	this->comm1					= NULL ;
	this->comm2					= NULL ;
	this->name 					= name ;
	this->timeTagging 			= true ;
	this->timeTag				= 0 ;
	this->attributeListChanged  = false ;
	this->nbOfAttributes 		= 0 ;
	this->nbOfGroups	 		= 0 ;
	this->currentLength 		= 0 ;
	this->nbUpdateMsgSent		= 0 ;
}
Tracker & Tracker::getTracker ( void )
{
	if ( Tracker::trackerInstance == NULL )
		Tracker::trackerInstance = new Tracker ;
	return * Tracker::trackerInstance ;
} ;
void Tracker::setCommunicationMedia1( TrackerCommunication * comm )  	{ this->comm1 = comm ; 			 	}
void Tracker::setCommunicationMedia2( TrackerCommunication * comm )  	{ this->comm2 = comm ; 				}
void Tracker::setComplement			( TrackerComplement * complement )	{ this->complement = complement ;	}
void Tracker::setName 				( const char * name ) 				{ this->name = name ; 				}
void Tracker::setTimeTagging 		( const bool timeTagging  ) 		{ this->timeTagging = timeTagging ; }

// Called to register an attribute
void Tracker::addAttribute ( TargetAttribute & targetAttribute )
{
	// Does this attribute have a group ?
	if ( strcmp ( targetAttribute.getGroupName (), NO_GROUP ) != 0 )
	{
		// Is the group already known ?
		bool groupFound = false ;
		for ( uint8_t groupId = 0 ; groupId < nbOfGroups ; groupId ++ )
		{
			if ( strcmp ( targetAttribute.getGroupName (), groups [ groupId ].name ) == 0 )
			{
				groupFound = true ;
			}
		}
		// If new group, create and add the attribute group
		if ( ! groupFound )
		{
			groups [ nbOfGroups ].id   = GROUP_ID_BASE + nbOfGroups ;
			groups [ nbOfGroups ].name = targetAttribute.getGroupName () ;
			nbOfGroups ++ ;
		}
	}
	// give this attribute a unique id
	uint8_t id = nbOfAttributes ++ ;

	// Store it in the attribute array
	attributes [ id ] = & targetAttribute ;

	// Remember that we have to let the Host know about this new attribute
	attributeListChanged = true ;

	// Give the attribute its id
	targetAttribute.setId ( id ) ;
}
bool Tracker::begin ()
{
	bool status = false ;
	if ( comm1 != NULL ) {
		comm = comm1 ;
		status = comm->begin () ;
		if ( comm2 != NULL )
			status = status && comm2->begin () ;
	}
	return status ;
}

// Called regularly to perform TargetTracker activities
bool Tracker::process ( void )
{
	if ( complement != NULL ) {
		complement->process () ;
		}
	if ( comm1->available () ) {
		comm = comm1 ;
		processHostCommands () ;
		}
	if ( comm2 && comm2->available () )  {
		comm = comm2 ;
		processHostCommands () ;
		}
	if ( attributeListChanged ) {
		sendDiscoverMessage () ;
		}
	if ( ! changedIdFifo.isEmpty () ) {
		sendUpdateMsg () ;
		}
	comm->send () ;
	return ( ! changedIdFifo.isEmpty () ) ;
}
void Tracker::processHostCommands ( void )
{
	static bool msgInProgress = false ;
	char *rxPtr ;
	int nbRxBytes = comm->getReceiveBuffer ( &rxPtr ) ;

	for ( int i = 0 ; i < nbRxBytes ; i ++ ) {
		if ( rxPtr[ i ] == SOM ) {
			// we got a SOM, forget everything (including the SOM)
			msgInProgress = true ;
			currentLength = 0 ;
		}
		else {
			if ( msgInProgress == true ) {
				// Message under reception add this char
				rxBuf [ currentLength ++ ] = rxPtr[ i ] ;
				if ( rxPtr[ i ] == EOM ) {
					// We got a complete message, process it
					parseCommand ( rxBuf , currentLength ) ;
					currentLength = 0 ;
					msgInProgress = false ;
				}
			}
		}
	}
	// tell to forget what we have processed
	if ( nbRxBytes ) {
		comm->receive ( nbRxBytes ) ;
	}
}

void Tracker::parseCommand ( char command [] , int length )
{
	switch ( command [ 0 ] )
	{
	case UPDATE_MSG :
		parseUpdateMessage ( & command [1], length - 1 ) ;
		break ;
	case DISCOVERY_MSG :
		println ( "DISCOVERY_MSG" ) ;
		sendDiscoverMessage () ;
		break ;
	case SOFTWARE_REPROG_MSG :
		println ( "SOFTWARE_REPROG_MSG" ) ;
		delay ( 1000 ) ;
		softwareReprogrammingReq () ;
		break ;
	case SOFTWARE_REBOOT_MSG :
		println ( "SOFTWARE_REBOOT_MSG" ) ;
		softwareReboot () ;
		break ;
	default :
		print( "UKNOWN MSG RECEIVED:" ) ;
		println( command ) ;
		break ;
	}
}
void Tracker::parseUpdateMessage ( char command [] , int length )
{
	char * token = command ;
	uint8_t id ;
	int i = 0 ;

	while ( i < length ) {
		// Get the id
		while ( command [i] != VALUE_SEPARATOR ) {
			if ( i >= length ) {
				print( "parseUpdateMessage: MALFORMED UPDATE MSG RECEIVED : attribute id" ) ;
				printf( "<length:%d><i:%d><i:%s>",length,i,command ) ;
				return ;
			}
			i ++ ;
		}
		command [i++] = _EOF ;
		id = atoi ( token ) ;
		token = &command [ i ] ;

		// Get the value
		while ( ! ( ( command [i] == ATTRIBUTE_SEPARATOR ) || ( command [i] == EOM ) ) ) {
			if ( i >= length ) {
				print( "parseUpdateMessage: MALFORMED UPDATE MSG RECEIVED : attribute value" ) ;
				printf( "<length:%d><i:%d><command:%s>",length,i,command ) ;
				return ;
			}
			i ++ ;
		}
		command [i++] = _EOF ;
		if ( id < nbOfAttributes )
		{
			attribute( id ).setStringValue( token ) ;
		}
		token = &command [ i ] ;
	}
}
void Tracker::sendDiscoverMessage ( void )
{
	char * buf ;
	if ( comm->getSendBuffer ( &buf , DISCOVERY_MSG_SIZE ) < DISCOVERY_MSG_SIZE ) {
//		println ( "Error: not enough space for Discover Message transmission" ) ;
		app_error ( MAIN_ERROR ) ;
	}
	else {
		getDiscoverMessage ( buf ) ;
		// prevent from sending a second discovery msg for nothing
		attributeListChanged = false ;
	}
}
void Tracker::getDiscoverMessage ( char buf [] )
{
	int size = 0 ;
	buf[ size ++ ] = SOM ;
	buf[ size ++ ] = DISCOVERY_MSG ;

	strcpy ( &buf [ size ] , name );
	size += strlen ( name ) ;
	buf [ size ++ ] = TARGET_NAME_TERMINATOR ;

	//insert time tag if requested
	if ( timeTagging )
	{
		timeTag = 0 ;
		size += insertTimeTag ( & buf [ size ] , timeTag ) ;
	}
	else {
		nbUpdateMsgSent = 0 ;
		size += insertSequenceNumber ( & buf[ size ] , nbUpdateMsgSent ++ ) ;
	}
	// Add all groups
	for ( uint8_t groupId = 0 ; groupId < nbOfGroups ; groupId ++ )
	{
		// Copy the group Id
		itoa( groups [ groupId ].id , & buf [ size ] , DECIMAL_BASE ) ;
		size += strlen( & buf [ size ] ) ;

		// Copy the group Name
		strcpy ( & buf [ size ] , groups [ groupId ].name ) ;
		size += strlen( groups [ groupId ].name ) ;

		// Add all sub-attribute of this group to the discover message
		for ( uint8_t id = 0 ; id < nbOfAttributes ; id ++ )
		{
			if ( strcmp( attribute( id ).getGroupName(), groups [ groupId ].name ) == 0 )
			{
				buf [ size ++ ] = SUB_ATTRIBUTE_SEPARATOR ;
				// Add the sub-attribute to the discover message
				size += getAttributeString ( attribute( id ) , & buf [ size ] ) ;

				// Flush and get a fresh buffer
//				comm->send( size ) ;
//				size = 0 ;
//				while ( comm->getSendBuffer ( &buf , DISCOVERY_MSG_SIZE ) < DISCOVERY_MSG_SIZE ) ;
			}
		}
		buf[ size ++ ] = ATTRIBUTE_SEPARATOR ;
	}

	// Add all attributes which does not have groups
	for ( uint8_t id = 0 ; id < nbOfAttributes ; id ++ )
	{

		if ( strcmp( attribute( id ).getGroupName(), NO_GROUP ) == 0 )
		{
			// Add the attribute to the discover message
			size += getAttributeString ( attribute( id ) , & buf [ size ] ) ;

			// Flush and get a fresh buffer
//			comm->send( size ) ;
//			size = 0 ;
//			while ( comm->getSendBuffer ( &buf , DISCOVERY_MSG_SIZE ) < DISCOVERY_MSG_SIZE ) ;

			buf[ size ++ ] = ATTRIBUTE_SEPARATOR ;
		}
	}

	// remove last ATTRIBUTE_SEPARATOR (or empty slot if no attributes)
	buf[ size - 1 ] = EOM ;
	// put a string terminator AFTER the end of the message so we can print it as a string..
	buf[ size ] = _EOF ;

	if ( size >= DISCOVERY_MSG_SIZE ) {
		for(;;);
	}
	comm->send ( size ) ;
}
// Attribute coded as transmitted in Discovery messages
uint8_t Tracker::getAttributeString ( TargetAttribute & attribute, char buf [] )
{
	uint8_t size ;

	// Convert the id to ascii and copy it
	itoa ( attribute.getId(), buf, DECIMAL_BASE ) ;
	size = strlen( buf ) ;

	// Copy the name
	const char *name = attribute.getName() ;
	strncpy ( & buf[ size ], name , strlen ( name ) );
	size += strlen ( name ) ;

	// Copy the type
	buf[ size ++ ] = TYPE_SEPARATOR ;
	buf[ size ++ ] = attribute.getType() ;

	// Copy the acces rights, if not default value
	if ( attribute.isReadOnly() )
	{
		buf[ size ++ ] = ACCES_READ ;
	}
	// Convert the value to ascii and copy it
	buf[ size ++ ] = VALUE_SEPARATOR ;
	size += attribute.getStringValue ( & buf [ size ] ) ;

	return ( size ) ;
}

// Notify Host that an attribute has changed its value
void Tracker::sendUpdateMsg ( void )
{
	bool done = false ;
	char * buf ;
	uint8_t id ;
	uint16_t size = 0 ;
	uint16_t validatedSize = 0 ;

	int maxSize = comm->getSendBuffer ( &buf , ATTRIBUTE_SIZE_MAX ) ;

	// insert header of the UPDATE_MSG and time tag if requested
	buf[ size ++ ] = SOM ;
	buf[ size ++ ] = UPDATE_MSG ;

	if ( timeTagging ) {
		uint32_t currentTime = micros () ;
		size += insertTimeTag ( & buf[ size ] , currentTime - timeTag ) ;
		timeTag = currentTime ;
	}
	else {
		size += insertSequenceNumber ( & buf[ size ] , nbUpdateMsgSent ++ ) ;
	}

	// While there is something to notify and we have room in msg
	while ( ( ! changedIdFifo.isEmpty () ) && ( ! done ) )
	{
		if ( validatedSize != 0 ) buf[ size++ ] = ATTRIBUTE_SEPARATOR ;

		id = changedIdFifo.pop () ;

		// Convert the id to ascii and write it
		itoa ( id, & buf [ size ], DECIMAL_BASE ) ;
		size += strlen ( & buf [ size ] ) ;

		buf[ size ++ ] = VALUE_SEPARATOR ;

		// add the attribute to the update message
		size += attribute ( id ).getStringValue ( & buf [ size ] ) ;

		// make sure we respect the max size, counting for terminaison character
		if ( ( size + 1 ) >= maxSize )
		{
			// too much ! reject last attribute
			changedIdFifo.pushOnTop ( id ) ;
			done = true ;
		}
		else
		{
			// Show that we took care of this attribute
			attribute ( id ).resetDirty () ;
			// Show that it fits in available driver buffers
			validatedSize = size ;
		}
	}
	// terminate the message ?
	if ( validatedSize != 0 )
	{
		buf [ validatedSize ++ ] = EOM ;
		comm->send ( validatedSize ) ;
	}
}
uint8_t Tracker::insertTimeTag ( char buf[] , uint32_t timeTagDelta )
{
	uint8_t size = sprintf ( buf , "%lu" , ( unsigned long int)timeTagDelta ) ;
	buf[ size ++ ] = TIMETAG_SEPARATOR;
	return size ;
}
uint8_t Tracker::insertSequenceNumber ( char buf[] , uint32_t sequenceNum )
{
	uint8_t size = sprintf ( buf , "%lu" , ( unsigned long int)sequenceNum ) ;
	buf[ size ++ ] = SEQNUM_SEPARATOR;
	return size ;
}
void Tracker::sendSoftwareReprogAck () {
	char * buf ;
	char ackMsg[] 		= { SOM , SOFTWARE_REPROG_ACK , EOM , _EOF } ;
	uint8_t ackMsgSize 	= strlen ( ackMsg ) ;

	while ( comm->getSendBuffer ( &buf , ackMsgSize ) != ackMsgSize ) {
		delay ( 1 ) ;
	}
	memcpy ( buf , ackMsg , ackMsgSize ) ;
	comm->send ( ackMsgSize ) ;
}
// softwareReboot : 	<SOM><SOFTWARE_REBOOT_MSG><EOM>
void Tracker::softwareReboot ()
{
	softwareRebootReq () ;
}
