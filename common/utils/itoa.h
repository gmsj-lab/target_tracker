/*
 * ltoa.h
 *
 *  Created on: 2 sept. 2016
 *      Author: gmsj
 */

#ifndef _LTOA_H_
#define _LTOA_H_

#if BOARD_TYPE == STM_32
#include <stdint.h>

#define BUFSIZE (sizeof(long) * 8 + 1)

inline char *ltoa ( long N, char *str, int base )
{
      register int i = 2;
      long uarg;
      char *tail, *head = str, buf[BUFSIZE];

      if (36 < base || 2 > base)
            base = 10;                    /* can only use 0-9, A-Z        */
      tail = &buf[BUFSIZE - 1];           /* last character position      */
      *tail-- = '\0';

      if (10 == base && N < 0L)
      {
            *head++ = '-';
            uarg    = -N;
      }
      else  uarg = N;

      if (uarg)
      {
            for (i = 1; uarg; ++i)
            {
                  register ldiv_t r;

                  r       = ldiv(uarg, base);
                  *tail-- = (char)(r.rem + ((9L < r.rem) ?  ('A' - 10L) : '0'));
                  uarg    = r.quot;
            }
      }
      else  *tail-- = '0';

      memcpy(head, ++tail, i);
      return str;
}
inline char *ultoa(unsigned long num, char *str, int radix) {
    char temp[33];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int digit;
    int str_loc = 0;

    //construct a backward string of the number.
    do {
        digit = (unsigned long)num % radix;
        if (digit < 10)
            temp[temp_loc++] = digit + '0';
        else
            temp[temp_loc++] = digit - 10 + 'A';
        num = ((unsigned long)num) / radix;
    } while ((unsigned long)num > 0);

    temp_loc--;


    //now reverse the string.
    while ( temp_loc >=0 ) {// while there are still chars
        str[str_loc++] = temp[temp_loc--];
    }
    str[str_loc] = 0; // add null termination.

    return str;
}

#endif /* BOARD_TYPE == STM_32 */
#endif /* _LTOA_H_ */
