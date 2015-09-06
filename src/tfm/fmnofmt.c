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
*
*   Mnemonic: FMnofmt
*   Abstract: This routine is called when the noformat flag is set. It
*             reads in lines from the input file and if there is not a
*             command in the first column then it places the line of
*             text as is out to the page buffer. If a command is on the
*             input line control is returned to the caller to process the
*             command.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     8 December 1988
*   Author:   E. Scott Daniels
*
*   Modified:  7 Jul 1994 - To convert to rfm
*              4 Oct 1994 - To reduce amount skipped in y direction.
*              6 Dec 1996 - To convert for hfm
*              4 Apr 1997 - To use the new tokenizer in AFIleio!
*             15 Apr 1997 - To return if vardelim is in first col too
*              2 Apr 2001 - Converted back to TFM... going round and round.
*******************************************************************************
*/
void FMnofmt( )
{
 char *buf;             /* work buffer */
 int status;            /* status of the read */
 int i;                 /* loop index */

 status = FMread( inbuf );        /* get the next buffer */

 while( status >= 0  &&  inbuf[0] != CMDSYM && *inbuf != vardelim ) 
  {
   for( i = 0; i < MAX_READ-1 && inbuf[i] != EOS; i++, optr++ )
    {
     switch( inbuf[i] )          /* properly escape html special chars */
      {
#ifdef NEVER_ME
       case '>':
        obuf[optr] = EOS;                 /* terminate for strcat */
        strcat( obuf, "&gt;" );           /* copy in the character */
        optr += 3;                        /* incr past the html esc string */
        osize += 3;                       /* incr number of chars in output */
        break;

       case '<':
        obuf[optr] = EOS;                 /* terminate for strcat */
        strcat( obuf, "&lt;" );           /* copy in the character */
        optr += 3;                        /* incr past the html esc string */
        osize += 3;                       /* incr number of chars in output */
        break;

       case '&':
        obuf[optr] = EOS;                 /* terminate for strcat */
        strcat( obuf, "&amp;" );          /* copy in the character */
        optr += 4;                        /* incr past the html esce string */
        osize += 4;                       /* incr number of chars in output */
        break;
#endif

       default:                 /* not a special character - just copy in */
        obuf[optr] = inbuf[i];
        obuf[optr+1] = EOS;     /* terminate incase of strcat on next loop */
        break;
      }
    }                          /* end while stuff in input buffer to copy */

   if( optr == 0 )             /* if this was a blank line */
    obuf[optr++] = ' ';        /* give flush a blank to write */
   obuf[optr] = EOS;           /* terminate buffer for flush */

   FMflush( );                 /* send the line on its way */
   status = FMread( inbuf );   /* get the next line and loop back */
  }                            /* end while */

 AFIpushtoken( fptr->file, inbuf );   /* put command back into input stack */
 iptr = optr = 0;              /* return pointing at beginning */
}                              /* FMnofmt */
