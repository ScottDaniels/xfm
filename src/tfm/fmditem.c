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
****************************************************************************
TFM
*
*   Mnemonic:  FMditem
*   Abstract:  This routine handles a definition item (.di) command.
*              The left margin is moved left to the start of the term area,
*              and the line length is set to the term size (points). Then
*              addtoken is called to put in the tokens, moving to the next
*              line if necessary. We fake it into thinking that the term
*              area is all that we can write to by adjusting the line len.
*              This routine will process all the tokens on the .di line or
*              until the colon (:) stop token is encountered.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      1 January 1989 (Happy New Year!)
*   Author:    E. Scott Daniels
*   Modified:   1 Jul 1994 - To convert to rfm.
*               6 Dec 1996 - To convert to hfm
*              30 Apr 1997 - To convert to use tables instead of dd/di
*              23 Mar 2001 - Back to the egg; changes for a new TFM
*              19 Apr 2001 - To fix extra space when term was size-1 in len.
* 	       10 Feb 2002 - To support skip (does not support format option 
*    				because of fixed width and justification (right)
*				issues.
*
*   Copyright (c) 1994  E. Scott Daniels. All rights reserved.
***************************************************************************
*/
void FMditem( )
{
 char *buf;           /* pointer at the token */
 char fmt[10];
 int len;             /* len of parameter entered */
 char tbuf[256];      /* buffer to build temp string in */
 char term[512];
 char *align = " ";
 int i;
 int tlen = 0;         /* length of the entire term */

 if( dlstackp < 0 )   /* if no stack pointer then no list in effect */
  {
   FMmsg( E_NO_DEFLIST, NULL );
   return;
  }


 FMflush( );                 /* put out whats in the buffer */
 if( dlstack[dlstackp].skip )
 {
	AFIpushtoken( fptr->file, "1" );
	FMspace( );
 }

 if( di_term )               /* just like pampers; no leaks */
  {
   free( di_term );
   di_term = NULL;
  }


 flags2 |= F2_DIBUFFER;     /* flush will adjust margin correctly */

 tbuf[0] = 0;

 switch( dlstack[dlstackp].anum )
  {
   case DI_ANUMA:                  /* automatic alpha "numbering */
     if( flags2 & F2_RIGHT )      /* align term to the right? */
      sprintf( fmt, "%%%dc ", (dlstack[dlstackp].indent/7)-1 );
     else
      sprintf( fmt, "%%-%dc ", (dlstack[dlstackp].indent/7)-1 );

     sprintf( tbuf, fmt, dlstack[dlstackp].astarta + dlstack[dlstackp].aidx);
 
     di_term = strdup( tbuf );

     if( ++dlstack[dlstackp].aidx > 26 )
      dlstack[dlstackp].aidx = 0;

     while( (len = FMgetparm( &buf )) > 0 );  /* skip any parms put in */
     break;

   case DI_ANUMI:                  /* automatic integer numbering */
     if( flags2 & F2_RIGHT )      /* align term to the right? */
      sprintf( fmt, "%%%dd ", (dlstack[dlstackp].indent/7)-1 );
     else
      sprintf( fmt, "%%-%dd ", (dlstack[dlstackp].indent/7) - 1 );

     sprintf( tbuf, fmt, dlstack[dlstackp].astarti + dlstack[dlstackp].aidx);
     di_term = strdup( tbuf );

     sprintf( tbuf, ".dv _dinum %d : ",
          dlstack[dlstackp].astarti + dlstack[dlstackp].aidx );

     dlstack[dlstackp].aidx++;
     while( (len = FMgetparm( &buf )) > 0 );  /* skip any parms put in */

     AFIpushtoken( fptr->file, tbuf );   /* must be last */
     break;

   default:
     while( tlen < 255 && ((len = FMgetparm( &buf )) > 0) )   
      {
       strcat( tbuf, buf );
       strcat( tbuf, " " );
       tlen += len + 1;
      }

     if( tlen <= dlstack[dlstackp].indent/7 )
      {
	if( flags2 & F2_RIGHT )     /* align term to the right? */
		sprintf( fmt, "%%%ds", (dlstack[dlstackp].indent/7)-1 );
	else
		sprintf( fmt, "%%-%ds", dlstack[dlstackp].indent/7 );
       sprintf( term, fmt, tbuf, " " );
       di_term = strdup( term );
      }
     else
      {
       FMaddtok( " ", 1 );       /* must have something in there to dump */
       di_term = strdup( tbuf );
       
       FMflush( );        /* larger than life, put out & start next indented */
      }
     break;
  }
}                               /* FMditem */
