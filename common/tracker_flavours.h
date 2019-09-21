/*
 * TrackerDefines.h
 *
 *  Created on: 4 mars 2017
 *      Author: gmsj
 */

#ifndef _TRACKER_FLAVOURS_H_
#define _TRACKER_FLAVOURS_H_

// BOARD_TYPE
#define UNO 						 1
#define ESP_8266					 2
#define STM_32 						 3

// TELENT_PROTOCOL_TYPE (ESP_8266 ONLY)
#define _UDP						21
#define _TCP						22

// TRACKER MSG TAGGING_TYPE
#define TIME_TAGGING				true
#define SEQUENCE_NUMBERS			false

// WIFI_MODE
#define _STATION_MODE				false
#define _ACCESS_POINT_MODE			true

#endif /* _TRACKER_FLAVOURS_H_ */
