#ifndef TRACKERTCPCOMM_H_
#define TRACKERTCPCOMM_H_

#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "tracker_communication.h"
#include "utilities.h"
#include "esp_tcp_driver.h"

#undef  __CLASS__
#define __CLASS__	"TrackerTcpComm"

#define LOCK_IT()		uint32_t savedPS = xt_rsil(15) 	//ETS_INTR_LOCK()
#define UNLOCK_IT()		xt_wsr_ps(savedPS) 				//ETS_INTR_UNLOCK()

#define TCP_PACKET_MAX_SIZE  	1460	// DEBUG !!!!!! 1000
#define BUFFER_POOL_SIZE	      16

class TcpBuffer {
public:
	static const uint16_t bufSize = TCP_PACKET_MAX_SIZE ;
	uint16_t length ;
	uint16_t offset ;
	uint8_t	 data [ bufSize ] ;

	TcpBuffer () {
	   length = 0 ;
	   offset = 0 ;
	}
} ;
typedef Fifo < TcpBuffer * >  BufQueue ;

class BufferPool {
private:
	BufQueue freeQueue = BufQueue ( BUFFER_POOL_SIZE + 1 ) ;
public:
	BufferPool () {
		// fill the free buffer list
		for ( int i = 0 ; i < BUFFER_POOL_SIZE ; i ++ ) {
			freeQueue.push ( new TcpBuffer ) ;
		}
	}
	inline bool isEmpty ( void ) {
		return freeQueue.isEmpty () ;
	}
	inline TcpBuffer * pop ( void ) {
		LOCK_IT() ;
		TcpBuffer * buf = freeQueue.pop () ;
		UNLOCK_IT() ;
		return buf  ;
	}
	inline void push ( TcpBuffer * tcpBuffer ) {
		LOCK_IT() ;
		freeQueue.push ( tcpBuffer ) ;
		UNLOCK_IT() ;
	}
} ;

class TrackerTcpClient {
public:
	virtual void txCompleteCallback ( void ) {} ;
	virtual void rxCallback ( char *data , uint16_t length ) {} ;
	virtual 	 ~TrackerTcpClient () {} ;
};

class TrackerTcpComm : public TrackerCommunication, TcpConnectionClient
{
public:
	bool				connected ;

private:
	static BufferPool 	freeQueue ;

	int 				port ;
	TrackerTcpClient 	*client ;
	bool        		trace ;
	// TX :
	volatile bool		txLock ;
	volatile bool		readyToSend ;
	BufQueue 			waitingTxQueue 	= BufQueue ( BUFFER_POOL_SIZE + 1 ) ;
	BufQueue			activeTxQueue 	= BufQueue ( BUFFER_POOL_SIZE + 1 ) ;
	// RX :
	BufQueue 			waitingRxQueue	= BufQueue ( BUFFER_POOL_SIZE + 1 ) ;
	TcpBuffer *			rxBufferIn ;
	TcpBuffer *			rxBufferOut ;
	uint16_t			rxBufLenAvail ;

