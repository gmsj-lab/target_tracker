/*
 * Stack.h
 *
 *  Created on: 23 juil. 2014
 *      Author: gilles Monzat de Saint Julien
 */

#ifndef STACK_H_
#define STACK_H_


#define INITIAL_SIZE	10

class Stack {

private:
	int 	size_ ;
	int 	top_ ;
	int *	data_ ;
public:
	Stack 		( void ) ;

	void resize	( void ) ;
	void clear	( void ) ;
	void push	( int element ) ;
	int pop		( void ) ;
	int top		( void ) ;
	int length	( void ) ;
	bool isEmpty( void ) ;
};
#endif /* STACK_H_ */

