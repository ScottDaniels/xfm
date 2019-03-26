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
*   Mnemonic:  FMcd
*   Abstract:  This routine parses the parameters on the .cd command and
*              builds the required column blocks.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      1 December 1988
*   Author:    E. Scott Daniels
*
*   .cd n a=v[ip] w=v[ip] i=v[ip] g=v[ip]
*          	n - The number of columns     (required)
*	   	a=  The anchor indention of first col for running footers etc. indent value used if not supplied.
*           	w=  The width of each column  (optional)
*           	i=  The indention of col 1 from edge of page (optional)
*           	g=  The gutter space between each column of each column. (optional)
*
*   Modified:  3 Feb 1989 - To issue warning if new col size is larger than
*                           the line length and left margin combination.
*             23 Oct 1992 - To convert to postscript
*              6 Apr 1994 - To accept indention and gutter sizes
*			10 Oct 2007 (sd) - Added a=v[ip] support to allow anchor setting for head/feet
*			02 Jan 2016 - Fix bug related to column block initialisation.
*			17 Jul 2016 - Bring decls into the modern world.
***************************************************************************
*/
extern void FMcd(  void )
{
	char *buf;                 /* pointer at the token */
	int gutter = DEF_GUTTER;   /* gutter value between cols */
	int i;                     /* loop index */
	int j;
	int len;
	int diff;
	int diffh;			/* calcuation of new margins */
	int diffx;
	struct col_blk *ptr;  /* pointer at newly allocated blocks */

	len = FMgetparm( &buf );    /* get the number of columns requested */
	if( len <= 0 )              /* if nothing then get lost */
		return;

	diffh = hlmar - cur_col->lmar; 	/* figure difference between col left mar */
	diff = lmar - cur_col->lmar;   	/* and what has been set using .in and .hm */
	if( lilist != NULL )           	/* if list in progress */
	diffx = lilist->xpos - cur_col->lmar;  /* then calc difference */

	while( firstcol != NULL )   /* lets delete the blocks that are there */
	{
	 	ptr = firstcol;
	 	firstcol = firstcol->next;   /* step ahead */
	 	free( ptr );                 /* loose it */
	}

	i = atoi( buf );               /* convert parm to integer - number to create */
	if( i <= 0 || i > MAX_COLS )
		i = 1;                        /* default to one column */

	firstcol = NULL;       /* initially nothing */

	for( ; i > 0; i-- )   /* create new col management blocks */
	{
	 	ptr = (struct col_blk *) malloc( sizeof( struct col_blk ) );
		if( ptr == NULL ) {
			fprintf( stderr, "abort: unable to malloc storage for a col block\n" );
			exit( 1 );
		}
		memset( ptr, 0, sizeof( *ptr ) );
	 	ptr->next = firstcol;    /* add to list */
	 	firstcol = ptr;
	} 

	firstcol->width = MAX_X;        /* default if not set by parameter */
	firstcol->lmar = DEF_LMAR;      /* default incase no overriding parm */
	firstcol->anchor = -1;		/* bogus -- we use lmar later if a= is not supplied */

	while( (len = FMgetparm( &buf )) > 0 )   /* while parameters to get */
	{
	 	switch( tolower( buf[0] ) )            /* check it out */
	  	{
			case 'a':			/* anchor point */
				firstcol->anchor = FMgetpts( &buf[2], len-2 );
				break;
	
	   		case 'y':			/* allow user to specify the starting y */
				cury = FMgetpts( &buf[2], len-2 );
				break;
	
	   		case 'i':                 /* indention value set for col 1 */
	    			firstcol->lmar = FMgetpts( &buf[2], len-2 ); /* convert to points */
	    			break;
	
	   		case 'g':                 /* gutter value supplied */
	    			gutter = FMgetpts( &buf[2], len-2 );   /* convert to points */
	    			break;
	
	   		case 'w':						 /* order of this IS important. MUST be last */
	    			buf+=2;                  /* skip to the value and fall into default */
	    			len-=2;                  /* knock down to cover skipped w= */
	                /* **** NOTICE NO break here on purpose *** */
	   		default:                  /* assume a number is a width (backward compat with old versions) */
	    			if( isdigit( buf[0] ) || buf[0] == '.' )
	     			firstcol->width = FMgetpts( buf, len  );  /* convert to points */
	    			break;
	  	}
	} 

	if( firstcol->anchor < 0 )
		firstcol->anchor = firstcol->lmar;		/* default to left margin setting */


	for( ptr = firstcol; ptr->next != NULL; ptr = ptr->next )
	{
	 	ptr->next->width = ptr->width;       /* set width of next block */
	 	ptr->next->lmar = ptr->lmar + ptr->width + gutter;
	}

	if( linelen > firstcol->width ) /* change if too big incase they dont set */
		linelen = firstcol->width -5;    /* no msg, becuase they may set later */

	cur_col = firstcol;        /* set up current column pointer */

	TRACE(1, "col-def: anchor=%d lmar=%d width=%d g=%di\n", firstcol->anchor, firstcol->lmar, firstcol->width, gutter/72 );

	lmar = cur_col->lmar + diff;   	/* set lmar based on difference calculated */
	hlmar = cur_col->lmar + diffh; 	/* earlier and next columns left edge */

	if( lilist != NULL )           	/* if list then reset xpos for next col */
		lilist->xpos = cur_col->lmar + diffx;
}       
