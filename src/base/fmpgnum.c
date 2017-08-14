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
****************************************************************************
*
*   Mnemonic:  FMpgnum
*   Abstract:  This routine sets the page number flag as requested by the user.
*			.pn off
*			.pn on [center] [noline] [roman] [tocfmt=string] [fmt=string] [starting-pgnum]
*
*			Format strings are something like -%d- A-%d. To include spaces the 
*			_whole_ parm must be back quoted:  `f=Page %d`.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Mod:		29 Aug 2011 -- Added center, format and noline support
*
*				17 Jul 2016 - Changes for better prototype generation.
*				11 Aug 2017 - reset format string on each call
***************************************************************************
*/

/*
	Given an integer 1 - 3999, generate the roman numeral string that corresponds to the 
	integer. Returns the buffer with the value that the caller must free.
*/
extern char* FMi2roman( int x  ) {
	int		power = 1000;	// loop backwards through powers of 10
	char	buf[128];
	char*	so;				// special output
	char*	ls;				// left side character: x in xi
	char*	rs;				// right side character: i in xi
	int		spec;			// value which generates a special output (e.g. 9 or 900)
	int		mp;				// midpoint of the group
	int		y;
	int		i;

	buf[0] = 0;

	if( x > 3999 ) {
		return strdup( buf );
	}

	while( power > 0 ) {
		mp = power * 5;
		spec = power * 9; 

		switch( power ) {
			case 1000:
				so = "";
				ls = "";
				rs = "m";
				break;

			case 100:
				so = "cm";
				ls = "d";
				rs = "c";
				break;

			case 10:
				so = "xc";
				ls = "l";
				rs = "x";
				break;

			case 1:
				so = "ix";
				ls = "v";
				rs = "i";
				break;
		}

		y = power * (x/power);
		if( y > 0 ) {
			x -= y;
			if( y == spec ) {		// special case 
				strcat( buf, so );
			} else {
				if( y >= mp ) {
					strcat( buf, ls );		// add the midpoint (D, L, or V)
					y = (y - mp)/power;
				} else {
					if( y == mp - power ) {		// special subtraction at midpoint
						strcat( buf, rs );		// characters are reversed
						strcat( buf, ls );
						y = 0;
					} else {
						y /= power;
					}
				}

				for( i = 0; i < y; i++ ) {
					strcat( buf, rs );			// add 0-3 Is, Xs, Cs
				}
			}
		}

		power /= 10;
	}	

	return strdup( buf );
}

extern void FMpgnum(  void )
{
 	char *buf;          /* pointer at the token */
 	int len;            /* length of parameter entered */
	char	 *fmt = NULL;

	flags = flags | PAGE_NUM;							/* default to on */
	flags3 &= ~(F3_PGNUM_CENTER | F3_ROMAN_PN);			/* default to off */
	flags3 |= F3_RUNOUT_LINES;							/* default to lines */
	if( pgnum_fmt != NULL ) {
		free( pgnum_fmt );
		pgnum_fmt = NULL;
	}

	while( (len = FMgetparm( &buf )) > 0 )
	{
		switch( *buf )
		{
			case 'o':
				if( strncmp( "off", buf, 3 ) == 0 )
				{
					flags = flags & (255 - PAGE_NUM);       /* turn off and short circuit out of here */
					return;
				}

				break;

			case 'c':									/* assume center */
					flags3 |= F3_PGNUM_CENTER;	
					break;

			case 'f':
					fmt = strchr( buf, '=' );
					if( fmt )
					{
						if( pgnum_fmt )
							free( pgnum_fmt );
						pgnum_fmt = strdup( fmt + 1 );
					}
					break;

			case 'n':									/* assume noline */
					flags3 &= ~F3_RUNOUT_LINES; 
					break;

			case 'r':									// assume roman numerals
					flags3 |= F3_ROMAN_PN;
					break;

			case 't':
					fmt = strchr( buf, '=' );
					if( fmt )
					{
						if( toc_pn_fmt )
							free( toc_pn_fmt );
						toc_pn_fmt = strdup( fmt + 1 );
					}
					break;

			default:
					page = atoi( buf );           		/* set the next page number */
					break;
		}

	}

}
