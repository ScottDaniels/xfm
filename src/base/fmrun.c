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
* Mnemonic:	fmrun
* Abstract:	Drives input until the end of file is reached.
* 		Added to support imbed as an oneject command.
*		Allows an imbed command to be processed before
*		the rest of the tokens on the command line.
* Date: 	18 Nov 2001
* Author: 	E. Scott Daniels
*		  		10 Apr 2007 - Memory leak cleanup 
*				17 Jul 2016 - Changes for better prototype generation.
*
* -------------------------------------------------------------
*/

extern void FMrun( void )
{
	char	*buf = NULL;
	int	len = 1;
	int	i = 0;
	char	b[2048];

	TRACE( 1,  "run: starts\n" );
	while( len >= 0 ) 
	{
		if( flags & NOFORMAT )
			FMnofmt( );
		else
			if( flags2 & F2_ASIS )
				FMasis( );

		if( (len = FMgettok( &buf )) > 0 )
		{
			TRACE( 2,  "run: (%s) lmar=%d\n", buf, lmar );

			if( len == 3 && *buf == CMDSYM && *(buf+3) == 0 )
			{
				switch( FMcmd( buf ) )			
				{
					case 0: 	FMaddtok( buf, len ); 			/* not a command -- add to output */
							break;

					case 1: 	break;					/* normal command handled -- continue */

					default: 	TRACE( 2, "run: pop run stack\n" );
							return;
							break;
				}
			}
			else
				FMaddtok( buf, len );
		}
	}
	TRACE( 1,  "run: stops  len=%d\n", len );
}
