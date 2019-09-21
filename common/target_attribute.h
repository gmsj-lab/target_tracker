/*
 * TargetAttribute.h
 *
 *  Created on: 18 mars 2016
 *      Author: gmsj
 */

#ifndef _TARGETATTRIBUTE_H_
#define _TARGETATTRIBUTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "ftoa.h"
#include "tracker.h"

#include "itoa.h"

#define UINT_1024_MASK 				0x03FF
#define DECIMAL_BASE				10
#define NO_GROUP					""

enum AttributeType
{
	GROUP		='G',
	BOOL		='B',
	UINT_8		='D',
	INT_8		='C',
	UINT_10		='S',
	UINT_16		='J',
	INT_16		='I',
	UINT_32		='M',
	INT_32		='L',
	FLOAT_32	='F',
	ARRAY_8		='A'
} ;

class TargetAttribute
{

private:
	Tracker *		tracker ;

	const char *	name ;
	const char *	groupName ;
	AttributeType  	type ;
	bool 			trackingOn ;
	bool 			readOnly ;

	TargetAttribute ( void ) ;
	void init ( const char * name, const char * groupName , AttributeType type, void (*callback) ( void ) ) ;

protected:
	uint8_t 			id ;
	bool 				dirty ;
	void 				( *callback ) ( void ) ;
	inline void 		 setDirty ( void ) 			 			 { if ( !dirty ) {
																		tracker->setAttributeIdChanged ( id ) ;
																		this->dirty = true ;
																		}										}
public:
	virtual ~TargetAttribute () { } ;

	TargetAttribute ( const char * name, const char * groupName , AttributeType type, void (*callback) ( void ) ) ;

	virtual uint8_t		 getStringValue ( char * buf )			 = 0 ;
	virtual bool 		 setStringValue ( const char * buf )	 = 0 ;

	inline const char *  getName () 							 { return ( (const char *) this->name ) ; 		}
	inline const char *  getGroupName () 						 { return ( (const char *) this->groupName ) ; 	}
	inline AttributeType getType () 		 		 			 { return ( this->type ) ; 						}
	inline void 		 setTracking ( bool trackingOn = true )	 { this->trackingOn = trackingOn ;				}
	inline void 		 setReadOnly ( const bool mode = false ) { this->readOnly  = mode ; 					}
	inline bool 		 isReadOnly () 							 { return ( this->readOnly ) ; 					}
	inline void 		 setId( const uint8_t id ) 			 	 { this->id  = id ; 							}
	inline const uint8_t getId ()								 { return ( (const uint8_t) id ) ; 				}
	inline void 		 resetDirty( void ) 			 		 { this->dirty = false ; 						}
};

//---------------------------------------------------------------------------
//                              Attributes classes
//---------------------------------------------------------------------------

class TT_bool : public TargetAttribute
{
private:
	bool	value ;
	bool * 	valueRef ;

public:
	TT_bool ( 					const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, BOOL , callback )
		{ value = false ; valueRef = & value ; }
	TT_bool ( bool & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, BOOL , callback )
		{ value = false ; valueRef = & attribute ; }

