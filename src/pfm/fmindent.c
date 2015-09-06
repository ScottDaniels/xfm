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
*   Mnemonic:  FMcindent
*   Abstract:  This routine causes a break and beginning with the next line
*              sets the left margin according to the parameter.
*   Parms:     mar - pointer to margin to adjust (heaer or left mar )
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Modified:   5 May 1992 - To support postscript conversion.
*              24 Mar 1994 - To allow amount to be specified in pts or inches
*               6 Apr 1994 - To use FMgetpts routine to convert value to pts
*                            so n can be postfixed with p (points) or i (in).
*   .in n | +n | -n  (indention not changed if n is omitted)
***************************************************************************
*/
void FMindent( mar )
 int *mar;
{
 char *buf;          /* pointer at the token */
 int len;
 int flags = 0;

 len = FMgetparm( &buf );    /* is there a length? */

 if( len == 0 )              /* no parameter entered by user */
  return;

 len = FMgetpts( buf, len );    /* convert value entered to points */

 if( buf[0] == '+' || buf[0] == '-' )  /* add/sub parm to/from cur setting */
  /*len = *mar + len; */            /* add pos/neg value to current setting */
  *mar += len;                     /* "add" current setting to value entered */
 else
  {                                  /* reset mar to specific vlaue */
   if( len < 0 || len > MAX_X )                 /* if out of range */
    {
     FMmsg( E_TAKEOUT, buf );
     return;
    }
   else
    *mar = cur_col->lmar + len;     /* base on the current column */
  }                    /* end else - +/- sign not there */


/* *mar = len;    */         /* reset the line length and go */
}                        /* FMindent */
