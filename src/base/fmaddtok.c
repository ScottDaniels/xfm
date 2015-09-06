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
*************************************************************************
*
*   Mnemonic: FMaddtok
*   Abstract: This routine adds a token to the output buffer. If necessary
*             the buffer is flushed before the token is added.
*   Params:   buf - pointer to the token to add
*             len - length of the token
*   Returns:  Nothing.
*   Date:     15 November 1988
*   Author:   E. Scott Daniels
*
*   Modified:  7 Jul 1994 - To convert to rfm
*              8 Apr 1997 - To properly recognize the ^ token 
*             23 Apr 2001 - To add support for .tu command
*************************************************************************
*/
void FMaddtok( buf, len )
 char *buf;
 int len;
{
 int remain;          /* calculated space remaining before the right margin */
 int i;               /* loop index */
 int toksize;         /* size of token in points */

 words++;             /* increase the number of words in the document */

 if( optr == 0 )       /* if starting at beginning */
  osize = 0;           /* reset current line size */

 remain = linelen - osize;    /* get space remaining for insertion */

 if( (toksize = FMtoksize( buf, len )) > remain )  /* room for it? */
  {                                    /* no - put out current buffer */
   if( flags & JUSTIFY )
    FMjustify( );            /* sendout in justification mode */
   else
    FMflush( );        /* if not justify - send out using flush  */
   osize = 0;          /* new buffer has no size yet */
  }

 osize += toksize;         /* add current token to current length */

 for( i = 0; i < len && xlate_u; xlate_u-- )          /* translate token if necessary */
  {
    buf[i] = toupper( buf[i] );
    i++;
  }

 xlate_u = 0;                        /* in case user asked for more than token size */

 for( i = 0; i < len; i++, optr++ )     /* copy token to output buffer */
  {
   if( buf[i] == '^' )                  /* output next character as is */
    i++;
   else
    if( buf[i] == '{' || buf[i] == '}' || buf[i] == '\\' ) /* need to escape */
     {
      obuf[optr] = '\\';       /* put in the escape character */
      optr++;                  /* and bump up the pointer */
     }
   obuf[optr] = buf[i];         /* copy the next char to the output buffer */
  }    /* end copy of tokin */

 obuf[optr]= BLANK;            /* terminate token with a blank */
 optr++;                       /* set for location of next token */

 if( flags2 & F2_PUNSPACE )    /* if need to space after punctuation */
  if( ispunct( obuf[optr-2] ) )  /* and last character is punctuation */
   {
    obuf[optr] = BLANK;           /* add extra space */
    optr++;                       /* and up the pointer */
   }
 obuf[optr] = EOS;             /* terminate buffer incase we flush */
}                 /* FMaddtok */

