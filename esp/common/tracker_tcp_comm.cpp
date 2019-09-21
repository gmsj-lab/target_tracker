/*
 * TrackerTcpComm.cpp
 *
 *  Created on: 21 oct. 2016
 *      Author: gilles
 */
#include "tracker_tcp_comm.h"

bool   TrackerTcpComm::espconnSendNok 				= false ;
uint16 TrackerTcpComm::nbDiffSendLength 			= 0 ;
uint16 TrackerTcpComm::nbFreeTxQueueEmptyOnStoring 	= 0 ;
uint32 TrackerTcpComm::nbTxLockedOnSendCallback 	= 0 ;

BufferPool 	TrackerTcpComm::freeQueue ;
