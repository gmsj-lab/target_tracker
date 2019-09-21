/*
 * Fifo.h
 *
 *  Created on: 18 mars 2016
 *      Author: gmsj
 */

#ifndef _FIFO_H_
#define _FIFO_H_

#include "Arduino.h"	// To prevent bunch of compil errors in WString.h....no idea why
#include "stdint.h"

#define FIFO_INITIAL_SIZE	100
#define NEXT(x) 		( ( (x) + 1 ) % size )
#define PREVIOUS(x) 	( ( (x) == 0 ) ? size - 1 : ( (x) - 1 ) )

template < typename T > class Fifo
{
private:
	T * 	 fifo ;
	uint8_t	 length ;
	uint8_t	 size ;
	uint8_t  popIndex ;
	uint8_t  pushIndex ;

	Fifo ( const T & ) ;  				 // Non-copyable
	Fifo & operator = ( const Fifo & ) ; // Non-assignable

	inline void init ( uint8_t size )
	{
		this->popIndex 		= 0 ;
		this->pushIndex 	= 0 ;
		this->length 		= 0 ;
		this->size 			= size ;
		this->fifo 			= new T [ size ] ;
	}
	inline void resize ( void )
	{
	// create a bigger sized array ( array must be full ! )
	T * temp = new T [ size + FIFO_INITIAL_SIZE ] ;

	// copy values to new array
	for ( uint8_t i = 0 ; i < size ; i++ ) {
		temp [ i ] = pop () ;
	}
	popIndex   = 0 ;
	pushIndex  = size ;

	delete [] fifo ;
	fifo = temp ;
	size += FIFO_INITIAL_SIZE ;
	} ;
public:
	explicit Fifo ( uint8_t size = FIFO_INITIAL_SIZE ) { init ( size ) ; }
	~Fifo () { delete [] fifo ; }

	inline bool 	isEmpty 	( void ) { return ( popIndex == pushIndex ) ? true : false ; }
	inline void 	reset 		( void ) { delete [] fifo ; init ( size ) ; }
	inline void 	clear 		( void ) { popIndex = pushIndex ; }
	inline uint8_t 	getLength 	( void ) { return length ; }

	inline void push ( T element ) {
		length ++ ;
		fifo [ pushIndex ] = element ;
		pushIndex = NEXT ( pushIndex ) ;
		if ( pushIndex == popIndex ) resize () ; // buffer full
	}

	inline T pop ( void ) {
		T element = (T) NULL ;
		if ( popIndex != pushIndex ) {
			element = fifo [ popIndex ] ;
			length -- ;
			popIndex = NEXT ( popIndex ) ;
		}
		return element ;
	}

	inline void pushOnTop ( T element ) {
		length ++ ;
		popIndex = PREVIOUS ( popIndex ) ;
		fifo [ popIndex ] = element ;
		if ( pushIndex == popIndex ) resize () ; // buffer full
	}

	inline T peek ( void ) {
		return fifo [ popIndex ]  ;
	}
	inline T peekLastPushed ( void ) {
		return fifo [ PREVIOUS ( pushIndex ) ]  ;
	}

	inline T popLastPushed ( void ) {
		if ( length ) {
			length -- ;
			pushIndex = PREVIOUS ( pushIndex ) ;
		}
		return fifo [ pushIndex ] ;
	}
};

#endif /* _FIFO_H_ */
