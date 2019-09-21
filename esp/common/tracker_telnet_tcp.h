/*
 * TrackerTcpTelnet.h
 *  Created on: October 9th, 2016
 *      Author: GMSJ
 */
#ifndef _TRACKER_TELNET_TDP_H_
#define _TRACKER_TELNET_TDP_H_

#include "tracker_serial_comm.h"
#include "tracker_tcp_comm.h"
#include "tracker.h"

#undef  __CLASS__
#define __CLASS__	"tracker_telnet_tcp"

#define MAX_TELNET_MSG_LENGTH	   1400
bool testSendPacket = 0 ;

class TrackerTelnetTcp : public TrackerComplement , TrackerTcpClient
{
private:
	TrackerSerialComm 	serial ;
	TrackerTcpComm 		tcp ;
	bool				tcpTxLock 	= false ;
	bool 				trace 		= true ;
#if 0
#define TX_BUF_SIZE 1400

	uint8_t 			data [ TX_BUF_SIZE ] ;

	inline void serialToTcp () {
		// Prevent re-entering from callback
		if ( ! tcpTxLock ) {
			if ( serial.available () ) {
				tcpTxLock = true ;
				tcp.send ( data , serial.receive ( data , TX_BUF_SIZE )  ) ;
				tcpTxLock = false ;
			}
			else {
				tcp.send () ;
			}
		}
	}
#else
	inline void ICACHE_RAM_ATTR serialToTcp () {
		// Prevent re-entering from callback
		if ( ! tcpTxLock ) {
			tcpTxLock = true ;
			int nb = MIN( serial.available () , TcpBuffer::bufSize ) ;
			if ( nb ) {
				TcpBuffer * buffer = tcp.getTxBuffer_ ( (size_t*) & nb ) ;
				if ( buffer != NULL ) {
					buffer->length += serial.receive ( & buffer->data [ buffer->length ] , nb ) ;
					tcp.send ( buffer ) ;
				}
				else {
					Serial1.println ("telnet: serialToTcp: RUNNING OUT OF BUFFERS") ;
				}
			}
			else {
				tcp.send () ;
			}
			tcpTxLock = false ;
		}
	}
#endif

public:
	virtual ~TrackerTcpTelnet () {}
	TrackerTcpTelnet () {}
	bool begin ( const uint32_t serialLineRate , const uint32_t localPort ) {

		serial.setConfig ( serialLineRate ) ;
		serial.begin () ;

		tcp.setConfig 	 ( localPort ) ;
		tcp.registerForCallback ( this ) ;
		tcp.begin () ;

		return true ;
	}

	void ICACHE_RAM_ATTR txCompleteCallback ( void ) {
		serialToTcp () ;
	}
	void ICACHE_RAM_ATTR rxCallback ( char *data , uint16_t length ) {
		serial.send ( (uint8_t*) data , length ) ;
	}
	void process ( void ) {
		if ( tcp.connected ) serialToTcp () ;
#if 0
		if ( tcp.available () ) {
			uint16_t length = tcp.receive ( data , tcp.available () ) ;
			if ( length ) {
				serial.send ( data , length ) ;
			}
		}
#endif
	}
};

#undef  __CLASS__
#endif /* _TRACKER_TELNET_TDP_H_ */
