/*
 * Trackercomplement.h
 *
 *  Created on: 9 avr. 2016
 *      Author: gilles
 */

#ifndef _TRACKERCOMPLEMENT_H_
#define _TRACKERCOMPLEMENT_H_

class TrackerComplement {
public:
	TrackerComplement () {} ;
	virtual ~TrackerComplement () {} ;
	virtual bool begin 	( const uint32_t serialLineRate , const uint32_t localPort )  = 0 ;
	virtual void process 	( void ) = 0 ;
};

#endif /* _TRACKERCOMPLEMENT_H_ */
