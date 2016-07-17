/*
=================================================================================================
	(c) Copyright 1995-2011 By E. Scott Daniels. All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
   		1. Redistributions of source code must retain the above copyright notice, this list of
      		conditions and the following disclaimer.
		
   		2. Redistributions in binary form must reproduce the above copyright notice, this list
      		of conditions and the following disclaimer in the documentation and/or other materials
      		provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY E. Scott Daniels ``AS IS'' AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL E. Scott Daniels OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
	ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	
	The views and conclusions contained in the software and documentation are those of the
	authors and should not be interpreted as representing official policies, either expressed
	or implied, of E. Scott Daniels.
=================================================================================================
*/
/*
*************************************************************************
*
*  Mnemonic: date.c
*  Abstract: Functions that Xfm uses with respect to date generation/manipulation.
*  Returns:  Nothing (void) *m will be assigned 0 in the event of an error.
*  Date:     26 December 1996
*  Author:   E. Scott Daniels
*
**************************************************************************
*/
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

extern void get_mdy( int *m, int *d, int *y )
{
 time_t t;						/* pointer returned by clock, filled in by time */
 struct tm *cur_time;			/* structure for localtime call to fill in */

 time( &t );					/* get seconds past midnight on magic day */
 cur_time = localtime( &t );	/* get current time/day */

 *d = cur_time->tm_mday;         /* fill in caller's variables */
 *m = cur_time->tm_mon + 1;      /* make month 1 based */
 *y = cur_time->tm_year;         /* years since 1900 */

}


/*
	Return time in hours, minutes, seconds by writing to user supplied
	addresses.
*/
extern void get_times( int *h, int *m, int *s )
{
 time_t t;               /* pointer returned by clock, filled in by time */
 struct tm *cur_time;         /* structure for localtime call to fill in */

 *h = -1;                     /* default to error */

 time( &t );            /* get seconds past midnight on magic day */
 cur_time = localtime( &t );  /* get current time/day */

 *h = cur_time->tm_hour;
 *m = cur_time->tm_min;
 *s = cur_time->tm_sec;
}
