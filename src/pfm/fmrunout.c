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
*****************************************************************************
*
*  Mnemonic: FMrunout
*  Abstract: This routine is responsible for writing the running matter
*            (header, footer and page number) for the current page to the
*            output file.
*  Parms:    page - The current page number
*            shift- TRUE if we should shift the text to the right margin.
*  Returns:  Nothing.
*  Date:     25 October 1992
*  Author:   E. Scott Daniels
*
*  Modified: 10 Dec 1992 - To use AFI routines for ansi compatability
*            21 Feb 1994 - To use rightxy instead of right
*			10 Oct 2007 (sd)  - Added support to anchor head/feet to left col's anchor and not lmar
*			23 Aug 2011 - Added page number centering and user formatted string.
*			23 Dec 2015 - Better management of running header via top gutter.
*			27 Dec 2015 - Fixed bug with header line (twice).
*			17 Jul 2016 - Bring decls into the modern world.
*			11 Aug 2017 - Fix centering so it works properly.
****************************************************************************
*/
extern void FMrunout( int page, int shift )
{
	struct col_blk *cb;		// pointer at the right most column block
	char *cmd = "show";      /* default to show command with header text info */
	int y;                   /* spot for writing the footer/page number info */
	int x;                   /* either far left or right depending on shift */
	int len;                 /* length of string for output to write buffer */
	int hfsize = 10;		// font size
	char *moveto = "moveto"; /* moveto command necessary for show command, but not shifting right */
	char buf[2048];          /* output buffer */
	char	ubuf[1024];		/* if user format string for number, build it here */


	if( rhead == NULL && rfoot == NULL && (flags & PAGE_NUM) == 0 )
		return;               /* nothing to do - get out */

	x = firstcol->anchor;						// seed with anchor point, move if there are multiple columns
	for( cb = firstcol; cb != NULL && cb->next != NULL; cb=cb->next ); 		// leaves cb at last column block

	if( cb == NULL )
		return;

	if( cb != firstcol )
		x = cb->lmar;						// move along as anchor is valid only for first column

	FMsetfont( runfont, runsize );   /* set up the font in the output */
	FMfmt_add( );

	if( shift == TRUE )    					// if we need to shift, use rightxy instead of show and set x to right edge
	{
		cmd = "rightxy";
		moveto = " ";     					/* moveto not necessary when using right */
		x += cb->width;						// align the string on the right at col width of last col
	}
	else
		x = firstcol->anchor;    			// not shifting, x is the left column's left margin
	
	TRACE(1, "runout: header cmd=%s  x=%d y=%d anchor=%d width=%d rmar=%d\n", cmd, x, y, firstcol->anchor, cb->width, cb->width + cb->lmar );
	if( rhead != NULL )   /* if there is a running header defined */
	{
		y = topy - top_gutter - hfsize;

		if( y > 0 ) {								// only if it will fit
			sprintf( buf, "%d %d %s (%s) %s\n", x, -y, moveto, rhead, cmd );
			AFIwrite( ofile, buf );
		}

		AFIwrite( ofile, buf );             /* send out the header */
		if( flags3 & F3_RUNOUT_LINES )
		{
			if( y > 0 ) {
				sprintf( buf, "%d %d moveto %d %d lineto\n", firstcol->lmar, -(y+4), cb->lmar + cb->width, -(y+4) );
				AFIwrite( ofile, buf );       				/* seperate header from text w/ line */
			}
		}
	}

 	y = boty + 27;  					/* set up the spot for write */

	if( flags3 & F3_RUNOUT_LINES && ( rfoot != NULL  ||  flags & PAGE_NUM) )  /* draw line to seperate */
	{
		sprintf( buf, " %d %d moveto %d %d lineto stroke\n", firstcol->anchor, -(y-10), cb->lmar + cb->width, -(y-10) );
		AFIwrite( ofile, buf );
	}

	if( rfoot != NULL )   /* if there is a running footer defined */
	{
		sprintf( buf, "%d %d %s (%s) %s\n", x, -y, moveto, rfoot, cmd );
		AFIwrite( ofile, buf );               /* send out the footer */
		y += 12;
	}

	if( flags & PAGE_NUM )    /* if we are currently numbering pages */
	{
		snprintf( ubuf, sizeof( ubuf ), pgnum_fmt ? pgnum_fmt : "Page %d", page );		/* format user string, or default if not set */
		if( flags3 & F3_PGNUM_CENTER ) {
			snprintf( buf, sizeof( buf ), "0 %d moveto\n (Times-roman) (%s) 0 10 [-1]\n 612 1 cent\n", -y, ubuf );
		} else {
			snprintf( buf, sizeof( buf ), "%d %d %s (%s) %s\n", x, -y, moveto, ubuf, cmd );
		}

		AFIwrite( ofile, buf );             /* send out the page number */
	} 
}
