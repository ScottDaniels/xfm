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
*   Mnemonic:  FMcd
*   Abstract:  This routine parses the parameters on the .cd command and
*              builds the required column blocks.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      1 December 1988
*   Author:    E. Scott Daniels
*
*   .cd n w i=value[i|p] g=value[i|p]
*           n - The number of columns     (required)
*           w - The width of each column  (optional)
*           i=- The indention of col 1 from edge of page (optional)
*           g=- The gutter space between each column (optional)
*
*   Modified: 13 Jul 1994 - To convert to rfm
*              9 Sep 1994 - To check first parameter properly
***************************************************************************
*/
void FMcd( )
{
 char *buf;                 /* pointer at the token */
 int gutter = DEF_GUTTER;   /* gutter value between cols */
 int i;                     /* loop index */
 int j;                     /*   "    "   */
 int len;                   /* length of parameter token */
 struct col_blk *ptr;       /* pointer at newly allocated blocks */
 char wbuf[50];             /* work buffer */

 len = FMgetparm( &buf );    /* get the number of columns requested */
 if( len <= 0 )              /* if nothing then get lost */
  return;

 while( firstcol != NULL )   /* lets delete the blocks that are there */
  {
   ptr = firstcol;
   firstcol = firstcol->next;   /* step ahead */
   free( ptr );                 /* loose it */
  }

 i = atoi( buf );               /* convert parm to integer - number to create */
 if( i <= 0 || i > MAX_COLS )
  i = 1;                        /* default to one column if none entered  */

 firstcol = NULL;               /* initially nothing */

 for( i; i > 0; i-- )           /* create new col management blocks */
  {
   ptr = (struct col_blk *) malloc( sizeof( struct col_blk ) );
   ptr->next = firstcol;         /* add to list */
   firstcol = ptr;
  }        /* end for */

 firstcol->width = MAX_X;        /* default if not set by parameter */
 firstcol->lmar = DEF_LMAR;      /* default incase no overriding parm */

 while( (len = FMgetparm( &buf )) > 0 )   /* while parameters to get */
  {
   switch( buf[0] )            /* check it out */
    {
     case 'i':                 /* indention value set for col 1 */
     case 'I':
      firstcol->lmar = FMgetpts( &buf[2], len-2 ); /* convert to points */
      firstcol->lmar = firstcol->lmar < 0 ? 18 : firstcol->lmar;
      break;

     case 'g':                 /* gutter value supplied */
     case 'G':
      gutter = FMgetpts( &buf[2], len-2 );   /* convert to points */
      break;

     default:                  /* assume a number is a width */
      if( isdigit( buf[0] ) || buf[0] == '.' )
       firstcol->width = FMgetpts( buf, len  );  /* convert to points */
      break;
    } /* end switch */
  }   /* end while */

 if( firstcol->next != NULL )     /* if in multiple column mode */
  {                               /* adjust margins and set col lmar to 0 */
   sprintf( wbuf, "\\margl%d", firstcol->lmar * 20 );
   AFIwrite( ofile, wbuf );
   firstcol->lmar = 0;
  }

                        /* in rfm all column blocks have same info */
 for( ptr = firstcol; ptr->next != NULL; ptr = ptr->next )
  {
   ptr->next->width = ptr->width;            /* set width of next block */
   ptr->next->lmar = 0; /*ptr->lmar; */        /*+ ptr->width + gutter; */
  }

 if( linelen > firstcol->width )   /* change if too big incase they dont set */
  linelen = firstcol->width -5;    /* no msg, becuase they may set later */

 cur_col = firstcol;               /* set up current column pointer */

 FMsetcol( gutter );               /* put col setup info into document */
 col_gutter = gutter;              /* save current gutter value for later */
}               /* FMcd */