	inline void 	set ( bool value )					{ * valueRef = value ; setDirty () ;											 	}
	inline bool 	get () 				 				{ return * valueRef ; 															 	}
	inline uint8_t	getStringValue ( char * buf ) 		{ itoa ( get (), buf, DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 				 	}
	inline bool		setStringValue ( const char * buf ) { *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;
//---------------------------------------------------------------------------
class TT_uint8 : public TargetAttribute
{
private:
	uint8_t   value ;
	uint8_t * valueRef ;

public:
	TT_uint8 ( 					    const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_8 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_uint8 ( uint8_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_8 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void    set ( uint8_t value ) 				{ *valueRef = value ; setDirty () ;	 												}
	inline uint8_t get () 				 				{ return *valueRef ;																}
	uint8_t		   getStringValue ( char * buf ) 		{ itoa ( get (), buf , DECIMAL_BASE ) ; 	return ( strlen ( buf ) ) ; 					}
	bool		   setStringValue ( const char * buf ) 	{ *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_int8  : public TargetAttribute
{
private:
	int8_t 	 value ;
	int8_t * valueRef ;

public:
	TT_int8 ( 					  const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, INT_8 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_int8 ( int8_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, INT_8 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void    set ( int8_t value ) 				{ *valueRef = value ; setDirty () ;	 												}
	inline int8_t  get () 				 				{ return *valueRef ;																}
	uint8_t		   getStringValue ( char * buf ) 		{ itoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 					}
	bool		   setStringValue ( const char * buf ) 	{ *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_uint10 : public TargetAttribute
{
private:
	uint16_t   value ;
	uint16_t * valueRef ;

public:
	TT_uint10 ( 				      const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_10 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_uint10 ( uint16_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_10 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void 	set ( uint16_t value ) 				{ *valueRef = ( value & UINT_1024_MASK ); setDirty () ;	 							}
	inline uint16_t get ( void )	  	 				{ return *valueRef ;																}
	uint8_t		    getStringValue ( char * buf ) 		{ itoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 					}
	bool		    setStringValue ( const char * buf ) { *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_uint16 : public TargetAttribute
{
private:
	uint16_t   value ;
	uint16_t * valueRef ;

public:
	TT_uint16 ( 				 	  const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_16 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_uint16 ( uint16_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_16 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void 	set ( uint16_t value ) 				{ *valueRef = value ; setDirty () ;	 												}
	inline uint16_t get ( void )	  	 				{ return *valueRef ;																}
	uint8_t		    getStringValue ( char * buf ) 		{ ltoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 					}
	bool		    setStringValue ( const char * buf ) { *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_int16 : public TargetAttribute
{
private:
	int16_t   value ;
	int16_t * valueRef ;

public:
	TT_int16 ( 				 	    const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, INT_16 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_int16 ( int16_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, INT_16 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void 	set ( int16_t value ) 				{ *valueRef = value ; setDirty () ;	 												}
	inline int16_t  get ( void )	  	 				{ return *valueRef ;																}
	uint8_t		    getStringValue ( char * buf ) 		{ itoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ;				 	}
	bool		    setStringValue ( const char * buf ) { *valueRef = atoi( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_uint32 : public TargetAttribute
{
private:
	uint32_t   value ;
	uint32_t * valueRef ;

public:
	TT_uint32 ( 				  	  const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute (name, groupName, UINT_32 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_uint32 ( uint32_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ) : TargetAttribute ( name, groupName, UINT_32 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void  	set ( uint32_t value )				{ *valueRef = value ; setDirty () ;	 												}
	inline uint32_t get ( void )	  	  				{ return *valueRef ;																}
	uint8_t		    getStringValue ( char * buf ) 		{ ultoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 					}
	bool		    setStringValue ( const char * buf ) { *valueRef = atol( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_int32 : public TargetAttribute
{
private:
	int32_t   value ;
	int32_t * valueRef ;

public:
	TT_int32 ( 					 	const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL )	: TargetAttribute ( name, groupName, INT_32 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_int32 ( int32_t & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL )	: TargetAttribute ( name, groupName, INT_32 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void    set ( int32_t value ) 				{ *valueRef = value ; setDirty () ;													}
	inline int32_t get ( void )	  	 					{ return *valueRef ;																}
	uint8_t		   getStringValue ( char * buf ) 		{ ltoa ( get (), buf , DECIMAL_BASE ) ; return ( strlen ( buf ) ) ; 					}
	bool		   setStringValue ( const char * buf )  { *valueRef = atol( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; }
} ;

class TT_float : public TargetAttribute
{
private:
	float   value ;
	float * valueRef ;

public:
	TT_float ( 					  const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL )	: TargetAttribute ( name, groupName, FLOAT_32 , callback )
		{ value = 0 ; valueRef = & value ; }
	TT_float ( float & attribute, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL )	: TargetAttribute ( name, groupName, FLOAT_32 , callback )
		{ value = 0 ; valueRef = & attribute ; }

	inline void  	set ( float value ) 				{ *valueRef = value ; setDirty () ;	 														}
	inline float	get ( void )	  	 				{ return *valueRef ;																		}
	uint8_t		   	getStringValue ( char * buf ) 		{ return ( (uint8_t) ftoa ( buf, get () ) ) ;												}
	bool		   	setStringValue ( const char * buf ) { *valueRef = (float)atof( buf ) ; if ( callback != NULL ) callback () ; return ( true ) ; 	}
} ;
// int length = snprintf( buf, MAX_FLOAT_SIZE, "%f", get () ) ; return ( ( length > MAX_FLOAT_SIZE ) ? MAX_FLOAT_SIZE : length )

typedef DynamicArray < uint8_t > ArrayValue ;

class TT_int8array : public TargetAttribute // TODO
{
private:
	uint8_t size ;
	ArrayValue value ;
	uint8_t *  valueRef ;
public:
	TT_int8array ( 					    uint8_t size, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ): TargetAttribute ( name, groupName, ARRAY_8 , callback )
		{ value = 0 ; valueRef = & value [ 0 ] ; this->size = size ; }
	TT_int8array ( uint8_t & attribute, uint8_t size, const char * name, const char * groupName = NO_GROUP , void (*callback) ( void ) = NULL ): TargetAttribute ( name, groupName, ARRAY_8 , callback )
		{ value = 0 ; valueRef = & attribute ; this->size = size ; }

	inline void 	 set ( void ) ; // TODO
	inline uint8_t * get ( void ) ; // TODO
} ;

#endif /* _TARGETATTRIBUTE_H_ */
