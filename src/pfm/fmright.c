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
****************************************************************************
*
*  Mnemonic: FMright
*  Abstract: This routine processes the .ri command. It accepts an x value
*            (optionally) defining the right most point and uses the remainder
*            of the characters on the input as the string to right justify.
*            If the x value is omitted, then the current column width is
*            added to the current column left margin value and that value
*            is used as the right most x coordinate of the text. If a number
*            is to be the first part of the string and no x value is to be
*            given then the number should be escaped with a carrot ^.
*  Parms:    None
*  Returns:  Nothing
*  Date:     23 March 1993
*  Author:   E. Scott Daniels
*	Mods:	17 Jul 2016 - Bring decls into the modern world.
*
*            .ri [x value] text to right justify <eos>
*******************************************************************************
*/
extern void FMright( void )
{
 char *buf;          /* pointer at input buffer */
 int i;              /* length of parameter from input */
 int x;              /* x value for PostScript moveto */
 char wbuf[80];      /* buffer to build moveto command in */

 flags2 |= F2_RIGHT;    /* make flush place out right command */

 if( ( i = FMgetparm( & buf )) <= 0 )   /* if nothing else on line */
  return;                               /* then get out now */

 if( optr == 0 )     /* if nothing in buffer, force flush to setup next y */
  {
   optr = 1;
   obuf[0] = ' ';   /* set up blank for flush */
   obuf[1] = EOS;   /* string must be terminated */
  }
 FMflush( );                 /* flush out the buffer and setup new y value */

 if( isdigit( buf[0] ) )     /* if user started with a digit then assume x */
  x = atoi( buf );           /* convert buffer to integer */
 else
  {                          /* assume first token of string to add */
   FMaddtok( buf, i );       /* add it to output buffer strip and escape */
   x = cur_col->lmar + cur_col->width;  /* calculate default x value */
  }

 while( (i = FMgetparm( &buf )) > 0 )   /* while not at end of input line */
  FMaddtok( buf, i );                   /* add remainder of string tokens */

 sprintf( wbuf, "%d %d\n", x, -cury );  /* setup x,y for right command */
 AFIwrite( ofile, wbuf );
 FMflush( );                           /* flush remainder of the buffer */

 flags2 &= ~F2_RIGHT;         /* turn off right justify flag */
}            /* FMright */
