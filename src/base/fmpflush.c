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
*    Abstract: This is now a dummy function for non page oriented formatters to 
*				use when needing to 'flush' a page.
*    Parms:    None.
*    Returns:  Nothing
*    Date:     1 December 1988
*    Author:   E. Scott Daniels
*
*    Modified: 29 Jun 1994 - To convert to rfm
*               8 Dec 1994 - To put a para mark before page if not there
*                            and if not in a list item list.
*				17 Jul 2016 - Changes for better prototype generation.
*				13 Oct 2017 - Remove last rtf traces from this.
*****************************************************************************
*/
extern void FMpflush(  void )
{
 int diff;                   /* difference between default lmar and cur lmar*/
 int diffh;                  /* difference between default lmar and header */
 int i;                      /* index into page number buffer */
 char tbuf[HEADFOOT_SIZE+1]; /* buf to gen page and head/foots in  */
 int len;                    /* number of characters in the string */
 int even = FALSE;           /* even/odd page number flag */
 int skip;                   /* # columns to skip before writing string */
 int shift = FALSE;          /* local flag so if stmts are executed only once */


 page++;                             /* increase the page number */

                                 /* determine if we need to shift the page */
 diff = lmar - cur_col->lmar;   /* calculate diff in col default mar& cur mar */
 diffh = hlmar - cur_col->lmar;  /* calc diff in header left margin */
 cur_col = firstcol;             /* start point at the first column block */
 lmar = cur_col->lmar + diff;    /* set up lmar for first column */
 hlmar = cur_col->lmar +diffh;   /* set up header left margin for first col */

 if( rtopy != 0 )
  {
   topy = rtopy;               /* reset as temp y only good to end of page */
   rtopy = 0;                  /* indicate nothing set at this point */
  }

 cury = topy;                  /* reset current y to top y position */
}                              /* FMpflush */
