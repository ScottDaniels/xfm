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
* TFM 
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
*              6 Dec 1996 - To convert for hfm (line length based only on
*                           the number of characters in the buffer as
*                           display utility will format, punctuation space
*                           is ignored as browser will provide.
*             21 Mar 2001 - Ironicly back to whence we began - tfm
*             23 Apr 2001 - To add support for .tu command
*				17 Jul 2016 - Bring prototypes into modern era.
*************************************************************************
*/
extern void FMaddtok( char* buf, int len )
{
 int remain;          /* calculated space remaining before the right margin */
 int i;               /* loop index */
 int toksize;         /* size of token (#characters) */

 flags2 |= F2_OK2JUST;

 words++;             /* increase the number of words in the document */

 if( len > ((linelen/7) - optr) )         /* room for it? */
   FMflush( );

 for( i = 0; i < len && xlate_u; xlate_u-- )          /* translate token if necessary */
  {
    buf[i] = toupper( buf[i] );
    i++;
  }

 xlate_u = 0;                         /* in case user asked for more than token size */

 for( i = 0; i < len; i++, optr++ )     /* copy token to output buffer */
  {                                     /* escaping special html characters */
   switch( buf[i] )
    {
#ifdef NEVER_ME
     case '>':
      obuf[optr] = EOS;                 /* terminate for strcat */
      strcat( obuf, "&gt;" );           /* copy in the character */
      optr += 3;                        /* incr past the html escape string */
      osize += 3;                       /* incr number of chars in output */
      break;

     case '<':
      obuf[optr] = EOS;                 /* terminate for strcat */
      strcat( obuf, "&lt;" );           /* copy in the character */
      optr += 3;                        /* incr past the html escape string */
      osize += 3;                       /* incr number of chars in output */
      break;

     case '&':
      obuf[optr] = EOS;                 /* terminate for strcat */
      strcat( obuf, "&amp;" );          /* copy in the character */
      optr += 4;                        /* incr past the html escape string */
      osize += 4;                       /* incr number of chars in output */
      break;
#endif

     case '^':                  /* allow user to escape ><& characters */
      i++;                  /* point at next character and drop into insert */

     default:                 /* not a special character - just copy in */
      obuf[optr] = buf[i];
      obuf[optr+1] = EOS;      /* just incase one of the above encountered */
      break;
    }                         /* end switch */
  }                            /* end copy of token */

 if( ! (flags2 & F2_QUOTED) )
  {
   obuf[optr]= BLANK;            /* terminate token with a blank */
   optr++;                       /* set for location of next token */
  }
 obuf[optr] = EOS;             /* terminate buffer incase we flush */

 
 flags2 &= ~F2_OK2JUST;        /* justify only on buffers dumped from here */
}                              /* FMaddtok */

