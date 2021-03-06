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
*   Mnemonic: FMflush
*   Abstract: This routine is responsible for flushing the output buffer
*             to the output file and resetting the output buffer pointer.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     15 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 29 Jun 1994 - To convert to rfm.
*              6 Dec 1994 - To reset font even if nothing to write.
*              7 Dec 1994 - To prevent par mark at page top if in li list
*             14 Dec 1994 - To prevent par mark at top if in di list
*				17 Jul 2016 - Changes for better prototype generation.
*				13 Oct 2017 - Strip deprecated rtf formatting junk.
****************************************************************************
*/
extern int FMflush( void )
{
 int len;           /* length of the output string */
 char fbuf[512];    /* buffer to build flush strings in */

 if( optr == 0 )  /* nothing to do so return */
  {                                        /* but..... */
   if( flags2 & F2_SETFONT )          /* reset font if flag is on before  */
    {                                 /* going back to caller */
     FMsetfont( curfont, textsize );  /* set the new font in file */
     flags2 &= ~F2_SETFONT;           /* turnoff setfont flag */
    }
   return 1;
  }

 cury += textsize + textspace;   /* keep track of where we are */

 if( cury >= boty )      /* if this would take us off the page... */
  {
   FMceject( 0 );          /* move on if past the end */
  }

 if( flags2 & F2_SETFONT )   /* need to reset font to text font? */
  {
   FMsetfont( curfont, textsize );  /* set the new font in file */
   flags2 &= ~F2_SETFONT;           /* turnoff setfont flag */
  }


 /* copy only the cached portion of the output buffer */
 AFIwrite( ofile, obuf );   /* write the text out to the file */

 optr = 0;
	return 1;		/* the return value is needed in hfm, but we share protots so we need to do too */
}                               /* FMflush */
