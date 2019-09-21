/*
 * TargetAttribute.cpp
 *
 *  Created on: 18 mars 2016
 *      Author: gilles
 */

#include "target_attribute.h"

TargetAttribute::TargetAttribute ( void )
{
	this->tracker = &Tracker::getTracker ( ) ;
	init ( "" , "" , INT_16, NULL ) ;
}

TargetAttribute::TargetAttribute ( const char * name, const char * groupName , AttributeType type, void (*callback) ( void ) )
{
	this->tracker = &Tracker::getTracker ( ) ;
	init ( name , groupName , type, callback ) ;
}

void TargetAttribute::init ( const char * name, const char * groupName , AttributeType type, void (*callback) ( void ) )
{
	this->name	 		 = name ;
	this->groupName	 	 = groupName ;
	this->type			 = type ;
	this->callback  	 = callback ;
	this->readOnly  	 = false ;
	this->dirty 		 = false ;
	this->id 			 = 0 ;

	tracker->addAttribute ( * this ) ;
}