	void ICACHE_RAM_ATTR connectCallback ( void ) {
		TRACE_PARAM( ": Connecting remote port: " , esp_tcp_conn.remote_port ) ;
		connected = true ;
	}
	void ICACHE_RAM_ATTR reconnectCallback 	( sint8 err ) {
		TRACE_PARAM( "err=" , err ) ;
		connected = true ;
	}
	void ICACHE_RAM_ATTR disconnectCallback ( void ) {
		TRACE_PARAM( ": Disconnecting remote port: " , esp_tcp_conn.remote_port ) ;
		connected = false ;
	}
	void ICACHE_RAM_ATTR receiveCallback ( char *data , uint16_t length ) {

		if ( client ) {
			client->rxCallback ( data , length ) ;
		}
		else {
			while ( length ) {
				// Still some room available in current buffer ?
				if ( ( rxBufferIn == NULL ) || ( rxBufferIn->bufSize - rxBufferIn->length == 0 ) ) {
					// No more room in current buffer, Store it and get a new one
					if ( rxBufferIn != NULL ) {
						waitingRxQueue.push ( rxBufferIn ) ;
					}
					rxBufferIn = freeQueue.pop () ;
					if ( rxBufferIn ) {
						rxBufferIn->length = 0 ;
					}
					else {
						TRACE( " no more free rx buffer available" ) ;
						rxBufferIn = NULL ;
						return ;
					}
				}
				// Copy incoming data to buffer
				uint16_t len = MIN ( length , rxBufferIn->bufSize - rxBufferIn->length ) ;
				os_memcpy ( rxBufferIn->data + rxBufferIn->length , data , len ) ;
				rxBufferIn->length 	+= len ;
				length 				-= len ;
				rxBufLenAvail 		+= len ;
			}
		}
	}
	void ICACHE_RAM_ATTR sendCallback ( void ) {
		releaseBuffers () ;
		if ( ! txLock ) {
			if ( client ) {
				client->txCompleteCallback () ;
			}
			else {
				processTransmission () ;
			}
		}
		else {
			nbTxLockedOnSendCallback ++ ;
			//TRACE_PARAM( " Locked on txLock !!!!!!!" , nbTxLockedOnSendCallback ) ;
		}
	}
#if TCP_DEBUG_INFO
	inline void traceInfo () {
			TRACE_PARAM_ ( ":snd_queuelen= ", info.snd_queuelen ) ;
			TRACE_PARAM_ ( ":snd_buf_size= ", info.snd_buf_size ) ;
			TRACE_PARAM_ ( ":sent_length= ", info.sent_length ) ;
			TRACE_PARAM_ ( ":packseqno= ", info.packseqno ) ;
			TRACE_PARAM_ ( ":packseq_nxt= ", info.packseq_nxt ) ;
			TRACE_PARAM  ( ":packnum= "	, info.packnum ) ;
		}
#endif
	inline void ICACHE_RAM_ATTR releaseBuffers ( void ) {
		if ( ! activeTxQueue.isEmpty () ) {
			TcpBuffer *buf = activeTxQueue.peek () ;
			espconn_get_packet_info ( & conn, & info ) ;
			buf->offset += info.sent_length ;
			if ( buf->offset == buf->length ) {
				freeQueue.push ( activeTxQueue.pop () ) ;
				//TRACE_PARAM( " FREE len:" , info.sent_length ) ;
				readyToSend  = true ;
			}
			else {
				nbDiffSendLength ++ ;
				TRACE_PARAM( "Send Length is different !!!!!!!" , info.sent_length ) ;
				if (trace) Serial1.println ( buf->length ) ;
			}
		}
	}
	inline void ICACHE_RAM_ATTR processTransmission () {
		if ( ( ! waitingTxQueue.isEmpty () ) && readyToSend ) {
			readyToSend	= false ;
			// Data is waiting, room in esp driver, let's try to send some of it
			TcpBuffer *buf = waitingTxQueue.pop () ;
			buf->offset = 0 ;
			activeTxQueue.push ( buf ) ;

			sint8 retVal = espconn_send ( & conn , buf->data , buf->length ) ;
			if ( retVal != ESPCONN_OK ) {
				//Utilities::blinkLed ( 10 , 3000 ) ;
				TRACE_PARAM_ ( "espconn_send NOK: " , retVal ) ;
				TRACE_PARAM_ ( "activeTxQueue.getLength (): " , activeTxQueue.getLength () ) ;
				TRACE_PARAM  ( "info.snd_queuelen: " , info.snd_queuelen ) ;

				espconnSendNok = true ;
				waitingTxQueue.pushOnTop ( activeTxQueue.pop () ) ;
			}
		}
	}
	inline bool ICACHE_RAM_ATTR addToLastBuffer ( const uint8_t * data , size_t length )
	{
		bool result = false ;

		if ( ! waitingTxQueue.isEmpty () ) {
			// Let see if we can add data to the last pushed buffer
			TcpBuffer * buf = waitingTxQueue.peekLastPushed () ;
			if ( ( buf->length + length ) < buf->bufSize ) {
				// add this data to it
				os_memcpy ( & buf->data [ buf->length ] , data , length ) ;
				buf->length += length ;
				result = true ;
				//TRACE_PARAM ( "adding to last buffer " , buf->length ) ;
			}
		}
		return result ;
	}
	inline void ICACHE_RAM_ATTR storeWaitingQueue ( const uint8_t * data , size_t length )
	{
		bool first = true ;
		TcpBuffer * buf = freeQueue.pop () ;
		// Wait for a free block if need be
		while ( ! buf ) {
			delay ( 0 ) ;
			buf = freeQueue.pop () ;
			if ( first == true ) {
				nbFreeTxQueueEmptyOnStoring ++ ;
				//TRACE ( "   waiting for free buffer " ) ;
				first = false ;
			}
		}

		buf->length = length ;
		os_memcpy ( buf->data , data , length ) ;
		waitingTxQueue.push ( buf ) ;
	}
public:
	static bool 	espconnSendNok ;
	static uint16 	nbDiffSendLength ;
	static uint16 	nbFreeTxQueueEmptyOnStoring ;
	static uint32 	nbTxLockedOnSendCallback ;
	TcpBuffer *		tcpBuf ;

