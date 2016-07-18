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
*****************************************************************************
* TFM
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
*             6 Dec 1996 - To convert for hfm
*            21 Mar 2001 - Back to the thing that started it all; TFM
*				17 Jul 2016 - Bring prototypes into modern era.
******************************************************************************
*/
extern void FMbeglst( void )
{
 struct li_blk *new;     /* new allocated block */
 char *buf;              /* pointer at next tokin in buffer */
 int len;                /* length of parameter input */
 int i;                  /* loop index */

 FMflush( );              /* kick out whatever is there */

 new = (struct li_blk *) malloc( sizeof( struct li_blk ) );  /* get space */
 if( new == NULL )
  {
   printf( "BEGLST: new was null on malloc\n" );
  }
 new->next = lilist;      /* point at whats there (if anything) */
 new->size = textsize/2;  /* default to 1/2 the current text size */
 new->ch = DEF_LICHR;     /* initial li character default */
 new->font = NULL;        /* no font yet */
 new->ch = '*';
 new->xpos = 0;
 lilist = new;            /* put this at the head of the list */

 linelen -= 14;           /* line leng is shorter now */
 lmar += 14;              /* scootch over a bit */

 while( (len = FMgetparm( &buf )) != 0 )  /* parms ignored here */
  if( *(buf+1) != '=' )
   new->ch = *buf;
}           /* FMbeglst */
