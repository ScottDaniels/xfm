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
*   Modified: 12-03-88 to space down topmar lines if page eject flag set.
*			12-08-88 not to space down past topmar if at start of col.
*			 3 Feb 1989 - Not to reset figure number if not paragraph
*                          numbering.
*			 7 Jul 1989 - To support variable expansion in headers.
*			23 Apr 1991- To add header left margin support.
*			5 May 1992- To support postscript conversion
*			 7 Nov 1992- To skip before and after from header block
*			23 Nov 1992- To call ccol to ensure 5 lines remain in column
*	      	13 Oct 2001 - To use the formatting blocks properly.
*	  	    27 Nov 2001 - To calc ccol value based on skip values
*			25 Jun 2013 - Added ability to short out to get end notes if 
*					forcing a new page/column.
*			05 Mar 2014 - checks remaining space and forces an eject if
*					we cannot get header, lead/trail space and 2 lines
*					in the current column. Fixed bug caused by flushing 
*					too early.
*			18 Dec 2015 - Prevent an extra page/col eject if flush does
*					one too.
*			03 Jul 2016 - Add support for getting better section info for
*					.gv command.
*			17 Jul 2016 - Bring decls into the modern world.
*			12 Aug 2017 - Enable fracion before/after spacing and required
*					space after.
*			23 Nov 2017 - Fix bug with alignment of header in y direction at
*					column top.
**************************************************************************
*/
/*
	pushes the .hX command back if we need to column note or somesuch.
	Returns true if we had to push the command back on to make way for
	end of colum stuff.
*/
static int push_cmd( int level )
{
	char buf[128];

	TRACE( 3, "header: checking for shortout: cnspace=%d\n", cn_space );
	if( cn_space > 0 )						/* there's a column note */
	{
		snprintf( buf, sizeof( buf ), ".h%d", level );
 		AFIpushtoken( fptr->file, buf );  	

		return 1;
	}

	return 0;
}

