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
*  Mnemonic: FMflush
*  Abstract: This routine is responsible for sending out the current buffer
*            with the proper formatting and no justificaiton.
*  Parms:    None.
*  Returns:  true if we eject the page; false (0) otherwise.
*  Date:     12 November 1992
*  Author:   E. Scott Daniels
*
*  Modified: 10 Oct 2001 - Converted to use format blocks
*	     20 Nov 2002 - Added kludgey way to set colours, provides some support, but not 
*			as finite of control as in hfm.
*		10 Apr 2007 - Memory leak cleanup 
*		09 Mar 2013 - Corrected bug that was causing font size not to change
*			when needed in the middle of the line. Super/subscript changes.
*		21 Mar 2013 - eliminated some uneeded blanks in ps output.
*		07 Jul 2013 - Allow for setting colours on substrings in the line. 
*		11 Feb 2014 - Added at end flag for foot notes on last page
*		18 Dec 2015 - now returns true if it ejected the page.
*		01 Jan 2016 - Added support for floating margins.
*****************************************************************************
*/
int FMflush( )
{
	int 	i;			/* loop index */
	int		j;
	int		size;		/* info about a format segment */
	char	*font;
	char	*colour;
	int		start;
	int		end;
	int		largest;			/* largest font size on the line */
	int		things = 0;			/* number of tripples in the stack */
	int		first = 1;			/* true if working with first block from the list */
	int		ydisp = 0;			/* super/subscript y displacement */
	int		last_cury;			/* cury before inc -- incase we need to ceject */
 	char 	jbuf[1024];    		/* initial work buffer */
 	char 	jjbuf[1024];		/* work buffer */
	int		ejected = 0;		// set to true for return if we ejected the page

	if( optr == 0 )
		return ejected;

 	FMfmt_end( );					/* mark the last format block as ending here */

	largest = FMfmt_largest( );		/* figure out largest point size for y positioning */
 
	last_cury = cury;
   	cury += largest + textspace;			/* move to the next y position */
   	if( flags & DOUBLESPACE )				/* if in double space mode ...    */
    	cury += textsize + textspace;   /* then skip down one more */

  	if( cury > boty )               	/* are we out of bounds? */
	{
		cury = last_cury;
		ejected = 1;
 		PFMceject( );       					/* move to next column */
		if( cury != topy )
			cury += largest + textspace;		/* in case something was insterted at the top of col */
 	}

	snprintf( jbuf, sizeof( jbuf ), "%d %d moveto\n", lmar, -cury );  /* create moveto */
	AFIwrite( ofile, jbuf );      /* write the move to command or x,y for cen */

	TRACE(2,  "flush: lmar=%d cury=%d topy=%d boty=%d cn_space=%d obuf=(%s)\n", lmar, cury, topy, boty, cn_space,  obuf );

 	while( FMfmt_pop( &size, &font, &colour, &start, &end, &ydisp ) )  /* run each formatting block put on the list */
 	{
		if( start <= end )
		{
			for( j = 0, i = 0; i <= end - start; i++ )		 /* generate (string) (fontname) ydisplacement fontsize */
			{
				if( obuf[start+i] == '^' )
					i++;
				jbuf[j++] = obuf[start+i];
			}
			jbuf[j] = 0;
	
			snprintf( jjbuf, sizeof( jbuf ), "(%s) (%s) %d %d [ %s ] ", font, jbuf, ydisp, size, colour ? colour : "-1" );   
			TRACE( 2, "flush: jbuf=(%s) start=%d end=%d size=%d font=%s colour=%s (%s)\n", jbuf, start, end, size, font, colour ? colour : "undefined", textcolour ? textcolour : "unset" );
   			AFIwrite( ofile, jjbuf );               				/* output the information */
			things++;
		}
		else
		{
			if( colour )					/* colour was reset, but no string to write we still must slide in something */
			{
				things++;
				snprintf( jjbuf, sizeof( jbuf ), "(%s) () %d %d [ %s ] ", font, ydisp, size, colour ? colour : "-1" );   
   				AFIwrite( ofile, jjbuf );
			}
		}

		first = 0;							/* no longer working with the head */
		if( font )
			free( font );
		if( colour )
			free( colour );
	}

	if( font )					/* these end up being default values even if nothing on fmt stack, so we must free */
		free( font );
	if( colour )
		free( colour );
 
	if( things <= 0 )
	{
		FMfmt_add( );		/* add the current font back to the list */
		return ejected;
	}

   	if( flags2 & F2_CENTER )      /* set up for center command */
		snprintf( jbuf, sizeof( jbuf ), "%d %d cent\n", linelen, things );
	else
   	if( flags2 & F2_RIGHT )      /* set up for right alignment command */
		snprintf( jbuf, sizeof( jbuf ), "%d %d right\n", linelen, things );
	else	
		snprintf( jbuf, sizeof( jbuf ), "%d splat\n", things );

	AFIwrite( ofile, jbuf );

	*obuf = 0;
	optr = 0;			/* reset the output buffer */
	FMfmt_add( );		/* add the current font back to the list */

	if( cur_col->flags & CF_TMP_MAR ) {
		if( cury > cur_col->revert_y ) {
			lmar = cur_col->olmar;
			linelen = cur_col->olinelen;
			cur_col->flags &= ~CF_TMP_MAR;
		}
	}

	if( cn_space && cn_space + cury >= boty )
	{
		cn_space = 0;
		FMcolnotes_show( 0 );			/* cause the column notes to be put in before eject */
	}

	return ejected;
}
