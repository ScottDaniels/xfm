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
*    Mnemonic: FMopen
*    Abstract: This routine processes an imbed statement and opens a new
*              file to process. It is also used by index generation and maybe
*		other functions. A file is opened and its info is stacked. 
*		the original purpose of this routine was to stack files and 
*		not to present and end of file until the last file on the stack
*		reached the end of file. The AFI routines now provide a generic
*		stack interface and now this function's responsibilities are 
*		significantly reduced.
*		file names for error messages. 
*    Parms:    name - Pointer to the name of the file to open
*    Returns:  ERROR if unable to open the file, otherwise VALID
*    Date:     15 November 1988
*    Author:   E. Scott Daniels
*
*    Modified: 16 Mar 1990 - Not to use FI routines.. there is a bug in
*                            them someplace.
*              10 Dec 1992 - To use AFI routines (which use ansi std fread)
*               3 Apr 1997 - To begin phase out of this routine.
*	       19 Oct 2001 - Cleanup - finish work started on in 97
*	        5 Feb 2001 - To save name 
*		21 Oct 2007 - Some general doc cleanup.
**************************************************************************
*/
int FMopen( char *name )
{
	int i;                   /* loop index */

	if( fptr == NULL )            /* if first file opened */
	{
		fptr = (struct fblk *) malloc( sizeof( struct fblk ) );
		if( fptr == NULL )
		{
			FMmsg( E_NOMEM, "FMopen" );
			return( ERROR );
		}

		memset( fptr, 0, sizeof( struct fblk ) );
		fptr->fnum = 0;
		if( (fptr->file = AFIopenp( name, "r", path )) == AFI_ERROR )
		{
			FMmsg( E_CANTOPEN, name );     
			free( fptr );	
			fptr = NULL;
			return( ERROR );
		}
		
		AFIsetsize( fptr->file, MAX_READ-1 );       /* input buffer size */
		strcpy( fptr->name, name );
	}
	else
	{
		if( AFIchain( fptr->file, name, "pr", path ) == AFI_ERROR )
		{
			FMmsg( E_CANTOPEN, name );     
			return( ERROR );
		}
		fptr->fnum = 0;
	}


	fptr->count = 0l;                  /* set record counter */

	return OK; 		/* beam us up scotty */
}			                  /* FMopen */