extern void FMheader( struct header_blk *hptr )
{
	double before;
	int	short_out = 0;	/* set if we had to push the header command back on and defer processing */
	int len;            /* len of parm token */
	int j;              /* index variables */
	int i;
	char buf[2048];     /* buffer to build stuff in */
	char stxt[2048];	// section string to save for .gv
	char *ptr;          /* pointer to header string */
	int oldlmar;        /* temp storage for left mar value */
	int oldsize;        /* temp storage for text size value */
	char *oldfont;      /* pointer to old font sring */
	int	need_eject = 0;	// set to true if we need to eject for any reason

	FMflush();

																// compute total space needed before, after, and reserved
	before = ((double) (textsize + textspace)) * hptr->bskip;	// space before (lines, or fractional lines)
	i = before;
	i += (textsize + textspace) * (hptr->askip);				// space after
	i += textspace +  hptr->size;           					// add in header itself
	i += (hptr->required);										// space required to exist after (already in points)

	if( (i + cury) > (boty - cn_space) )						// if required space is more than what's left force a break
	{
		need_eject  = 1;
		before = 0;											// when at top, no before space needed
		TRACE( 2, "header: forcing eject: i+cury=%d  boty-cn_space=%d\n", i + cury, boty - cn_space );
	} else {
		if( cury <= topy ) {
			before = 0;											// when at top, no before space needed
		}
	}
	TRACE( 2, "header:  level=%d skip=%.1f/%.1f before=%.2f ejecting=%d require=%d needed=%d cury=%d boty=%d cn_space=%d\n", hptr->level, hptr->bskip, hptr->askip, before, need_eject, hptr->required, i, cury, boty, cn_space );

	
	if( (hptr->flags & HEJECTP) && (cur_col != firstcol || cury > topy) )	// must check for new page first as it trumps
	{
		short_out = push_cmd( hptr->level );		// if needed, push the header command back on the stack to execute after ejecting
		if( cury != topy )		 					// flush didn't see us at the end of page, so it didn't eject, we must
		{											// this check is legit as if HEJECTP is set we eject even if flush ejected too
			if( short_out  )						// if column notes waiting, we must dump those first
			{
				FMcolnotes_show( 0 );			
				cn_space = 0;
			}
			else
				FMpflush( );	// and finally eject the page leaving short_out SET!
		before = -textsize;
		}
	} else {
		if( need_eject || (hptr->flags & HEJECTC) && cury != topy ) 			// either at end or column eject
		{
			short_out = push_cmd( hptr->level );					// push the .hn command back if there is end column stuff
			
			if( ! FMflush() )							// safe to flush, and if flush didn't eject, we must
			{
				if( short_out  )						// if column notes waiting, we must dump those first
				{
					FMcolnotes_show( 0 );			
					cn_space = 0;
				}
				else
					PFMceject( );	// and finally the eject; we leave short_out SET as we pushed our .hn command so we need to return to pick it up again
		before = -textsize;
			}
		} else {
			FMflush( );		// nothing special, just flush
		}
	}

	if( short_out )			// if we pushed the header command so as to allow end of col/page things, get out early
		return;


	TRACE( 2, "header: didn't short out: cury=%d lmar=%d curfont=%s textsize=%d before=%.0f\n", cury, lmar, curfont, textsize, before );
	cury += before;	 						// skip before if needed (already points), and zero if we ejected
	pnum[(hptr->level)-1] += 1;     		 /* increase the paragraph number */

									/* preserve everything and set up header font etc */
	TRACE( 2, "header: preserving: cury=%d lmar=%d curfont=%s textsize=%d\n", cury, lmar, curfont, textsize );
	oldlmar = lmar;                  /* save left margin */
	oldfont = curfont;               /* save current text font */
	oldsize = textsize;              /* save old text size */

	if( hptr->level < 4 );           /* unindent if level is less than 4 */
		lmar = cur_col->lmar + hptr->hmoffset;
	textsize = hptr->size;           /* set text size to header size for flush */
	curfont = hptr->font;            /* set font to header font for flush */
	FMfmt_add( );

	if( hptr->level == 1 && (flags & PARA_NUM) && (flags3 &  F3_FIG_SECT_NUM)  ) {
		fig = 1;                          		// restart figure numbers, but only if numbering headers and turned on
	}

	for( i = hptr->level; i < MAX_HLEVELS; i++ )
		pnum[i] = 0;                    	   /* zero out all lower p numbers */

	if( flags & PARA_NUM )         			 /* number the paragraph if necessary */
	{
		char*	hnum;

		hnum = FMmk_header_snum( hptr->level );
		FMaddtok( hnum, strlen( hnum ) );
		free( hnum );
	}

	stxt[0] = 0;
	while( (len = FMgetparm( &ptr )) > 0 )    /* get next token in string */
	{
		if( hptr->flags & HTOUPPER )
			for( i = 0; i < len; i++ )
				ptr[i] = toupper( ptr[i] );
		if( (strlen( stxt ) + len) < sizeof( stxt ) - 1 ) {
			strcat( stxt, ptr );
		}
		FMaddtok( ptr, len );
	}
	FMmk_header_stxt( stxt );

				/* the call to FMtoc must be made while the header is in obuf */
	if( hptr->flags & HTOC )       /* put this in table of contents? */
		FMtoc( hptr->level );         /* put entry in the toc file */

	if( hptr->level < 4 )
		FMflush( );                      /* writeout the header line */

	lmar = oldlmar;                  /* restore left margin value */
	textsize = oldsize;              /* restore text size */
	curfont = oldfont;               /* restore font too */
	TRACE( 3, "header: restored: lmar=%d curfont=%s textsize=%d\n", lmar, curfont, textsize );
	FMfmt_add( );			/* setup for next buffer */

	if( hptr->level < 4 )      /* indent if not header level 4 */
	{
		//cury += (textsize + textspace) * (hptr->skip%10); 		/* skip after */
		cury += (textsize + textspace) * hptr->askip;				// skip after
		if( cury % 2 )
			cury++;		/* ensure its even after a header */
		for( optr = 0, i = 0; i < hptr->indent; i++, optr++ )	/* indent */
			obuf[optr] = BLANK;
		obuf[optr] = EOS;                       /* terminate incase flush called */
		osize = FMtoksize( obuf, hptr->indent );  /* set size of blanks */
	}
	else       /* for level 4 place text next to the header */
	{
		obuf[optr] = BLANK;
		optr++;
		obuf[optr] = EOS;
		osize = FMtoksize( obuf, strlen( obuf ) );  /* set size of stuff there */
	}
}                    /* FMheader */
