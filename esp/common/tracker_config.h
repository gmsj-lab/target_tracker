/*
 * TrackerConfig.h
 *
 *  Created on: 25 juil. 2016
 *      Author: gilles
 */
#ifndef _TRACKERCONFIG_H_
#define _TRACKERCONFIG_H_

// Tracker flavour specific includes
#include "utilities.h"
#include "wifi_setup.h"
#include "tracker_serial_comm.h"
#include "tracker_udp_comm.h"
#include "tracker_tcp_comm.h"

#if TELNET_PROTOCOL_TYPE == _UDP
	#include "tracker_telnet_udp.h"
#elif TELNET_PROTOCOL_TYPE == _TCP
	#include "tracker_telnet_tcp.h"
#else
	# error "Error in TELNET_PROTOCOL_TYPE definition: TRACKER only supports _UDP OR _TCP "
#endif /* TELNET_PROTOCOL_TYPE */

#endif /* _TRACKERCONFIG_H_ */

