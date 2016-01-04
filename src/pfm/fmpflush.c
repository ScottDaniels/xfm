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
*    Mnemonic: FMpflush
*    Abstract: This routine causes the running header and footer to be placed
*              into the file and then issues the newp command which will issue
*              a showpage and then a translate.
*    Parms:    None.
*    Returns:  Nothing
*    Date:     1 December 1988
*    Author:   E. Scott Daniels
*
*    Modified: 22 Apr 1991 - To put header and footer and page under 2nd
*                              column if in two column mode.
*               3 May 1991 - To support two sided page format of headers.
*               4 May 1991 - To shift pages by pageshift size
*                            Broke header/footer code to FMrunout
*              13 May 1992 - Conversion to PostScript output
*              25 Oct 1992 - To call fmrunout with page and shift only.
*              27 Nov 1992 - To also set hlmar on new column
*              10 Dec 1992 - To use AFI routines for ansi compatability
*               7 Apr 1994 - To reset topy to realy if realy not 0.
*		       17 Aug 2001 - To add support for on eject command buffer
*				26 Jun 2013 - Calls colnotes_show() to show the notes before
*						doing the eject. 
*				07 Jul 2013 - Colour support changes. 
*				18 Dec 2015 - Cleaned up commented out code.
*****************************************************************************
*/
void FMpflush( )
{
	int diff;                   /* difference between default lmar and cur lmar*/
	int diffh;                  /* difference between default lmar and header */
	int i;                      /* index into page number buffer */
	char tbuf[HEADFOOT_SIZE+1]; /* buf to gen page and head/foots in  */
	int len;                    /* number of characters in the string */
	int even = FALSE;           /* even/odd page number flag */
	int skip;                   /* # columns to skip before writing string */
	int shift = FALSE;          /* local flag so if stmts are executed only once */
	char *tok;

	if( cn_space  )				/* if and end note was defined, need to add it too */
	{
		TRACE( 3, "pflush: dumping column notes first, pushing .pa command back\n"  );
 		AFIpushtoken( fptr->file, ".pa" );  	/* page command to execute after the column notes are written */
			
		cn_space = 0;
		FMcolnotes_show( 0 );			/* cause the column notes to be put in before eject */
		return;							// go run all of the commands which will 'end' with the page eject we just pushed
	}

	page++;                             /* increase the page number */

	if( rtopy != 0 )          /* see if top margin reset temporarily */
	{
		topy = rtopy;               /* reset as it is good only to the end of pg*/
		rtopy = 0;                  /* indicate nothing set at this point */
	}

                              /* determine if we need to shift the page */
	if( flags2 & F2_2SIDE )      /* if in two sided mode */
	{
		if( (page % 2) != 0 )      /* and its an odd page (only shift odd pages) */
		shift = TRUE;
	}
	else                         /* page shift not 0 and one sided mode */
		shift = TRUE;               /* then EACH page is shifted */

	FMpushcolour( "#000000" );	/* push current colour and set head/foot/pagen colour (black) */
	FMrunout( page, shift );	/* put header/footer and page strings */
	FMpopcolour( );				/* restore the colour */

	AFIwrite( ofile, "newp\n" );    /* newpage ps definition "showpage" */

	diff = lmar - cur_col->lmar;  /* calculate diff in col default mar& cur mar */
	diffh = hlmar - cur_col->lmar;  /* calc diff in header left margin */
	cur_col = firstcol;           /* start point at the first column block */
	lmar = cur_col->lmar + diff;  /* set up lmar for first column */
	hlmar = cur_col->lmar +diffh; /* set up header left margin for first col */

	cury = topy + (textsize/2);                  /* reset current y to top y position */

	FMateject( 1 );		/* do page eject stuff first */
}
