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

#include "../base/symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
****************************************************************************
* TFM
*
*   Mnemonic: FMflush
*   Abstract: This routine is responsible for flushing the output buffer
*             to the output file and resetting the output buffer pointer.
*				The return value is to be consistent with other versions and
*				is meaningless in the context of tfm.
*   Parms:    None.
*   Date:     15 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 29 Jun 1994 - To convert to rfm.
*              6 Dec 1994 - To reset font even if nothing to write.
*              7 Dec 1994 - To prevent par mark at page top if in li list
*             14 Dec 1994 - To prevent par mark at top if in di list
*              6 Dec 1996 - To convert for hfm
*              9 Nov 1998 - To remove </font> - Netscape seems unable to
*	                    handle them properly
*             22 Jan 2000 - To strncmp for color rather than strcmp
*             21 Mar 2001 - Revisiting an old friend; tfm conversion
*			07 Jul 2013 - Support centered text
*				17 Jul 2016 - Bring prototypes into modern era.
****************************************************************************
*/
extern int FMflush( void )
{
	char *xp;
	char *dp;               /* pointers to walk through buffers */
	int len;                /* length of the output string */
	char fbuf[2048];        /* buffer to build flush strings in */
	char fmt[255];          /* dynamic sprintf format string */
	char *xterm = NULL;     /* expanded di_term (must make % into %%) */
	int i;             /* the Is have it? */
	int li = 0;        /* list item indention */
	int iv = 0;        /* indention value */

	iv = (lmar/7);      /* basic indent value converted to char */

	if( lilist && lilist->xpos == 0 )		/* list item */
	{
		iv -= 2;
		li = lilist->xpos;
		lilist->xpos = 2;
		sprintf( fmt, "%%%ds%c %%s", iv, lilist->ch );   /* add current term */
	}
	else
	if( flags2 & F2_DIBUFFER )        /* need to back off some if term in buff */
	{
		iv -= dlstack[dlstackp].indent/7; 
		xterm = (char *) malloc( (strlen( di_term ) * 2) + 1 );
		for( xp = xterm, dp = di_term; *dp; )
		{
			*xp = *dp++;
			if( *xp++ == '%' )
			*xp++ = '%';
		}
		*xp = 0;

		sprintf( fmt, "%%%ds%s%%s", iv, xterm );   /* add current term */
		free( xterm );
	}
	else
	if( flags2 & F2_CENTER )
	{
		int extra;
		if( (extra = ((linelen/7)-iv) - strlen( obuf ) ) > 2 )
			sprintf( fmt, "%%%ds%%s", iv + (extra/2) );
		else
			sprintf( fmt, "%%%ds%%s", iv );
	}
	else
		sprintf( fmt, "%%%ds%%s", iv );

	TRACE( 2, "flush: flags=%02x fmt = (%s) buf=(%s)\n", flags, fmt, obuf );
	flags2 &= ~F2_DIBUFFER;

	if( optr == 0 ) 		 /* nothing to do so return */
		return 0;

	/* copy only the cached portion of the output buffer */
	if( ! (flags2 & F2_CENTER) )
		FMjustify( );                          /* justify if on, and not centering */

	sprintf( fbuf, fmt, " ", obuf ); 
	AFIwrite( ofile, fbuf );   /* write the text out to the file */
	
	*obuf = EOS;             /* reset */
	optr = 0;

	return 0;
}
