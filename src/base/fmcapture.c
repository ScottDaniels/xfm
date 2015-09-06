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
***************************************************************************
*
*  Mnemonic: 	FMcapture
*  Abstract: 	capture all lines until .ca end command is noticed
*		lines are written to the file named on the command 
*  Parms:   
*  Returns:  	Nothing.
*  Date:     	06 November 2007
*  Author:   	E. Scott Daniels
*  Modified: 
*
*  Command syntax:  .ca {start filename|end}
*
*****************************************************************************
*/

void FMcapture( )
{
	FILE 	*f = NULL;
	char 	*fname;
	char 	*buf;
	char	*cp;
	int	i = 0;

   	if( FMgetparm( &buf) != 0  )
	{
		if( strcmp( buf, "end" ) == 0 )			/* should not happen */
			return;

		if( strcmp( buf, "start" ) == 0 )
			FMgetparm( &buf );

		fname = strdup( buf );	
		if( (f = fopen( fname, "w" )) == NULL )
			FMmsg( E_CANTOPEN, fname );			/* we skip what was to be captured, so continue */

		while( FMread( inbuf ) >= 0 )
		{
			if( inbuf[0] == CMDSYM )			/* must find .ca end in first spot */
			{
				if( inbuf[1] == 'c' && inbuf[2] == 'a' )  
				{
					for( cp = &inbuf[3]; *cp && (*cp == ' ' || *cp == '\t'); cp++ );
					if( *cp && strncmp( cp, "end", 3 ) == 0 )
					{
						iptr = (cp+3) - inbuf;		/* should not be anything, but leave pointed past end */
						if( f )
							fclose( f );
						TRACE( 1, "capture: %d lines captured in %s\n", i, fname );
						free( fname );
						return;                                     /* get out now */
					}
				}
    			} 

			if( f )
				fprintf( f, "%s\n", inbuf );

			i++;
		}
	}
}
