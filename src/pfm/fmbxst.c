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
**************************************************************************
*
*  Mnemonic: FMbxstart
*  Abstract: This box will setup for a box at the current y position.
*            with the left and right margin values set based on the info
*            in the current column block. If the option is TRUE then this
*            routine assumes that the veritcal column numbers are in the
*            input buffer, and will attempt to read them in and set them
*            up (this is so the routine can be called from columeject.
*            Calling routine is responsible for calling FMflush if necessary.
*            This routine must NEVER call FMflush.
*  Parms:    option - TRUE if block structure is completely initialized
*                     (ie a new box not continuation to next column)
*		The remainder of the parms are implemented only by hfm at the 
*		moment and are ignored here. 
*  Returns:  Nothing.
*  Date:     15 October 192
*  Author:   E. Scott Daniels
*  Modified: 26 Oct 1992 - To call flush
*            11 Nov 1992 - Not to call flush
*            17 Nov 1992 - To set lmar & rmar only when option is true
*            23 Apr 1993 - To set box.lmar/rmar on each call to properly
*                          handle new columns where only the lmar value changes
*             7 Apr 1994 - To better display box at top of column
*            11 Arp 1994 - To use FMgetpts routine to allow vert info in inches
*                          and to make margins relative to column.
*	     11 Aug 2001 - To ignore all of the html box options 
*			30 Aug 2012 - To better handle the left side of the box.
*			20 Mar 2013 - Fixed problem with margins.
*			23 Jun 2013 - Reverse the left setting as it was backwards
*			25 Dec 2015 - Corrected bug with column oriented boxes.
*			17 Jul 2016 - Bring decls into the modern world.
**************************************************************************
*/
extern void FMbxstart( int option, char *colour, int border, int width, char *align )
{
 int len;          /* length of paramters passed in */
 char *buf;        /* pointer to next option to parse */
 int i;            /* loop index */
 int j;

 if( option == FALSE )
  {
   box.topy = cury;                    /* set box top */
   cury += textspace + textsize;       /* skip down a bit */
  }
 else
  box.topy = cury + textspace;     /* set the top of the box to current pos */

 if( option )    /* if true - then setup rest of the block */
  {
	i = 0;
	
	//box.lmar = -3; 			   /* default relative positions to column margins */
	//box.lmar = lmar - 3;				/* default relative positions to column margins */
	box.lmar = cur_col->lmar -3;  		/* default to column rather than margin/linelen */
	box.rmar = box.lmar + cur_col->width  + 6;
	box.hconst = FALSE;                   /* default to no hlines */

	while( (len = FMgetparm( &buf )) > 0 ) 	/* snag parameters */
	{
		switch( tolower( *buf ) )
		{
			case 'h':		/* horizontal lines option */
				box.hconst = TRUE;
				break;

			case 'm':		/* margin box rather than column box */
          			//box.lmar = (lmar - cur_col->lmar) -3;  /* relative val to col lmar */
					box.lmar = lmar - 3; 			   		/* baed on margin and line length rather than col def */
          			box.rmar = box.lmar + linelen + 6;     /* relative to col lmar */
				break;
	
			default:			/* assume vert line option if dig, else ignore */
				if( isdigit( *buf ) && i < MAX_VCOL )
					box.vcol[i] = FMgetpts( buf, len );  /* get offset  */
				break;
		}
	}

	box.vcol[i] = -1;    /* mark end of vcol list */
	TRACE(1, "bx-start: lmar=%d rmar=%d\n", box.lmar, box.rmar );
  }                     /* end if option was true */
}                       /* FMbxstart */
