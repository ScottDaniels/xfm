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
****************************************************************************
*
*   Mnemonic:  FMsetstr
*   Abstract:  This routine builds a string from the parameter list.
*              If the pointer passed in is NULL then the buffer is allocated.
*              If no more parameters exist on the input line then the string
*              is freed.
*              This routine is needed (as opposed to doing a string copy into
*              string) to expand variables and take literals (between ` `) by
*              using FMgetparm.
*   Parms:     string - Pointer to the pointer to the string to fill.
*              slen - Maximum string length
*   Returns:   Nothing.
*   Date:      6 June 1993
*   Author:    E. Scott Daniels
*	Mods:		17 Jul 2016 - Bring decls into the modern world.
*
***************************************************************************
*/
extern void FMsetstr( char **string, int slen )
{
 char *buf;          /* pointer at the token */
 char *lstring;      /* local pointer at the string */
 int i;              /* pointer into the token string */
 int j;              /* pointer into the footer buffer */
 int len;            /* length of the token */

 lstring = *string;
 if( lstring == NULL )
  lstring = *string = (char *) malloc( (unsigned) slen );

 len = FMgetparm( &buf );       /* get 1st parameter */
 if( len == 0 )
  {
   free( lstring );
   *string = NULL;         /* no parameter means no string */
   return;
  }

 j = 0;                   /* initiaize j to index into buffer at 0 */
 do                       /* put parameters into the buffer */
  {
   for( i = 0; i < len && j < slen-1; j++, i++ )
    lstring[j] = buf[i];
   len = FMgetparm( &buf );   /* get next parameter */
   if( len > 0 )              /* if another token on parameter line */
    lstring[j++] = ' ';       /* then seperate with a blank */
  }
 while( (len > 0)  && (j < slen-1) );  /* until max read or end of line */

 lstring[j] = EOS;      /* terminate the string */
}               /* FMsetstr */
