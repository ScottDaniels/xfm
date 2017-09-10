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
*********************************************************************************************************
*
*  Mnemonic: FMjustify
*  Abstract: This routine is responsible for sending out the current buffer
*            with the proper justification command and related parameters.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     12 November 1992
*  Author:   E. Scott Daniels
*
*  Modified:	04 Dec 1992 - To call flush if spaces are 0
*            	10 Dec 1992 - To use AFI routines for ansi compatability
*				06 Apr 1994 - To take advantage of linelen being points now
*				20 Nov 2002 - Added kludgey way to set colours, provides some support, but not 
*					as finite of control as in hfm.
*				10 Apr 2007 - Memory leak cleanup 
*				09 Mar 2013 - Corrected bug with size and added super/subscript support
*				07 Jul 2013 - Allows for setting colour on substrings. 
*				24 Jul 2013 - Adjusts cury if needed after an eject that might have inserted something.
*			17 Jul 2016 - Bring decls into the modern world.
***********************************************************************************************************
*/
extern void FMjustify( void )
{
 	char jbuf[1024];    /* initial work buffer */
 	char jjbuf[1024];    /* work buffer */
	int 	i;		/* loop index */
	int	j;
	int	size = 0;		/* info about a format segment */
	char	*font = NULL;
	char	*colour = NULL;
	int	start = 0;
	int	end = 0;
	int	ydisp = 0;
	int	largest = 0;		/* largest font size on the line */
	int	things = 0;		/* number of tripples in the just stack */
	int	blanks = 0;		/* number of blanks in the string */
	int	first = 1;		/* true if working with first block from the list */

 	FMfmt_end( );		/* mark the last format block as ending here */

	largest = FMfmt_largest( );
 
   	cury += largest + textspace;    /* move to the next y position */
   	if( flags & DOUBLESPACE )        /* if in double space mode ...    */
    		cury += textsize + textspace;   /* then skip down one more */

	TRACE(2,  "just: lmar=%d topy=%d cury=%d boty=%d cn_space=%d obuf=(%s)\n", lmar, topy, cury, boty, cn_space,  obuf );
  	if( cury > boty )               /* are we out of bounds? */
	{
 		PFMceject( );       			/* move to next column */
		if( cury != topy )			/* possible that something was injected, we must advance cury again */
			cury += largest + textspace;
	}

	sprintf( jbuf, "%d %d moveto\n", lmar, -cury );  	/* create moveto */
	AFIwrite( ofile, jbuf );      				/* write the move to command or x,y for cen */

 	while( FMfmt_pop( &size, &font, &colour, &start, &end, &ydisp ) )	/* run each fmt block put on the list */
 	{		 															/* to generate (string) (fontname) ydisplacement fontsize tuples while counting blanks */
		if( end - start >= 0 )											/* dont do a fmt that has no text with it */
		{
			for( j = 0, i = 0; i <= end - start; i++ )
			{
				if( obuf[start+i] == ' ' )
					blanks++;
				else
					if( obuf[start+i] == '^' )
						i++;
				jbuf[j++] = obuf[start+i];
			}

			while( jbuf[j-1] == ' ' )		/* trim trailing blanks */
			{
				j--;
				blanks--;
			}
			jbuf[j] = 0;

			TRACE( 2, "justif: lmar=%d topy=%d cury=%d ydisp=%d jbuf=(%s)\n", lmar, topy, cury, ydisp, jbuf );
			if( i )
			{
				sprintf( jjbuf, "(%s) (%s) %d %d [ %s ] ", font, jbuf, ydisp, size, colour ? colour : "-1" );    /* no trail space  */
   				AFIwrite( ofile, jjbuf );               /* output the information */
				things++;
			}
			else
			{
				if( colour )
				{
					sprintf( jjbuf, "(%s) () %d %d [ %s ] ", font, ydisp, size, colour ? colour : "-1" );    /* must still do something to change colour */
   					AFIwrite( ofile, jjbuf );
					things++;
				}
			}
			first = 0;			/* no longer working with the head */
		}
		else
		{
			sprintf( jjbuf, "(%s) () %d %d [ %s ] ", font, ydisp, size, colour ? colour : "-1" );    /* must still do something to change colour */
			AFIwrite( ofile, jjbuf ); 
			things++;
		}

		if( font )
			free( font );
		if( colour )
			free( colour );
	}

	if( font )					/* these end up being default values even if nothing on fmt stack, so we must free */
		free( font );
	if( colour )
		free( colour );

	if( things )		/* actually wrote something, finish it off */
	{
		sprintf( jbuf, " %d %d %d just\n", things, linelen, blanks ? blanks : 1 );
		AFIwrite( ofile, jbuf );
	}

	optr = 0;		/* reset the output buffer */
	*obuf = 0;
	FMfmt_add( );		/* add the current font back to the list */

	if( cur_col->flags & CF_TMP_MAR ) {			// check for temp margin reset
		if( cury > cur_col->revert_y ) {
			lmar = cur_col->olmar;
			linelen = cur_col->olinelen;
			cur_col->flags &= ~CF_TMP_MAR;
		}
	}

	if( cn_space && cn_space + cury > boty )
	{
		TRACE(2,  "just: col-notes call lmar=%d cury=%d topy=%d boty=%d cn_space=%d obuf=(%s)\n", lmar, cury, topy, boty, cn_space,  obuf );
		FMcolnotes_show( 0 );			/* cause the column notes to be put in before eject */
		cn_space = 0;
	}
}
