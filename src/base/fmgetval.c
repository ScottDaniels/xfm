/*
All source and documentation in the xfm tree are published with the following open source license:
Contributions to this source repository are assumed published with the same license. 

=================================================================================================
	(c) Copyright 1995-2015 By E. Scott Daniels. All rights reserved.

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


#include <stdio.h>     
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>    
#include <ctype.h>   
#include <string.h> 
#include <memory.h>
#include <time.h>

#include "symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
**************************************************************************************************
*
*  Mnemonic: FMgetval
*  Abstract: This routine is invoked when the .gv command is encountered in
*            the input file. Based on input it creates a variable name (all with 
*			leading underbar) and sets the value to the current setting.  
*
*            The following are valid parameters:
*				D  	- Set variable _date to current dd <string> yyyy
*				d  	- Set variable _date to the current m/d/y date
*				e  env-name xfm-var-name - set xfm-var-name to environment var name env-name
*				fi 	- Set variable _fig to the current figure number
*				Fi 	- Set variable _fig to the current figure number AND advance the number
*				fo 	- Set variable _font to the current font string
*				h  	- set variable _host to the local host name
*				li[nes] - set variable _line to lines remaining in the current col.
*				lm  	- Set variable _lmar to current left margin value
*				mdy 	- Set variables _mon _day _year with current date values
*				p  	- Set variable _page to the current page number
*				rm[ar]  	- Set variable _rmar to current right margin
*				re[main] - Set the variables: 
*						_iremain 	(inches remaining)
*						_premain 	(points remaining)
*						_lremain	(lines remaining)	(also sets _lines for back compat)
*						_attop		(true if next line is first in column; false otherwise) 
*
*				s[ect] 	- Set the var _sect to the current h1 value
*				t[able]	- Set the var _table to the current table number
*				t[xt]  	- Set variable _tsize to current text point size
*				T[able]	- Set the variable _table to the current number AND advance the number
*				T[ime] 	- Set variable _time to current hours and minutes
*				w[ords]	- set the variable _words to be the current word count
*				v[er]  	- XFM/HFM/RFM version number into _ver
*				y  		- Set variable _cury to the current y value
*
*  Returns:  Nothing.
*  Date:     6 April 1993
*  Author:   E. Scott Daniels
*
*  Modified: 26 Mar 1997 - To correct problem with page number get
*             3 Apr 1997 - To use the tokenizer now in AFI!
*            28 Apr 2001 - To add e (environment snag) 
*			21 Aug 2011 - Corrected a long standing bug in date.
*				Now instantly creates the variable (call to FMset_var()) rather than 
*				pushing onto the input stream which prevented use in a macro. 
*			24 Jun 2013 - Added _attop to remain.
*			04 Mar 2014 - Added Fi[gure] and Ta[ble] types to set vars and incr the value
*			28 Dec 2015 - Correct variable name (_lremain not right).
*						  Correct computation of right mar for .gv rmar.
*			17 Jul 2016 - Changes for better prototype generation.
*			17 Mar 2018 - Fix printf warnings
**************************************************************************************************
*/
/*
#include <sys/systeminfo.h>
*/

char *mname[13] = {
 " ", "January", "February", "March", "April", "May", "June", "July", 
 "August", "September", "October", "November", "December" 
};

