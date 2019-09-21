/*
 * DynamicArray.h
 *
 *  Created on: 23 juil. 2014
 *      Author: gmsj
 */

#ifndef DYNAMICARRAY_H_
#define DYNAMICARRAY_H_

#include <stdint.h>
#include <stdio.h>

#define ARRAY_INITIAL_SIZE	120

template < typename T > class DynamicArray {
private:
	uint8_t size ;
	T * 	data ;
public:

	DynamicArray ( uint8_t initialSize = ARRAY_INITIAL_SIZE ) {
		size = initialSize ;
		data = new T  [ size ] ;
	}

	 T& operator[] ( size_t index ) {
		if ( index >= size ){
			resize () ;
		}
		return data [ index ] ;
	}

	 const T& operator[]( size_t index ) const {
	    return const_cast<T&>(*this)[ index ] ;
	}

	void resize () {
		// create a bigger sized array
		uint8_t newSize = size + ( size / 2 ) ;
		T* temp = new T [ newSize ] ;

		// copy values to new array
		for ( uint8_t i = 0 ; i < size ; i++ ) {
			temp[ i ] = data[ i ] ;
		}
		// free old array memory
		delete [] data ;
		// now data points to new array
		data = temp ;
		size = newSize ;
	}
	T * getArray () {
		return data ;
	}
	void release () {
		delete [] data ;
	}
	uint8_t length () {
		return size ;
	}
} ;

#endif /* DYNAMICARRAY_H_ */

