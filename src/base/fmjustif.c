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
*  Mnemonic: FMjustify
*  Abstract: This routine is responsible for sending out the current buffer
*            with the proper justification command and related parameters.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     12 November 1992
*  Author:   E. Scott Daniels
*
*  Modified: 04 Dec 1992 - To call flush if spaces are 0
*            10 Dec 1992 - To use AFI routines for ansi compatability
*             6 Apr 1994 - To take advantage of linelen being points now
*****************************************************************************
*/
void FMjustify( )
{
 int spaces;        /* number of blank spaces */
 int i;             /* loop index */
 int len;           /* length of the string for write */
 char jbuf[256];    /* buffer for write */

 for( i = 0; obuf[i] != EOS; i++ );   /* find end of the string */
 for( i--; obuf[i] == BLANK; i-- );   /* find last non blank */
 obuf[i+1] = EOS;                     /* ensure no blanks at end of str */

 for( spaces = 0, i = 0; obuf[i] != EOS; i++ )
  if( obuf[i] == BLANK )
   spaces++;                   /* bump up the count */

 if( spaces == 0 )
  {
   FMflush( );            /* dont call just with a 0 parameter */
   return;
  }

 if( optr == 0 )   /* need to do a move to? */
  {
   cury += textsize + textspace;    /* move to the next y position */
   if( flags & DOUBLESPACE )        /* if in double space mode ...    */
    cury += textsize + textspace;   /* then skip down one more */

   if( cury > boty )               /* are we out of bounds? */
    {
     FMceject( 0 );       /* move to next column */
    }

   sprintf( jbuf, "%d -%d moveto\n", lmar, cury );  /* create moveto */
   AFIwrite( ofile, jbuf );      /* write the move to command or x,y for cen */
  }                              /* end need to do a move to */

 if( flags2 & F2_SETFONT )   /* need to reset font to text font? */
  {
   FMsetfont( curfont, textsize );  /* set the font */
   flags2 &= ~F2_SETFONT;           /* turnoff setfont flag */
  }

 sprintf( jbuf , "%d (%s) %d just\n", spaces, obuf, lmar + linelen );
             /*   cur_col->lmar + cur_col->width ); */
 AFIwrite( ofile, jbuf );         /* write the buffer to output file */

 if( flags2 & F2_BOX && box.hconst == TRUE )  /* in box and horz lines */
  {
   sprintf( jbuf, "%d -%d moveto %d -%d lineto stroke\n",
                  box.lmar, cury + 2, box.rmar, cury + 2 );
   AFIwrite( ofile, jbuf );               /* output the information */
  }
}           /* FMjustify */
