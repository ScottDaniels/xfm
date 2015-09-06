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
*   Mnemonic: FMmsg
*   Abstract: This routine writes a message to the user console.
*             If the message number is negative then no message is
*             displayed. When the first character of a message is a +
*             or a * then the message is displayed regardless. If the
*             first character of the message is blank ( ) then the
*             message is displayed only if in verbose (noisy) mode.
*             The * character in the first column of a message causes
*             the current input file and line number to be appended to
*             the front of the message.
*   Parms:    msg - Message number of mesage to display.
*             buf - Non standard message to display with standard message
*   Returns:  Nothing.
*   Date:     16 November 1988
*   Author:   E. Scott Daniels
*
*   Modified:	11 Apr 1994 - To allow for "silent" operation
*				03 Apr 1997 - To get file info from AFI
*				15 Dec 2005 - To write messages to stderr rather than stdout
*				08 Jul 2013 - A bit of cleanup and to fix spacing in message. 
***************************************************************************
*/

  /* if the message text begins with a splat (*) then the current line number is also included */

 char *messages[ ] = {
 " FM000: Copyright (c) 1992-2013 E. Scott Daniels. All Rights reserved",
 " FM001: End of file reached for file: ",
 "*FM002: Too many files open. Unable to include: ",
 "*FM003: Unable to open file. File name: ",
 "*FM004: Name on command line is missing. ",
 " FM005: Imbedding file: ",
 "*FM006: Unable to allocate required storage. ",
 " FM007: Lines included from the file = ",
 " FM008: Required parameter(s) have been omitted from command line",
 "*FM009: Unknown command encountered and was ignored",
 "*FM010: Attempt to delete eject failed: not found",
 " FM011: Number of words placed into the document = ",
 "*FM012: Parameter takes margin out of range. ",
 "*FM013: Term length too large. Setting to 1/2 linelength",
 "*FM014: Left margin set less than term size, term ignored",
 "*FM015: Parameter takes line length past the current column width",
 " FM016: Unexpected end of input file",
 "*FM017: Parameter missing or out of range",
 "*FM018: %%boundingbox statement missing or invalid in file",
 "*FM019: Definition list not started. .di command ignored",
 "*FM020: Unrecognized parameter: ",
  "  "  };


void FMmsg( int msg, char *buf )
{
	char	wbuf[1024];
	char *ptr;              	/*  pointer at the message */
	char *name = NULL;			/* pointer to file name */
	long line = 0;				/* line number in current file */

	if( msg >= VALID )			/* if user wants a standard message */
	{
		if( fptr != NULL && fptr->file >= 0 )
		{
			AFIstat( fptr->file, AFI_NAME, (void **) &name );  	/* get the file name */
			line = AFIstat( fptr->file, AFI_OPS, NULL ); 		/* get the line number */

			if( !name )
				name = "stdin";
		}
		else
			name = "unknown-file";

		ptr = messages[msg];
		if( ptr[0] != ' ' || (flags2 & F2_NOISY) )
		{
			ptr = &ptr[1];              			/* skip the lead character */
			if( ptr[-1] == '*' && fptr != NULL )  	/* print line number too */
				snprintf( wbuf, sizeof( wbuf ), "(%s:%ld) %s: %s", name, line, ptr, buf ? buf : "" );
			else
				snprintf( wbuf, sizeof( wbuf ), "%s: %s: %s", name,  ptr, buf ? buf : "" );

			fprintf( stderr, "%s\n", wbuf );
		}
	}
	else
		if( buf != NULL )
			fprintf( stderr, "    %s\n", buf );       /* display only the user supplied buffer */
}                                     /* FMmsg */
