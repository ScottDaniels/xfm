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
*  Mnemonic: FMsetcol
*  Abstract: This routine will write the necessary rich text format info for
*            multiple column definitions
*  Parms:    gutter - space between columns (points)
*  Returns:  Nothing
*  Date:     13 July 1994
*  Author:   E. Scott Daniels
*  Mods:	17 Jul 2016 - Changes for better prototype generation.
*
******************************************************************************
*/
extern void FMsetcol( int gutter )
{
 struct col_blk *cptr;         /* pointer into column list */
 int i;
 char wbuf[50];                /* work buffer for output */

 if( (rflags & RF_SBREAK) == 0 )                   /* if no section break */
  {
   AFIwrite( ofile, "\\sect \\sectd \\sbkcol\\footery2" );  /* then do one */
   rflags |= RF_SBREAK;                                     /* and set flg */
  }

 for( i = 0, cptr = firstcol; cptr != NULL; cptr = cptr->next, i++); /* count*/

 sprintf( wbuf, "\\cols%d", i );   /* get number of cols */
 AFIwrite( ofile, wbuf );          /* and place in file */

 if( firstcol->next != NULL )  /* if more than one col */
  {
   for( i = 1, cptr = firstcol; cptr != NULL; cptr = cptr->next, i++ )
    {
     sprintf( wbuf, "\\colno%d\\colw%d", i, firstcol->width * 20 );
     AFIwrite( ofile, wbuf );
     if( cptr->next != NULL )    /* if more then insert gutter */
      {
       sprintf( wbuf, "\\colsr%d", gutter * 20 ); /* confert to rfm scale */
       AFIwrite( ofile, wbuf );                   /* and place into the file */
      }
    }        /* end for */
  }          /* end if multi cols */
}            /* FMsetcol */