	TrackerTcpComm ()
	{
		port			= 0 ;
		client			= NULL ;
		trace			= false ;
		connected		= false ;
		rxBufLenAvail	= 0 ;
		rxBufferIn 		= NULL ;
		rxBufferOut 	= NULL ;

		txLock			= false ;
		readyToSend		= true ;

		tcpBuf = NULL ;
	}
	virtual ~TrackerTcpComm () {} ;

	void registerForCallback ( TrackerTcpClient *client ) {
		this->client = client ;
	}
	void setConfig ( int port ) {
		this->port = port ;
	}
	inline bool begin () {
		TRACE_PARAM ( "calling EspTcpDriver::begin on port : ", port) ;

		return EspTcpDriver::begin ( port , this ) ;
	}
	inline int ICACHE_RAM_ATTR available ( void )
	{
		return  ( connected ) ? rxBufLenAvail : 0 ;
	}
	inline int receive ( uint8_t * data , size_t length )
	{
		size_t 		given = 0 ;
		uint16_t	lenAvail = rxBufLenAvail ;

		// Give no more than we've got
		if ( length > lenAvail ) length = lenAvail ;

		while ( given < length ) {
			if ( rxBufferOut == NULL ) {
				// Get a buffer from the waiting list, if any...
				rxBufferOut = waitingRxQueue.pop () ;

				if ( rxBufferOut == NULL ) {
					// Take the block under reception
					LOCK_IT() ;
					rxBufferOut = rxBufferIn ;
					rxBufferIn  = NULL ;
					UNLOCK_IT() ;

					if ( rxBufferOut == NULL ) {
						TRACE( " no more free receive buffer available" ) ;
						return given ;
					}
				}
			// New block, use Offset to show what we take out from this buffer
			rxBufferOut->offset = 0 ;
			}

			// Give no more than requested and no more than this buffer holds
			uint16_t len = MIN ( length , (size_t) rxBufferOut->length - rxBufferOut->offset ) ;
			os_memcpy ( data , rxBufferOut->data + rxBufferOut->offset , len ) ;
			rxBufferOut->offset += len ;
			given 				+= len ;
			if ( rxBufferOut->offset == rxBufferOut->length ) {
				// Free the buffer
				freeQueue.push ( rxBufferOut ) ;
				rxBufferOut = NULL ;
			}
		}
		LOCK_IT() ;
		rxBufLenAvail -= given ;
		UNLOCK_IT() ;
		return given ;
	}
	inline int availableForSend ( void )
	{
		int result = 0 ;
		if ( ( connected ) && (	readyToSend	) ) {
			espconn_get_packet_info ( & conn, & info ) ;

			if ( info.snd_queuelen > 0 ) {
				result = info.snd_buf_size ;
			}
		}
		return result ;
	}
	inline TcpBuffer * ICACHE_RAM_ATTR getTxBuffer ( size_t length )
	{
		TcpBuffer * buf = NULL ;

		if ( length > TcpBuffer::bufSize ) {
			TRACE_PARAM ( "length too big for buffer size " , length ) ;
		}
		else {
			txLock = true ;

			// Let see if we can add data to the last pushed buffer
			if ( ! waitingTxQueue.isEmpty () ) {
				buf = waitingTxQueue.peekLastPushed () ;
				if ( ( buf->length + length ) < buf->bufSize ) {
					// Ok, take it out of the waiting list
					return waitingTxQueue.popLastPushed () ;
				}
			}
			// no room in last buffer or no buffer waiting: allocate a new one if we can
			buf = freeQueue.pop () ;
			if ( buf ) {
				buf->length = 0 ;
			}
			txLock = false ;
		}
		return  buf ;
	}
	// getTxBuffer_ : May give less than requested to optimize message size and buffer usage
	inline TcpBuffer * ICACHE_RAM_ATTR getTxBuffer_ ( size_t * length )
	{
		TcpBuffer * buf = NULL ;

		if ( *length > TcpBuffer::bufSize ) {
			TRACE_PARAM ( "length too big for buffer size " , *length ) ;
		}
		else {
			txLock = true ;

			// Let see if we can add data to the last pushed buffer
			if ( ! waitingTxQueue.isEmpty () ) {
				buf = waitingTxQueue.peekLastPushed () ;
				if ( buf->length < buf->bufSize ) {
					*length = buf->bufSize - buf->length ;
					// Ok, take it out of the waiting list
					return waitingTxQueue.popLastPushed () ;
				}
			}
			// no room in last buffer or no buffer waiting: allocate a new one if we can
			buf = freeQueue.pop () ;
			if ( buf ) {
				buf->length = 0 ;
			}
			txLock = false ;
		}
		return  buf ;
	}
	inline void ICACHE_RAM_ATTR send ( TcpBuffer * buffer )
	{
		if ( connected ) {
			txLock = true ;
			waitingTxQueue.push ( buffer ) ;
			processTransmission () ;
			txLock = false ;
		}
	}
	inline void  send ( const uint8_t * data , size_t length )
	{
		if ( connected ) {
			txLock = true ;

			// try to add to last buffer
			if ( addToLastBuffer ( data , length ) == false ) {
				// No, let's allocate a new buffer and store it in waiting queue
				storeWaitingQueue ( data , length ) ;
			}
			processTransmission () ;
			txLock = false ;
		}
	}
	inline void sendPacket ( const uint8_t * data , size_t length )
	{
		send ( data , length ) ;
	}
	inline void print ( const char * data )
	{
		sendPacket ( (const uint8_t *) data , strlen ( data ) ) ;
	}
	inline void println ( const char * data )
	{
		sendPacket ( (const uint8_t *)data , strlen ( data )  ) ;
		sendPacket ( (const uint8_t *)"\n" , strlen ( "\n" ) ) ;
	}
	inline void ICACHE_RAM_ATTR send ( void ) {
		if ( connected ) {
			txLock = true ;
			processTransmission () ;
			txLock = false ;
		}
	}
	inline void endPacket ( void ) {}

