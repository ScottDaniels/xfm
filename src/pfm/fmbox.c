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
* Mnemonic: FMbox
* Abstract: This routine interprets the .bx command and calls either bxstart
*           or the bxend routine to start or end a box.
* Parms:    None.
* Returns:  Nothing.
* Date:     Original 5 May 1989 Rewritten for Postscript 15 October 1992
* Author:   E. Scott Daniels
*
* Modified: 11 Nov 1992 - To call flush before start and end
*             so we dont flush on an h on/off call.
* .bx {start | end} [H] <vert col1> <vert col2>... <vert col10>
*      H - draw a horizontal line after each line of text
* .bx H {on | off} start/stop drawing horizontal lines
****************************************************************************
*/
void FMbox( )
{
 int len;        /* length of parameter entered on .bx command */
 char *buf;      /* pointer at the input buffer information */
 int col;        /* column for vertical line placement */
 int i;          /* loop index */

 len = FMgetparm( &buf );   /* get next parameter on command line */

 if( len > 0 && toupper( buf[0] ) == 'S' )  /* start box */
  {
   if( (flags2 & F2_BOX) == 0 )   /* if not inprogress already */
    {
     FMflush( );             /* flush out last line in the box */
     FMbxstart( TRUE, 0, 0, 0, 0 );           /* initialize box block */
     flags2 |= F2_BOX;            /* indicate box in progress */
    }
  }
 else
  if( len == 0 || toupper( buf[0] ) == 'E' ) /* no parm or end asked for */
   {
    if( flags2 & F2_BOX )   /* if one is inprogress */
     {
      FMflush( );             /* flush out last line in the box */
      FMbxend( );             /* then terminate the box here */
      flags2 &= ~F2_BOX;      /* indicate box has stopped */
     }
   }
  else
   if( toupper( buf[0] ) == 'H' )    /* start or stop drawing horiz lnes */
    {
     len = FMgetparm( &buf );     /* get the next parameter */
     if( len > 0 && toupper( buf[1] ) == 'N' )   /* turn on horiz lines */
      box.hconst = TRUE;
     else                       /* default to turning them off */
      box.hconst = FALSE;
    }       /* end handle horizontal line stuff */
}                        /* FMbox */
