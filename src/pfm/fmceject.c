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
***********************************************************************************
*
*  Mnemonic: FMceject
*  Abstract: This rotine ejects the current column such that the next token
*            is placed at the top of the next column on the page. If the
*            current column is the last defined column then pflush is
*            called to flush the page and then the first column is selected.
*            FMflush MUST be called prior to this routine... This routine
*            cannot call FMflush as it is called by FMflush and a loop might
*            occur.
*  Parms:    None.
*  Date:     6 May 1992
*  Returns:  Nothing.
*  Author:   E. Scott Daniels
*
*  Modified: 15 Oct 1992 - To call box end and box start if in a box
*            19 Oct 1992 - To call end list routine to put out list item marks
*            21 Oct 1992 - To reset hlmar for new column
*            25 Mar 1993 - To properly handle list item xmar when in multi col
*             7 Apr 1994 - To check reset of real top y
*			26 Oct 2006 - Now uses FMatejet() to do stacked eject commands
*				and to allow a set of commands on the .oe command.
*			26 Jun 2013 - calls colnotes_show() if needed before doing the eject
***********************************************************************************
*/
void PFMceject( )
{
	int diff;     /* difference between lmar in col block and current lmar */
	int diffh;    /* difference betweeh header lmar in col block and current */
	int diffx;    /* diff between x value in list blk and curcol lmar */
	char *tok;

	if( flags2 & F2_BOX )		/* if a box is inprogress */
		FMbxend( );             /* then end the box right here */

	FMendlist( FALSE );			/* putout listitem mark characters if in list */

	if( cn_space  )				/* if and end note was defined, need to add it too */
	{
		FMcolnotes_show( 0 );			/* cause the column notes to be put in before eject */
		cn_space = 0;
		return;							/* must return so that col note imbed is processed before */
	}

	if( cur_col->flags & CF_TMP_MAR ) {		// if temp margins were set, reset them before moving on
		cur_col->lmar = cur_col->olmar;
		cur_col->width = cur_col->width;
		cur_col->flags &= ~CF_TMP_MAR;
	}

	diffh = hlmar - cur_col->lmar; /* figure difference between col left mar */
	diff = lmar - cur_col->lmar;   /* and what has been set using .in and .hm */
	if( lilist != NULL )           /* if list in progress */
		diffx = lilist->xpos - cur_col->lmar;  /* then calc difference */

	/* moved before peject in case it calls an oneject that puts in a drawing */
	cury = topy;                   			/* make sure were at the top */
	if( cur_col && cur_col->next != NULL )    /* if this is not the last on the page */
		cur_col = cur_col->next;       /* then select it */
	else
	{
   		FMpflush( );           /* cause a page eject with headers etc */
	
		cur_col = firstcol;  /* select the first column */
  	}

	lmar = cur_col->lmar + diff;   /* set lmar based on difference calculated */
	hlmar = cur_col->lmar + diffh; /* earlier and next columns left edge */
	if( lilist != NULL )           /* if list then reset xpos for next col */
	lilist->xpos = cur_col->lmar + diffx;
	
	if( rtopy > 0  &&  rtopcount > 0 )     /* reset real topy if needed */
	{
		if( (--rtopcount) == 0 )    /* need to reset */
		{
			TRACE( 2, "ceject: reset topy=%d to rtopy=%d\n", topy, rtopy );
			topy = rtopy;
			rtopy = 0;
		}
	}

	if( flags2 & F2_BOX )    /* if a box is inprogress */
		FMbxstart( FALSE, 0, 0, 0, 0 );     /* reset top of box to current y position */

	TRACE( 2, "ceject: new-lmar=%d new-hlmar=%d\n", lmar, hlmar );
	FMateject( 0 );			/* do any queued on eject commands */



#ifdef KEEP_OLD_WAY
/* if user wants to embed multiple commands on an oe statement, then they need to 
   put them in a file and oe should just have the .im command
*/
 if( oncoleject )
 {
	if( trace > 1 )
		fprintf( stderr, "ceject: oe (next col) = (%s)\n", oncoleject );

 	AFIpushtoken( fptr->file, oncoleject );  /* and push onto the input stack */
	free( oncoleject );
	oncoleject = NULL;

	FMpush_state( );
   	flags = flags & (255-NOFORMAT);      /* turn off noformat flag */
   	flags2 &= ~F2_ASIS;                  /* turn off asis flag */
	if( FMgettok( &tok ) )
		FMcmd( tok );
	FMpop_state( );
 }
 else
	if( onallcoleject )
	{
		if( trace > 1 )
			fprintf( stderr, "ceject: oe (all cols) = (%s)\n", oncoleject );
 		AFIpushtoken( fptr->file, onallcoleject );  
		FMpush_state( );
   		flags = flags & (255-NOFORMAT);      /* turn off noformat flag */
   		flags2 &= ~F2_ASIS;                  /* turn off asis flag */
		if( FMgettok( &tok ) )
			FMcmd( tok );
		FMpop_state( );
	}
#endif
}                  /* FMceject */
