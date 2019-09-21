#ifndef _TRACKERUDPCOMM_H_
#define _TRACKERUDPCOMM_H_

#include <arduino.h>
#include <WiFiUdp.h>
#include "tracker_communication.h"
#include "utilities.h"

#undef  __CLASS__
#define __CLASS__	"tracker_udp_comm"

#define UDP_RX_PACKET_MAX_SIZE    1000
#define UDP_TX_PACKET_SEND_SIZE   1000  // wait for this size to build bigger packets...
#define UDP_PACKET_WAITING_TIME      8	// but do not wait for more than this time in ms
#define UDP_OK                       1
#define NO_TX_PACKET_IN_PROGRESS    -1

class TrackerUdpComm : public TrackerCommunication
{
private:
	int 		port ;
	WiFiUDP     udp ;
	IPAddress	hostAddress ;
	uint16_t    hostPort ;
	bool        connected ;
	bool        trace ;
	long        packetStartTime ;
	int         nbBytesTx ;
	uint16_t    nbBytesRx ;
	int			txPacketSequenceNum ;
	uint8_t 	txBuffer [ UDP_TX_PACKET_SEND_SIZE ] ;

	inline void ICACHE_RAM_ATTR listen ( const unsigned int localPort )
	{
		Serial1.print( "\n>udp.listen on port: "  ) ;Serial1.println(localPort);
		if ( udp.begin ( localPort ) != 1) {
			if (trace) Serial1.println( "\n>udp.begin : no sockets available to use"  ) ;
		}
	}
	inline size_t ICACHE_RAM_ATTR buildSeqNumMsg ( char *buf , int seqNum ) {
		size_t size = 0 ;

		buf [ 0 ] = '[' ;
		itoa ( seqNum , & buf [ 1 ], 10 ) ;
		size = strlen ( buf ) ;
		buf [ size ++ ] = ']' ;

		return size ;
	}
	inline void ICACHE_RAM_ATTR beginTxPacket ( void )
	{
		char seqNumMsg [ 20 ] ;
		if ( nbBytesTx == NO_TX_PACKET_IN_PROGRESS ) {
			udp.beginPacket ( hostAddress , hostPort ) ;
			packetStartTime = millis () ;
			size_t seqNumMsgLength = buildSeqNumMsg ( seqNumMsg , txPacketSequenceNum ++ ) ;
			nbBytesTx = udp.write( (const uint8_t *) seqNumMsg , seqNumMsgLength ) ;
		}
	}
public:
	TrackerUdpComm ()
	{
		port				= 0 ;
		connected   		= false ;
		trace 				= false ;
		hostPort			= 0 ;
		packetStartTime  	= 0 ;
		nbBytesTx   		= NO_TX_PACKET_IN_PROGRESS ;
		nbBytesRx			= 0 ;
		txPacketSequenceNum = 0 ;
	}
	virtual ~TrackerUdpComm () {} ;

	void setConfig ( int port ) {
		this->port = port ;
	}
	inline bool ICACHE_RAM_ATTR begin () {
		this->trace = false ;
		listen ( port ) ;
		return true ;
	}

	inline int ICACHE_RAM_ATTR available ( void )
	{
		if ( nbBytesRx == 0 )
		{
			nbBytesRx = udp.parsePacket () ;
			if ( nbBytesRx > 0 ) {
			   hostAddress = udp.remoteIP () ;
			   hostPort    = udp.remotePort () ;
			   if ( !connected ) {
				   if (trace) Serial1.println ( "\n>Connected to host addr: "+ hostAddress.toString() + ", port " + hostPort + "nb:" + nbBytesRx ) ;
				   connected = true ;
			   }
		   }
		}
		else
		{
			nbBytesRx = udp.available () ;
		}
		return nbBytesRx ;
	}

	inline int ICACHE_RAM_ATTR receive ( uint8_t * data , size_t maxLength )
	{
		size_t length = 0 ;
		if ( available () ) {
			length   = udp.read ( data, maxLength ) ;
			nbBytesRx = ( length < nbBytesRx ) ? nbBytesRx - length : 0 ;
		}
		return length ;
	}
	inline int ICACHE_RAM_ATTR availableForSend ( void )
	{
		int nbBytes = 0 ;
		if ( connected ) {
			nbBytes = UDP_TX_PACKET_MAX_SIZE ;
		}
		return nbBytes ;
	}

	// use sendPacket() to send right away
	inline void ICACHE_RAM_ATTR sendPacket ( const uint8_t * data , size_t length )
	{
		send ( data , length ) ;
		endPacket () ;
	}
	// if send (data,length) is used, make sure to call "endPacket (void) " or "send (void)"
	inline void ICACHE_RAM_ATTR send ( const uint8_t * data , size_t length )
	{
		if ( connected ) {
			beginTxPacket () ;
			if ( udp.write ( data , length ) != length ) {
				TRACE_PARAM( " Write failed !!!!!!!" , length ) ;
			}
			else {
				nbBytesTx += length ;
				send () ;
			}
		}
	}
	// check if time to flush...
	inline void ICACHE_RAM_ATTR send ( void )
	{
		if ( connected ) {
			if ( ( nbBytesTx > UDP_TX_PACKET_SEND_SIZE ) || ( ( millis () - packetStartTime ) > UDP_PACKET_WAITING_TIME ) ) {
				endPacket () ;
			}
		}
	}
	inline void endPacket ( void )
	{
		if ( nbBytesTx > 0 ) {
			if ( udp.endPacket () == 0 ) {
				TRACE( " failed !!!!!!!" ) ;
			}
			nbBytesTx = NO_TX_PACKET_IN_PROGRESS ;
		}
	}
	inline void print ( const char * data )
	{
		if ( connected ) {
			beginTxPacket () ;
			udp.write ( data ) ;
			endPacket () ;
		}
	}
	inline void println ( const char * data )
	{
		if ( connected ) {
			beginTxPacket () ;
			udp.write ( data ) ;
			udp.write ( "\n" ) ;
			endPacket() ;
		}
	}
	int getSendBuffer    ( char ** buf , int requestedSize ) { // Optimized API
		* buf = (char *) txBuffer ;
		return MIN ( requestedSize, UDP_TX_PACKET_SEND_SIZE ) ;
	}
	void send ( int length ) {	// Optimized API
		send ( txBuffer , length ) ;
	}
	int getReceiveBuffer ( char ** data ) {	// Optimized API
		static uint8_t	rxBuffer [ UDP_RX_PACKET_MAX_SIZE ] ;
		int length = receive ( rxBuffer , UDP_RX_PACKET_MAX_SIZE ) ;

		* data = (char *) rxBuffer ;
		return length ;
	}
	void receive ( int length ) { // Optimized API
		// Nothing to do.
	}

} ;
#undef  __CLASS__
#endif /* _TRACKERUDPCOMM_H_ */
