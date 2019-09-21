/*
 * TrackerUdpTelnet.h
 *
 *  Created on: 9 avr. 2016
 *      Author: gmsj
 */

#ifndef _TRACKER_TELNET_UDP_H_
#define _TRACKER_TELNET_UDP_H_

#include "tracker_serial_comm.h"
#include "tracker_udp_comm.h"
#include "tracker_communication.h"
#include "tracker_complement.h"

#define MAX_TELNET_MSG_LENGTH	   1400
bool testSendPacket = 0 ;

class TrackerTelnetUdp : public TrackerComplement
{
private:
	TrackerSerialComm serial ;
	TrackerUdpComm 		udp ;
	uint8_t 			data [ MAX_TELNET_MSG_LENGTH ] ;
public:
	virtual ~TrackerUdpTelnet () {} ;
	TrackerUdpTelnet () {}

	bool begin ( const uint32_t serialLineRate , const uint32_t localPort ) {
		serial.setConfig ( serialLineRate  	) ;
		serial.begin 	 () ;

		udp.setConfig 	 ( localPort 		) ;
		udp.begin 		 () ;

		return true ;
	}
	void process ( void )
	{
		if ( serial.available () ) {
			udp.send ( data , serial.receive ( data , MAX_TELNET_MSG_LENGTH ) ) ;
		}
		else if ( udp.available () && serial.availableForSend () ) {
			int nb = ( udp.available () < serial.availableForSend () ) ? udp.available () : serial.availableForSend () ;
			serial.send ( data , udp.receive ( data , nb ) ) ;
		}
		else {
			udp.endPacket () ;
		}
		if ( testSendPacket ) {
			udp.sendPacket ( data , MAX_TELNET_MSG_LENGTH ) ;
		}
	}
};
#endif /* _TRACKER_TELNET_UDP_H_ */
