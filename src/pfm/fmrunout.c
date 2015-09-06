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
****************************************************************************
*/
void FMrunout( int page, int shift )
{
	char *cmd = "show";      /* default to show command with header text info */
	int y;                   /* spot for writing the footer/page number info */
	int x;                   /* either far left or right depending on shift */
	int len;                 /* length of string for output to write buffer */
	char *moveto = "moveto"; /* moveto command necessary for show command, but not shifting right */
	char buf[2048];            /* output buffer */
	char	ubuf[1024];		/* if user format string for number, build it here */


	if( rhead == NULL && rfoot == NULL && (flags & PAGE_NUM) == 0 )
		return;               /* nothing to do - get out */

	FMsetfont( runfont, runsize );   /* set up the font in the output */
	FMfmt_add( );

	if( flags3 & F3_PGNUM_CENTER )
		x = (pagew/2)-4;
	else
	{
		if( shift == TRUE )    /* if we need to shift, use right instead of show */
		{
			cmd = "rightxy";
			moveto = " ";     /* moveto not necessary when using right */
			x = pagew - firstcol->lmar;        /* x must be far right for the right command */
		}
		else
			x = firstcol->anchor;    /* x is the firstcolumn's left margin */
	}

	if( rhead != NULL )   /* if there is a running header defined */
	{
		int hx = x;								// x to use for header
		int osize = textsize;
		char *ofont;

		ofont = curfont;
		curfont = runfont;
		textsize=10;
		hx = pagew - firstcol->lmar - FMtoksize( rhead, strlen( rhead ) );
		textsize = osize;
		curfont = ofont;


		if( topy - 6 - textsize > 0 )
			sprintf( buf, "%d %d %s (%s) %s\n", hx, -(topy - 6 - textsize), moveto, rhead, cmd );
		else
			sprintf( buf, "%d %d %s (%s) %s\n", hx, topy - 6 - textsize, moveto, rhead, cmd );
		AFIwrite( ofile, buf );             /* send out the header */
		if( flags3 & F3_RUNOUT_LINES )
		{
			if( topy - textsize - 4 > 0 )
				sprintf( buf, "%d %d moveto %d %d lineto\n", firstcol->lmar, -(topy - textsize -4), pagew - firstcol->lmar, -(topy - textsize - 4) );
			else
				sprintf( buf, "%d %d moveto %d %d lineto\n", firstcol->lmar, topy - textsize -4, pagew - firstcol->lmar, topy - textsize - 4 );
			AFIwrite( ofile, buf );       				/* seperate header from text w/ line */
		}
	}

 	y = boty + 27;  					/* set up the spot for write */

	if( flags3 & F3_RUNOUT_LINES && ( rfoot != NULL  ||  flags & PAGE_NUM) )  /* draw line to seperate */
	{
		TRACE(1, "runout: anchor=%d y=%d w=%d/%d \n", firstcol->anchor, y, pagew-firstcol->lmar, MAX_X );
		sprintf( buf, " %d %d moveto %d %d lineto stroke\n", firstcol->anchor, -(y-10), pagew - firstcol->lmar, -(y-10) );
		AFIwrite( ofile, buf );
	}

	if( rfoot != NULL )   /* if there is a running footer defined */
	{
		sprintf( buf, "%d %d %s (%s) %s\n", x, -y, moveto, rfoot, cmd );
		AFIwrite( ofile, buf );               /* send out the footer */
		y += 12;
		/*y += textsize + textspace;  */          /* bump up for page number */
	}

	if( flags & PAGE_NUM )    /* if we are currently numbering pages */
	{
		snprintf( ubuf, sizeof( ubuf ), pgnum_fmt ? pgnum_fmt : "Page %d", page );		/* format user string, or default if not set */
		//snprintf( buf, sizeof( buf ), "%d %d %s (%s) %s\n", x, -y, moveto, ubuf, cmd );
		snprintf( buf, sizeof( buf ), "%d %d %s (%s) %s\n", x, -y, moveto, ubuf, cmd );

		AFIwrite( ofile, buf );             /* send out the page number */
	} 
}