	bool isConnected () const {
		return connected ;
	}
	int getSendBuffer    ( char ** buf , int requestedSize ) // Optimized API
	{
		tcpBuf =  getTxBuffer (  requestedSize ) ;

		while ( ! tcpBuf ) {
			// Wait as long as need be to get a free tx buffer
			delay ( 2 ) ;
			tcpBuf =  getTxBuffer (  requestedSize ) ;
		}
		* buf = (char *) tcpBuf->data + tcpBuf->length ;
		return tcpBuf->bufSize - tcpBuf->length ;
	}
	void send ( int length )  // Optimized API
	{
		if ( tcpBuf ) {
			tcpBuf->length += length ;
			send ( tcpBuf ) ;
			tcpBuf = NULL ;
		}
		else {
			Utilities::blinkLed ( 10 , 3000 ) ;
		}
	}

	int getReceiveBuffer ( char ** data )	// Optimized API (not optimized for tcp ! for optimized tcp, use rxCallback)
	{
		static uint8_t	rxBuffer [ TCP_PACKET_MAX_SIZE ] ;
		int length = receive ( rxBuffer , TCP_PACKET_MAX_SIZE ) ;

		* data = (char *) rxBuffer ;
		return length ;
	}
	void receive ( int length )  // Optimized API
	{
		// Nothing to do.
	}
};

#ifdef __cplusplus
}
#endif
#undef  __CLASS__
#endif /* TRACKERTCPCOMM_H_ */
