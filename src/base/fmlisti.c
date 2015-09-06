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
******************************************************************************
*
*  Mnemonic: FMlisti
*  Abstract: This routine will setup for a list item in the current text.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     1 July 1994
*  Author:   E. Scott Daniels
*
*  Modified: 19 Jul 1994 - To allow list items in boxes
*             8 Dec 1995 - To use font in list item block
******************************************************************************
*/
void FMlisti( )
{
 struct li_blk *lptr;         /* pointer into listitem list */
 char buf[81];                /* output buffer */
 int right;                   /* right indention value */

 lptr = lilist;           /* point at the list */

 FMflush( );

 if( rflags & RF_PAR )               /* if a par has been issued */
  AFIwrite( ofile, "\\pard" );       /* just terminate settings */
 else
  {
   AFIwrite( ofile, "\\par\\pard" );         /* terminate previous setup */
  }

 if( flags2 & F2_BOX )
	FMbxstart( FALSE, 0, 0, 0, 0 );

 rflags &= ~RF_PAR;                   /* reset flag */

 FMccol( 1 );                  /* ensure at least one line remains on page */

 right = cur_col->width - ((lmar-cur_col->lmar) + linelen);

 sprintf( buf,
   "\\plain\\li%d\\fi-200\\qj{\\pnlvlblt\\pn%s\\pntxtb \\'%02x\\pnindent200}",
               lmar * 20, lptr->font, lptr->ch & 0xff );
 AFIwrite( ofile, buf );
 sprintf( buf, "\\ri%d\\%s\\fs%d", right * 20, curfont, textsize * 2 );
 AFIwrite( ofile, buf );
}             /* FMlisti */
