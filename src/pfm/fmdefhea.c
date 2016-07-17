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
#include "pfmproto.h"

/*
****************************************************************************
*
*  Mnemonic: FMdefheader
*  Abstract: This routine parses the .dh command which allows the user to
*            define the various attributes for each of the header levels.
*            If the skip value is set to 0 then no skipping is done before
*            or after the header.
*            .dh <level> [f=<fontname>]        supply the font for header
*                        [p=<size.[p|i]]       size of header text
*                        [s=<skip value>]      spaces skipped before/after
*                        [i=<indent>]          indention of first line after
*                        [u=on|off]            translate header to upper case
*                        [t=on|off]            put in toc
*                        [m=<space>[p|i]]      offset from hmar location
*                        [e=p(page)|c(olumn)|n(one) ]  eject type
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     7 November 1992
*  Author:   E. Scott Daniels
*
*  Modified: 11 Jan 1993 - To allow toc, toupper, ejectp and ejectc flags
*                          to be set/reset.
*            17 Aug 1994 - To allow for header margin relative offset value
*            15 Dec 1994 - To allow skip values only up to 99.
*		     24 Oct 2007 - Corrected bug in t= parsing
*			17 Jul 2016 - Bring decls into the modern world.
*****************************************************************************
*/
extern void FMdefheader( void )
{
	char *buf;                /* pointer at next parm to parse */
	int level;                /* current level we are working with */
	int len;                  /* length of the parameter read */
	struct header_blk *hptr;  /* pointer at current block */

	if( FMgetparm( &buf ) <= 0 )   /* if no parameters entered */
	{
		return;
	}

	level = (atoi( buf )) - 1;   /* convert level to numeric */
	if( level < 0 || level >= MAX_HLEVELS )  /* if out of range */
	{
		FMmsg( E_PARMOOR, inbuf );
		return;
	}

	hptr = headers[level];   /* point right at what we are changing */

	while( (len = FMgetparm( &buf )) > 0 )   /* while parameters left */
	{
		switch( buf[0] )       /* act on the command */
		{
			case 'e':        /* set eject type p==page, c==column, anything else */
			case 'E':        /* turns off all ejects */
				hptr->flags &= ~(HEJECTP + HEJECTC);  /* initially turn them both off */
				if( len > 2 )
					if( toupper( buf[2] ) == 'P' )    /* user wants page eject */
						hptr->flags |= HEJECTP;   /* turn on the page eject flag */
					else
						if( toupper( buf[2] ) == 'C' )   /* user wants column eject */
							hptr->flags |= HEJECTC;         /* so turn it on */
				break;

			case 'f':        /* set font for this level */
			case 'F':
				if( len > 2 )  /* if more than f= entered */
				{
					free( hptr->font );   /* free previous font buffer */
					hptr->font = (char *) malloc( (unsigned) len );  /* get new buffer */
					strncpy( hptr->font, &buf[2], len-2 );   /* copy new font name */
					hptr->font[len-2] = EOS;      /* terminate the string */
				}
				break;
		
			case 'i':        /* set indent value */
			case 'I':
				hptr->indent = atoi( &buf[2] );   /* convert to integer */
				if( hptr->indent < 0 || hptr->indent > 50 )
					hptr->indent = 5;     /* dont let user go crazy */
				break;

			case 'm':
			case 'M':        /* offset from header margin for this level */
				hptr->hmoffset = FMgetpts( &buf[2], len-2 );  /* cvt input to points */
				break;

			case 'p':        /* set the point size */
			case 'P':
				hptr->size = atoi( &buf[2] );   /* convert to integer */
				if( hptr->size < 0 || hptr->size > 72 )
					hptr->size = 12;    /* dont let user go crazy */
				break;

			case 's':
			case 'S':        /* set skip value */
				hptr->skip = atoi( &buf[2] );   /* convert to integer */
				if( hptr->skip < 0 || hptr->skip > 99 )
					hptr->skip = 2;     /* dont let user go crazy */
				break;

			case 't':       /* set/clear toc flag */
			case 'T':
				if( len == 0  || toupper( buf[3] ) == 'Y' )  /* on or missing */
					hptr->flags |= HTOC;    /* turn on the toc flag */
				else
					hptr->flags &= ~HTOC;   /* turn off the flag */
				break;

			case 'u':
			case 'U':
				if( len == 0 || toupper( buf[3] ) == 'N' )  /* turn on or missing */
					hptr->flags |= HTOUPPER;
				else
					hptr->flags &= ~HTOUPPER;   /* turn off */
			break;
		
			default:
				break;     /* right now do nothing - later error message */
		}            
	}
}                
