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
*   Mnemonic: FMgettok
*   Abstract: This routine gets the next token from the input file buffer
*             and returns its length.
*   Parms:    buf - pointer to a buffer pointer that is to point at the token
*                   upon the return from this routine
*   Returns:  The length of the token or 0 if at end of file.
*   Date:     16 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 7-07-89 - To support variable expansion.
*             9-01-89 - To support user defined variable delimiter
*             4-13-93 - To allow a quoted string as one token
*             4-03-97 - To use AFI tokenizing support!
*		22 Oct 2007 - Fixes to backquoting token.
****************************************************************************
*/
int FMgettok( char **buf )
{
	int 	len = ERROR;     /* length of buffer read from input file */
	int 	i;               /* loop index */
	int 	j;
	char 	*cp;
	char	exbuf[2048];	/* extra work buf */


	*buf = inbuf;        /* new token is placed at start of the input buffer */
	**buf = 0;           /* ensure eos at start */

	while( (len = AFIgettoken( fptr->file, inbuf )) == 0 );  /* gettok or eof */

	if( *inbuf == '`' )
	{
     		AFIsetflag( fptr->file, AFI_F_WHITE, AFI_SET ); 

		i = 0;
		cp = inbuf + 1;			/* skip lead bquote */
		do
		{
			if( i + len >= sizeof( exbuf ) )
			{
				exbuf[50] = 0;
				fprintf( stderr, "buffer overrun detected in gettoken: %s...\n", exbuf );
				exit( 1 );
			}

			for( ; *cp && *cp != '`'; cp++ )
			{
				if( *cp == '^'  &&  *(cp+1) == '`' )
					cp++;					/* skip escape and put in bquote as is */
				exbuf[i++] = *cp;	
			}

			exbuf[i] = 0;

			if( *cp == '`' )
				break;

			cp  = inbuf;						/* next token will be here */
		}
		while( (len = AFIgettoken( fptr->file, inbuf)) >  0 );


		strcpy( inbuf, exbuf );
		*buf = inbuf;
		len = strlen( inbuf );
		TRACE( 2, "getparm returning backquoted parameter: len=%d (%s)\n", len, inbuf );
	
       		AFIsetflag( fptr->file, AFI_F_WHITE, AFI_RESET ); 
	}


	TRACE( 4, "fmgettok: returning(%s)\n", inbuf );
	 return( len );
}      
