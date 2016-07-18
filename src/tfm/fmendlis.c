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

#include "../base/symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
******************************************************************************
*
* TFM
*  Mnemonic: FMendlist
*  Abstract: This routine is responsible for putting out the list item
*            marks that are defined in the current list item blocks.
*            As each mark is putout its place in the ypos array is reset.
*            If the option true is passed in the current (top) list item
*            block is deleted. This allows this routine to be called from
*            FMcflush to paint all of the item marks on the page, without
*            really terminating the list.
*            Because this routine is called from FMceject to putout item
*            marks in the current column, this routine must NOT call flush
*            as flush calls ceject and we stackoverflow.
*  Parms:    option - If true then top list item block is removed.
*  Returns:  Nothing
*  Date:     19 October 1992
*  Author:   E. Scott Daniels
*
*  Modified:  1 Jul 1994 - To convet to rfm
*             6 Dec 1996 - To convert to hfm
*            21 Mar 2001 - Tracing our roots into the soil; TFM
*				17 Jul 2016 - Bring prototypes into modern era.
******************************************************************************
*/
extern void FMendlist( int option )
{
 struct li_blk *liptr;       /* pointer at list item block to delete */
 char outbuf[80];            /* buffer to build output string in */
 int len;                    /* length of output buffer */
 int i;                      /* loop index */

 if( lilist != NULL )    /* if list has been started */
  {
   if( option == TRUE )       /* not just end of pg - delete the block too */
    {
     liptr = lilist;                   /* prep to delete the head list item */
     lilist = liptr->next;
     if( liptr )
      free( liptr );                    /* delete the block */
#ifdef KEEP
     strcat( obuf, "</ul>" );   /* put in html end list tag */
     optr += 5;
#endif
    }
  }   /* end if liliptr != NULL */

}   /* FMendlist */
