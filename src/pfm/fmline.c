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
*  Mnemonic: FMline
*  Abstract: This routine is responsible for putting a line into the output
*            file.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     1 November 1992
*  Author:   E. Scott Daniels
*
*  Modified: 10 Dec 1992 - To use AFI routines for ansi compatability
*            11 Apr 1994 - To check for box to see what margins to use.
*			20 Mar 2013 - Tweaks to work with changes to the box lmar.
*****************************************************************************
*/
void FMline( )
{
 char out[100];  /* output buffer */
 int len;        /* lenght of output string */

 FMflush( );     /* put out what ever is currently cached */

 if( flags2 & F2_BOX )          /* if in box use box margins */
  sprintf( out,
      "gsave %d setlinewidth %d %d moveto %d %d lineto stroke grestore\n", linesize, box.lmar, -(cury+4), box.rmar, -(cury+4) );
 else
  sprintf( out, "gsave %d setlinewidth %d %d moveto %d %d lineto stroke grestore\n", linesize, lmar, -(cury+4), lmar + cur_col->width, -(cury+4) );

 AFIwrite( ofile, out );   /* write the command to draw the line out */

 cury += 8 + linesize;   /* bump the current spot up some */
}           /* FMline */

/*
	a partial line that is centered in the column
*/
void FMcline( )
{
 char out[100];  /* output buffer */
 int len;        /* lenght of output string */
	int width; 

 FMflush( );     /* put out what ever is currently cached */

	
 sprintf( out, "gsave %d setlinewidth %d %d moveto %d %d lineto stroke grestore\n", linesize, lmar + 10, -(cury+4), lmar + (linelen - 10), -(cury+4) );

 AFIwrite( ofile, out );   /* write the command to draw the line out */

 cury += 8 + linesize;   /* bump the current spot up some */
}           /* FMline */