extern void FMgetval( void )
{
	char	*buf;			/* pointer to parameter to use */
	char	*ep;			/* pointer to value of env var */
	char	*ename;           /* pointer to environment var name */
	char	work[128];
	char	value[2048];        /* buffer to build .dv commands in */
	char	vname[128];
	int	m;                 /* parameters to get date/time in */
	int	d;
	int y;
	int h;
	int s;
	int i;
	struct col_blk *cp;

	*value = 0;
	*vname = 0;

 if( FMgetparm( &buf ) > 0 )  /* if there is a parameter on the line */
  {
   iptr = 0;           /* start the input pointer at beginning of buffer */
   switch( *buf )      /* look at user parameter and set psuedo command */
    {
     case 'D':         /* get string formatted date */
       get_mdy( &m, &d, &y );   /* get the values */
		strcpy( vname, "_date" );
       snprintf( value, sizeof( value ), "%d %s %d", d, mname[m], 1900 + y );
       break;

     case 'd':         /* set date value */
       get_mdy( &m, &d, &y );   /* get the values */
		strcpy( vname, "_date" );
       snprintf( value, sizeof( value ), "%d/%d/%d", m, d, 1900 + y );  /* create value */
       break;

	case 'E':								// eurpoean date dd/mm/yyyy
       get_mdy( &m, &d, &y );				// values
		strcpy( vname, "_date" );			// variable name
       snprintf( value, sizeof( value ), "%d/%d/%d", d, m, 1900 + y );  // value
       break;

     case 'e':								/* e env-name xfm-var-name */
			if( FMgetparm( &buf ) > 0 )		/* if there is a parameter on the line */
			{
				ename = strdup( buf );

				if( FMgetparm( &buf ) > 0 )
				{
					if( (ep = getenv( ename )) )
					{
						snprintf( vname, sizeof( vname ), "%s", buf );
						snprintf( value, sizeof( value ), "%s",  ep );
					}
				}
		
				free( ename );
			}
			break;
	
     case 'F':                      /* set figure number variable AND advance the counter */
		if( *(buf+1) == 'i' )
		{
			strcpy( vname, "_fig" );
			if( flags & PARA_NUM ) {
				snprintf( value, sizeof( value ), "%d-%d", pnum[0], fig );
			} else {
				snprintf( value, sizeof( value ), "%d", fig );
			}
			fig++;
		}
		break;
		

     case 'f':                      /* set figure number or font variable */
		if( *(buf+1) == 'i' )
		{
			if( flags & PARA_NUM )
			{
				strcpy( vname, "_fig" );
				snprintf( value, sizeof( value ), "%d-%d", pnum[0], fig );
			}
			else
			{
				strcpy( vname, "_fig" );
				snprintf( value, sizeof( value ), "%d", fig );
			}
		}
		else
		{
			strcpy( vname, "_font" );
       		snprintf( value, sizeof( value ), "%s", curfont );
		}
       break;

     case 'h':                       /* get host name */
       gethostname( work, 128 );
		strcpy( vname, "_host" );
       snprintf( value, sizeof( value ), "%s", work );
       break;

	case 'i':								// ISO 8601 extended date
       get_mdy( &m, &d, &y );				// values
		strcpy( vname, "_date" );			// variable name
       snprintf( value, sizeof( value ), "%d-%d-%d", 1900 + y, m, y );  // value
       break;

     case 'l':      /* set margin variables (lmar) or lines remaining in col (lines) */
		if( *(buf+1) == 'i' )
		{
			strcpy( vname, "_lines" );
			snprintf( value, sizeof( value ), "%d", (boty - cury)/(textsize + textspace) );
		}
		else
		{
			strcpy( vname, "_lmar" );
			snprintf( value, sizeof( value ), "%d", lmar );
		}
       break;

     case 'm':         					/* set month day year */
		get_mdy( &m, &d, &y );   				/* get the values */
		strcpy( vname, "_mon" );
		snprintf( value, sizeof( value ), "%s", mname[m] );
		FMset_var( vname, value );

		strcpy( vname, "_day" );
		snprintf( value, sizeof( value ), "%d", d );
		FMset_var( vname, value );

		strcpy( vname, "_year" );
		snprintf( value, sizeof( value ), "%d", 1900 + y );
		break;

     case 'p':       /* set page variable */
		strcpy( vname, "_page" );
       snprintf( value, sizeof( value ), "%d", page+1 );
       break;

     case 'r':       				/* remain | rmar */
		if( *(buf+1) == 'e' )		/* assume remain - generate iremain (inches) lines */
		{
			strcpy( vname, "_attop" );
			snprintf( value, sizeof( value ), "%s", cury == topy ? "true" : "false" );
			FMset_var( vname, value );

			strcpy( vname, "_lremain" );
			snprintf( value, sizeof( value ), "%d", (boty - cury)/(textsize + textspace) );
			FMset_var( vname, value );

			strcpy( vname, "_lines" );
			snprintf( value, sizeof( value ), "%d", (boty - cury)/(textsize + textspace) );
			FMset_var( vname, value );

			strcpy( vname, "_iremain" );
			snprintf( value, sizeof( value ), "%d", (boty - cury)/72 );		/* 72 points per inch */
			FMset_var( vname, value );

			strcpy( vname, "_premain" );
			snprintf( value, sizeof( value ), "%d", (boty - cury));		/* points */
			FMset_var( vname, value );
		}
		else
		{
			strcpy( vname, "_rmar" );
			snprintf( value, sizeof( value ), "%d", lmar + linelen );
		}
       break;
	
	case 's':
		strcpy( vname, "_sect" );
		ep = FMget_header_num();				// if numbering, get the number
		if( ! ep ) {
			ep = FMget_header_txt();			// else get the string
		}
		
		if( ep ) {
			snprintf( value, sizeof( value ), "%s", ep );
		} else {
			return;
		}
		break;


     case 't':
		if( *(buf+1) == 'a' )					// table number
		{
			if( flags & PARA_NUM )
			{
				strcpy( vname, "_table" );
				snprintf( value, sizeof( value ), "%d-%d", pnum[0], table_number );
			}
			else
			{
				strcpy( vname, "_table" );
				snprintf( value, sizeof( value ), "%d", table_number );
			}
		}
		else
		{
			strcpy( vname, "_tsize" );
			snprintf( value, sizeof( value ), "%d", textsize );
		}
       break;

     case 'T':        
		if( *(buf+1) == 'a' )					// table number and advance it
		{
			if( flags & PARA_NUM )
			{
				strcpy( vname, "_table" );
				snprintf( value, sizeof( value ), "%d-%d", pnum[0], table_number );
			}
			else
			{
				strcpy( vname, "_table" );
				snprintf( value, sizeof( value ), "%d", table_number );
			}

			table_number++;
		}
		else						// set time variable
		{
			get_times( &h, &m, &s );   					// current time 
			strcpy( vname, "_time" );
			snprintf( value, sizeof( value ), "%02d:%02d", h, m );
		}
       break;

     case 'w':
			strcpy( vname, "_words" );
			snprintf( value, sizeof( value ), "%ld", words );
			break;

     case 'v':
			strcpy( vname, "_ver" );
			snprintf( value, sizeof( value ), "%s", version );
			break;

     case 'y':       				/* set current y info: current y, top y, col-number  */
			strcpy( vname, "_cury" );
			snprintf( value, sizeof( value ), "%d", cury );
			FMset_var( vname, value );

			strcpy( vname, "_topy" );
			snprintf( value, sizeof( value ), "%d", topy );
			FMset_var( vname, value );

			i = 0;
			for( cp = firstcol; cp && cp != cur_col; cp = cp->next )
				i++;
			strcpy( vname, "_coln" );
			snprintf( value, sizeof( value ), "%d", i );
			break;

     default:
       FMmsg( E_PARMOOR, inbuf );   /* error message */
       break;
    }       /* end switch */

	if( *value )
	{
		TRACE( 2, "getval: setting: %s=%s\n", vname, value );
		FMset_var( vname, value );
	}
	else
		fprintf( stderr, "getval: value was emppty; nothing set for %s\n", vname );
  }                                       /* end if parameter entered */
}          /* fmgetval */
