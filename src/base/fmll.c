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
*   Mnemonic:  FMll
*   Abstract:  This routine resets the line length.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      17 November 1988
*   Author:    E. Scott Daniels
*
*   .ll  n | +n | -n  (default if n omitted is 65)
*
*   Modified:	01 Jul 1994 - To convert to rfm
*				08 Jul 2013 - Better error message.
***************************************************************************
*/
void FMll( )
{
	char *buf;          /* pointer at the token */
	char obuf[256];
	int len;

	if( (len = FMgetparm( &buf )) > 0 )  /* if user supplied parameter */
	{
		len = FMgetpts( buf, len );          /* convert it to points */

		if( buf[0] == '+' || buf[0] == '-' )
			len = len + linelen;          /* user wants parameter added/subed */

		if( len > cur_col->width || len <= 0 )   /* if out of range */
		{
			snprintf( obuf, sizeof( obuf ), "%s takes ll to %d; colwidth=%d", buf, len, cur_col->width );
			FMmsg( W_LL_TOOBIG, obuf );
		}
		else
		{
			linelen = len;                          /* set the line length */
			len = cur_col->width - (linelen + (lmar-cur_col->lmar));
		}
	}
}               /* FMll */
