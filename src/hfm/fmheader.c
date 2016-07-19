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


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
*************************************************************************
*
*   Mnemonic: FMheader
*   Abstract: This routine is responsible for putting a paragraph header
*             into the output buffer based on the information in the
*             header options buffer that is passed in.
*   Parms:    hptr - Pointer to header information we will use
*   Returns:  Nothing.
*   Date:     1 December 1988
*   Author:   E. Scott Daniels
*
*   Modified: 30 Jun 1994 - Conversion to rfm (not much changed)
*             15 Dec 1994 - To seperate skip before/after numbers.
*			18 Jul 2016 - Add consistent, and sometimes modern, prototypes.
**************************************************************************
*/
extern void FMheader( struct header_blk *hptr )
{
	int len;            /* len of parm token */
	int j;              /* index variables */
	int i;
	char buf[80];       /* buffer to build paragraph number in */
	char *tbuf;         /* pointer at temporary buffer to hold out buf string */
	char *ptr;          /* pointer to header string */
	int oldlmar;        /* temp storage for left mar value */
	int oldsize;        /* temp storage for text size value */
	int oldlen;         /* temp storage for old line length */
	char *oldfont;      /* pointer to old font sring */
	int  skip;          /* number of lines to skip before continuing */

TRACE( 2, "header: level=%d saving: len=%d lmar=%d textsize=%d\n", hptr->level, linelen, lmar, textsize );
	FMflush( );          /* flush the current line */

	pnum[(hptr->level)-1] += 1;      /* increase the paragraph number */
	for( i = hptr->level; i < MAX_HLEVELS; i++ )
	pnum[i] = 0;                       /* zero out all lower p numbers */

	sprintf( buf, "<h%d>", hptr->level );    /* build html tag at runtime */
	strcat( obuf, buf );
	optr += 4;                     /* add header tag and scoot past it */


	if( hptr->level == 1 && (flags & PARA_NUM) )
		fig = 1;                          /* restart figure numbers */

	if( flags & PARA_NUM )          /* number the paragraph if necessary */
	{
		sprintf( buf, "%d.%d.%d.%d", pnum[0], pnum[1], pnum[2], pnum[3] );
		for( i = 0, j = 0; j < hptr->level; j++, i++ )
		{
	  		for( ; buf[i] != '.' && buf[i] != EOS; i++, optr++ )
	   			obuf[optr] = buf[i];          /* copy in the needed part of the number */

	  		if( buf[i] != EOS )
	   			obuf[optr++] = '.';          /* put in the dot */
		}                             /* end for j */

		obuf[optr++] = BLANK;          /* seperate with a blank */
	}

	while( (len = FMgetparm( &ptr )) > 0 )    /* get next token in string */
	{
		for( i = 0; i < len; i++, optr++ )
		{
	 		if( hptr->flags & HTOUPPER )     /* xlate to upper flag for this header */
	  			obuf[optr] = toupper( ptr[i] );
	 		else
	  			obuf[optr] = ptr[i];           /* copy in the buffer */
		}
		obuf[optr++] = BLANK;            /* add an additional blank */
	}
	obuf[optr] = EOS;                  /* terminate the header string */

	
	/* the call to FMtoc must be made while the header is in obuf */
	if( hptr->flags & HTOC )         /* put this in table of contents? */
		FMtoc( hptr->level );           /* put entry in the toc file */

#ifdef NOT_IN_HFM
	oldlen = linelen;                /* save the line length */
	oldlmar = lmar;                  /* save left margin */
	oldfont = curfont;               /* save current text font */
	oldsize = textsize;              /* save old text size */

	textsize = hptr->size / 4;       /* set text size to header size for flush */
	curfont = hptr->font;            /* set font to header font for flush */
#endif

	sprintf( buf, "</h%d>", hptr->level );  /* end of header */
	strcat( obuf, buf );
	optr += strlen( buf );
	FMflush( );                      /* writeout the header line */

#ifdef NOT_IN_HTM
	linelen = oldlen;                /* restore line length */
	lmar = oldlmar;                  /* restore left margin value */
	textsize = oldsize;              /* restore text size */
	curfont = oldfont;               /* restore font too */
#endif

	for( optr = 0, i = 0; i < hptr->indent; i++, optr++ )   /* indent */
		obuf[optr] = BLANK;
	obuf[optr] = EOS;                          /* terminate incase flush called */
	osize = hptr->indent;                      /* set size of blanks */
}
