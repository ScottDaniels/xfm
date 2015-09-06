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


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
****************************************************************************
*
*  Mnemonic: FMelse
*  Abstract: This routine is called when the .ei command is encountered
*            during normal processing. If it is encountered then we were
*            processing the true portion of an if statement and we need
*            to skip the else section of text which is exactly what this
*            routine does. It will stop when the .fi command si found.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     20 July 1994
*  Author:   E. Scott Daniels
*
*  Modified: 27 Jul 1994 - To allow nested ifs in else clauses
*****************************************************************************
*/
void FMelse( )
{
 char *tok;              /* pointer to the token */
 int nested = 0;         /* number of nested levels we must resolve */

 while( FMgettok( &tok ) > OK )      /* while tokens still left in the file */
  {
   if( strncmp( ".if", tok, 3 ) == 0 )  /* nested if statement */
    nested++;                           /* increase number of fis to find */
   else
    if( strncmp( ".fi", tok, 3 ) == 0 )  /* found end if */
     {
      if( nested == 0 )                  /* if this is the last one to find */
       return;                            /* then get out */
      else
       nested--;                         /* else - just dec number looking fr*/
     }
  }            /* end while - if we drop out then unexpected end of file */

 FMmsg( E_UNEXEOF, ".ei not terminated" );
}    /* FMelse */

