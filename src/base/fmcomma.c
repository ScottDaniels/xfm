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
*  Mnemonic: FMcomma
*  Abstract: take the next token and add commas assuming it is a number:
*		12345678 ==> 12,345,678
*  Returns:  Pushes token back onto the stack
*  Date:     26 Feb 2007
*  Author:   E. Scott Daniels
*
*  Modified: 
****************************************************************************
*/
void FMcomma( )
{
	char *buf;               /* parameter pointer */
	char	*p;
	int len;                 /* length of the parameter */
	int i;                   /* index vars */
	int j;
	int n;			/* number of commas needed */
	int nxt;		/* number of chars before next comma */
	char wbuf[1024];

	len = FMgetparm( &buf );     
	if( len <= 0 )
		return;                     /* no parm, exit now */

	if( (p = index( buf, '.' )) )
	{
		n = ((p-buf)-1)/3;
		nxt = (p-buf) - (n * 3);
	}
	else
	{
		n = (len-1) / 3;
		nxt = len - (n * 3);
	}

	j = 0;
	for( i = 0; i < len; i ++)
	{
		wbuf[j++] = buf[i]; 
		if( --nxt == 0 && n )
		{
			n--;
			nxt = 3;
			wbuf[j++] = ',';
		}
	}

	wbuf[j] = 0;
	AFIpushtoken( fptr->file, wbuf );   /* push the command on input stack */

	
}                         
