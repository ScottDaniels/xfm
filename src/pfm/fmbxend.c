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
*  Mnemonic: FMbxend
*  Abstract: This routine is called to "end" a box at the current y position.
*            The routine does NOT reset the box in progress flag so that this
*            routine can be called to end a box at the end of a page so that
*            we do not have open boxes flowing from page to page. This routine
*            must NOT call FMflush as it will cause an unexitable recursion
*            to occur.
*            This routine assumes that the postscript "box" routine has been
*            defined and has the following format:
*                 x1 y1 x2 y2 box
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     15 October 1992
*  Author:   E. Scott Daniels
*
*  Modified: 26 Oct 1992 - To call flush.
*            10 Dec 1992 - To use AFI routines for ansi compatability
*            23 Apr 1992 - To base vert lines on lmar in the box structure
*                          and not the physical left margin of the page.
*            11 Apr 1994 - lmar/rmar are now relative to column not absolute.
*			17 Jul 2016 - Bring decls into the modern world.
*****************************************************************************
*/
extern void FMbxend( void )
{
 char buf[255];      /* buffer to build output string in */
 int len;            /* length of output to write */
 int i;              /* loop index */
 int x;              /* xposition for drawing line */
 int almar;          /* absolute left margin of box for vert lines */

/*
 if( cury >= MAX_Y )
  cury = MAX_Y + 2;    */ /* dont overwrite footer line if its being printd */

 sprintf( buf, "%d %d %d %d box\n",
                      box.lmar, -(cury + textspace),  /* lleft */
                      box.rmar, -box.topy );

 AFIwrite( ofile, buf );  /* put out the box command */

 almar = box.lmar + cur_col->lmar;                   /* absolute lmar of box */
almar = box.lmar;
 TRACE( 2, "box-end: almar = %d  vcol[0] =%d\n", almar, box.vcol[0] );
 for( i = 0; i < MAX_VCOL && box.vcol[i] > 0; i++ )   /* draw each vert line */
  {
   x = box.vcol[i] + almar;            /* figure x for lines based on mar */
   snprintf( buf, sizeof( buf ), "%d %d moveto %d %d lineto stroke\n", x, -box.topy, x, -(cury + textspace) );

   AFIwrite( ofile, buf );              /* write buffer to draw the line */
  }                                     /* end for loop */

}       /* FMbxend */
