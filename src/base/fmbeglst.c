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
*  Mnemonic: FMbeglst
*  Abstract: This routine is responsible for adding a list item block to the
*            list item list.
*  Parms:    Nothing. It reads the input buffer to get the character that is
*            to be used, and the optional font name.
*  Returns:  Nothing.
*  Date:     19 October 1992
*  Author:   E. Scott Daniels
*
*      .bl [character | \nnn] s=size f=fontname
*
*  Modified:  1 Jul 1994 - To convert to rfm
*                          made parameters non positional.
*            25 Nov 1996 - Correct problems picking up f= parameter
******************************************************************************
*/
void FMbeglst( )
{
 struct li_blk *new;     /* new allocated block */
 char *buf;              /* pointer at next tokin in buffer */
 int len;                /* length of parameter input */
 int i;                  /* loop index */

 FMflush( );              /* kick out whate ever is there */

 new = (struct li_blk *) malloc( sizeof( struct li_blk ) );  /* get space */
 if( new == NULL )
  {
   printf( "BEGLST: new was null on malloc\n" );
  }
 new->next = lilist;      /* point at whats there (if anything) */
 new->size = textsize/2;  /* default to 1/2 the current text size */
 new->ch = DEF_LICHR;     /* initial li character default */
 new->font = NULL;        /* no font yet */
 lilist = new;            /* put this at the head of the list */

 while( (len = FMgetparm( &buf )) != 0 )  /* get parameters */
  {
   if( toupper( buf[0] ) == 'S' && buf[1] == '=' )   /* size parameter */
    new->size = FMgetpts( &buf[2], len-2 );    /* convet to points */
   else
    if( buf[0] == '\\' )               /* if escape character not entered */
     {
      new->ch = (char) atoi( &buf[1] );  /* xlate decimal value input */
      if( new->ch == 0x04 )
       new->ch = '*';                 /* dont allow control d to be entered */
     }
    else
     if( toupper( buf[0] ) == 'F'  && buf[1] == '=' )  /* font entered */
      {
       if( len > 79 )
        len = 79;                                   /* dont bring us down */
       new->font = (char *) malloc( (unsigned) 80 );
       strncpy( new->font, &buf[2], len-2 );        /* copy in the string */
       new->font[len-2] = EOS;                      /* mark its end */
      }
     else
      new->ch = buf[0];        /* just use character that was entered */
  }                            /* end parameter grabbing while loop */

 if( new->font == NULL )   /* user did not enter font on .bl - use default */
  {
   new->font = (char *) malloc( (unsigned) 80 );
   strcpy( new->font, DEF_LIFONT );
  }

 for( i = 0; i < MAX_LIITEMS; i++ )
  new->ypos[i] = -1;         /* reset all item values */

 new->xpos = lmar;           /* set up on the current left margin */

 new->yindex = 0;            /* set up y position index */

 lmar += new->size + 5;       /* scoot over where text will be placed */
 linelen -= new->size + 5;    /* reduce number of points per line */

}           /* FMbeglst */
