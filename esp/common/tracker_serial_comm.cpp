/*
 * TrackerSerialComm.cpp
 *
 *  Created on: 19 sept. 2016
 *      Author: gmsj
 */
#include "tracker_serial_comm.h"

#include "utilities.h"

#define DISABLE_UART_IT()	uart_rx_interrupt_disable() //ETS_UART_INTR_DISABLE ()  	// uint32_t savedPS = xt_rsil(15) 	//ETS_INTR_LOCK()
#define ENABLE_UART_IT()	uart_rx_interrupt_enable()  //ETS_UART_INTR_ENABLE () 	//xt_wsr_ps(savedPS) 				//ETS_INTR_UNLOCK()
#define RX_IT_THRESHOLD		100
#define RTS_IT_THRESHOLD	110
//#define HW_RTS
//#define HW_CTS
LOCAL void 		uart_rx_interrupt_handler ( void *p ) ;
LOCAL void 		uart_rx_interrupt_disable() ;
LOCAL void 		uart_rx_interrupt_enable() ;
LOCAL char 		rxBuf [ MAX_RX_LENGTH ] ;
LOCAL uint16_t 	rxBufLength 			= 0 ;
size_t			nbMaxRxSerial  			= 0 ;

bool TrackerSerialComm::rx_overrun = false ;

bool TrackerSerialComm::begin ( void ) {
	Serial.begin ( serialRate ) ;
	rxBufLength 	= 0 ;
	rx_overrun  	= false ;
	nbMaxRxSerial	= 0 ;
	uart_config () ;
	return ( true ) ;
}
int ICACHE_RAM_ATTR TrackerSerialComm::receive ( uint8_t * data , size_t length ) {
	// Prevent interrupt from messing around while we work
	DISABLE_UART_IT() ;

	// Give from sw buffer
	size_t given = MIN ( length , rxBufLength ) ;
	if ( given ) {
	   os_memcpy ( data , rxBuf , given ) ;
	   rxBufLength -= given ;
	}

	// Anything left in sw buffer ?
	if ( rxBufLength ) {
		// copy what's left at start of local buffer
		memmove ( rxBuf , & rxBuf [ given ] , rxBufLength ) ;
	}
	// Do we need more from hw buffer ?
	else if ( length > given ) {
		// Get the maximum from hw to fulfill the request without blocking
		given += readBytes ( & data [ given ] , length-given ) ;
	}
	// copy the hw buffer into sw buffer
	rxBufLength += readBytes ( (uint8_t *)  & rxBuf [ rxBufLength ] , MAX_RX_LENGTH - rxBufLength ) ;

	ENABLE_UART_IT() ;
	return ( given ) ;
}
inline void TrackerSerialComm::uart_config () {
	// attach interrupt routine
	ETS_UART_INTR_ATTACH ( (void *) & uart_rx_interrupt_handler, NULL ) ;
	#if HW_RTS
	// HW FLOW CONTROL RTS PIN
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);
	#endif
	#if HW_CTS
	//HW FLOW CONTROL CTS PIN
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_U0CTS);
	#endif

	// clear UART RXFIFO & TXFIFO
    SET_PERI_REG_MASK(  UART_CONF0(UART0), UART_RXFIFO_RST | UART_TXFIFO_RST);
	CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST | UART_TXFIFO_RST);
	// set interrupt trigger threshold
	WRITE_PERI_REG( UART_CONF1(UART0), ((RX_IT_THRESHOLD & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S)
		#if HW_RTS
			| ((RTS_IT_THRESHOLD & UART_RX_FLOW_THRHD) << UART_RX_FLOW_THRHD_S) |
			UART_RX_FLOW_EN |   // enable rx flow control
		#endif
		) ;
	#if HW_CTS
		SET_PERI_REG_MASK(UART_CONF0(UART0), UART_TX_FLOW_EN);  //add this sentence to add a tx flow control via MTCK (CTS)
	#endif
    // clear all interrupt
    WRITE_PERI_REG( UART_INT_CLR(UART0), 0xffff ) ;
	// enable RX interrupt
    SET_PERI_REG_MASK( UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA ) ;
	// enable UART interrupt
	ETS_UART_INTR_ENABLE () ;
}
int ICACHE_RAM_ATTR TrackerSerialComm::available ( void ) {
	return rxBufLength + ( READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S) ) ;
}
inline int TrackerSerialComm::readBytes ( uint8_t * data , size_t length ) {
	uint16_t nbRead = 0 ;
	while (READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
		data [ nbRead ++ ] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF ;
		if ( nbRead >= length ) break ;
	}
	return nbRead ;
}
LOCAL void ICACHE_RAM_ATTR uart_rx_interrupt_disable() {
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA | UART_RXFIFO_TOUT_INT_ENA);
}

LOCAL void ICACHE_RAM_ATTR uart_rx_interrupt_enable() {
	WRITE_PERI_REG(   UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
	SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA | UART_RXFIFO_TOUT_INT_ENA);
}
LOCAL void ICACHE_RAM_ATTR uart_rx_interrupt_handler ( void *p ) {
	 if (UART_RXFIFO_FULL_INT_ST != (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST)) {
		// clear all interrupt ?
		WRITE_PERI_REG( UART_INT_CLR(UART0), 0xffff ) ;
        return ;
    }
	while (READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
		if ( rxBufLength < MAX_RX_LENGTH ) {
			rxBuf [ rxBufLength ++ ] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF ;
		}
		else {
			TrackerSerialComm::rx_overrun = true ;
			//Serial1.println ( "it:rx_overrun" ) ;
			// clear UART RXFIFO
			SET_PERI_REG_MASK(  UART_CONF0(UART0), UART_RXFIFO_RST );
			CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST );
		}
	}
	if ( rxBufLength > nbMaxRxSerial ) nbMaxRxSerial = rxBufLength ;
	// clear it
	WRITE_PERI_REG( UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR ) ;
}
