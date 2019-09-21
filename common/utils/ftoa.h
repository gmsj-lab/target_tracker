/*
 * ftoa.h
 *
 *  Created on: 26 mars 2016
 *      Author: gmsj
 */

#ifndef _FTOA_H_
#define _FTOA_H_

#include "arduino.h"
#include <stdint.h>
#include <math.h>

#include "itoa.h"

#ifndef DECIMAL_BASE
	#define DECIMAL_BASE 10
#endif

#if 1

//static double PRECISION = 0.00000000000001;
static double PRECISION = 0.000000001;
//static int MAX_NUMBER_STRING_SIZE = 32;


/**
 * Double to ASCII
 */
inline char * __dtoa(char *s, double n) {
    // handle special cases
    if (isnan(n)) {
        strcpy(s, "nan");
    } else if (isinf(n)) {
        strcpy(s, "inf");
    } else if (n == 0.0) {
        strcpy(s, "0");
    } else {
        int digit, m, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}
inline int ftoa ( char *buf , double f , int precision = 8 )
{
	__dtoa( buf, f ) ;
	return strlen(buf) ;
}
#else

#define MAX_FLOAT_SIZE	20
inline int ftoa ( char *buf , double f ) {
	int length = snprintf ( buf, MAX_FLOAT_SIZE, "%lf", f ) ;
	return ( ( length >= MAX_FLOAT_SIZE ) ? MAX_FLOAT_SIZE : length ) ;
}

#endif

#if 0
inline int ftoa ( char *buf , double f , int precision = 8 )
{
	uint8_t i = 0 ;
	long p[] = { 0,10,100,1000,10000,100000,1000000,10000000,100000000 } ;

	long integerPart = ( long ) f ;

	if ( ( integerPart == 0 ) && ( f < 0.0 ) ) {
		// Insert maunally the sign, since ltoa will not does it for -0
		buf [ i ++ ] = '-' ;
 	}

	ltoa ( integerPart , & buf [ i ] , DECIMAL_BASE ) ;

	while ( buf [ i ] != '\0' ) i ++ ;

	// insert decimal part
	buf [ i ++ ] = '.';

	// TODO : Take care of leading zeros in decimal part...
	long decimalPart = abs ( ( long ) ( ( f - integerPart ) * p [ precision ] ) ) ;

	ltoa ( decimalPart, & buf [ i ], DECIMAL_BASE ) ;

	// remove unecessary zero on tail (remove the dot if no decimal part)
	i += strlen( & buf [ i ] ) ;
	while ( buf [ -- i ] == '0' ) ;
	if 	  ( buf [ i ] == '.' ) i -- ;
	buf  [ ++ i ] = '\0' ;

	return ( i ) ;
}
#endif

#endif /* _FTOA_H_ */



