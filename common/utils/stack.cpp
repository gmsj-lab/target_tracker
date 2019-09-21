/*
 * Stack.cpp
 *
 *  Created on: 23 juil. 2014
 *      Author: gilles Monzat de Saint Julien
 */

#include "stack.h"


Stack::Stack ( void )
{
	top_ = -1 ;
	size_ = INITIAL_SIZE ;
	data_ = new int [ size_ ] ;
}

void Stack::resize ( void )
{
	// create a bigger sized array
	int newSize = size_ + INITIAL_SIZE ;
	int* temp = new int [ newSize ] ;

	// copy values to new array
	for ( int i = 0 ; i < size_ ; i++ ) {
		temp[ i ] = data_ [ i ] ;
	}
	// free old array memory
	delete [] data_ ;
	// now data points to new array
	data_ = temp ;
	size_ = newSize ;
}
void Stack::push ( int element )
{
	if ( top_ >= size_ )
	{
		// not big enough, double it !
		resize () ;
	}
	data_[ ++top_ ] = element ;
}

void Stack::clear ( void )
{
	top_ = -1 ;
}

int Stack::pop ( void )
{
	if ( top_ == -1 )
	{
		//GM001 Serial.println( "INTERNAL ERROR !" ) ;
	}
	return data_ [ top_-- ] ;
}

int Stack::top ( void )
{
	return data_ [ top_ ] ;
}

int Stack::length ( void )
{
	return top_ + 1 ;
}

bool Stack::isEmpty ( void )
{
	return ( top_ == -1 ) ? true : false ;
}


