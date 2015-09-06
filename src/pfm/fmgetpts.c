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
*  Mnemonic: FMgetpts
*  Abstract: This routine will convert a valued entered by the user into
*            points. The token passed in is assumed to be a number that may
*            be postfixed with a 'p' (points) or an i (inches). If the value
*            is in inches (postfix i) then the value may be a real number.
*  Parms:    tok - Pointer to token to look at
*            len - Length of the token to look at
*  Returns:  Number of points that the token converted to.
*  Date:     6 April 1994
*  Author:   E. Scott Daniels
*
*****************************************************************************
*/
int FMgetpts( tok, len )
 char *tok;
 int len;
{
 int pts = 0;         /* points to return */
 double fval;          /* value for converting to float */

 switch( tok[len-1] )   /* look at last character of the token */
  {
   case 'i':                    /* units are inches */
   case 'I':
    fval = atof( tok );         /* convert to float */
    fval = fval * 72;           /* convert to floating points */
    pts = fval;                 /* convert to integer points */
    break;

   case 'p':                    /* user entered points */
   case 'P':
    pts = atoi( tok );          /* direct conversion to points */
    break;

   default:                     /* nothing specified - assume characters */
    pts = atoi( tok );          /* convet to integer */
    pts *= PTWIDTH;             /* convert to points */
    break;
  }  /* end switch */

 return( pts );           /* and send back calculated value */
}                     /* FMgetpts */
