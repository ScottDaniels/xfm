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
******************************************************************************
*
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
*  Modified: 10 Dec 1992 - To use AFI routines for ansi compatability
*            12 May 1993 - To reduce bullet size to textsize/2.
*             6 Jun 1993 - To output bullet characters in octal (\xxx).
*             6 Apr 1994 - To convert to pure points.
*             4 May 1994 - To use block->size value for adjustments
*			17 Jul 2016 - Bring decls into the modern world.
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
   for( liptr = lilist; liptr != NULL; liptr = liptr->next )
    {                       /* run each of the block and place item marks */
     if( liptr->ypos[0] > 0 )              /* if something set on this block */
      {
       FMsetfont( liptr->font, liptr->size );   /* set font for this block */
	FMfmt_add( );				/* add a formatting block to the list */
      }
     for( i = 0; i < MAX_LIITEMS && liptr->ypos[i] > 0; i++ ) /* run list */
      {
       sprintf( outbuf, "%d %d moveto (\\%o) show\n", liptr->xpos,
                      -liptr->ypos[i], (unsigned int) liptr->ch );
       AFIwrite( ofile, outbuf );                   /* write it to the file */
       liptr->ypos[i] = -1;                         /* turn off this mark */
      }                      /* end for each item on this block */
     liptr->yindex = 0;      /* start back at the beginning */
    }                        /* end for each block in the list */

   if( option == TRUE )       /* not just end of pg - delete the block too */
    {
     lmar = lilist->xpos;          /* reset the margin */
     linelen += lilist->size + 3;  /* and line length */
     liptr = lilist;               /* prep to delete the head list item */
     lilist = liptr->next;
     free( liptr->font );          /* free the font that was allocated */
     free( liptr );                /* delete the block */
    }
  }   /* end if liliptr != NULL */

}   /* FMendlist */
