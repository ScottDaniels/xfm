/*
=================================================================================================
	(c) Copyright 1995-2011 By E. Scott Daniels. All rights reserved.

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
/*
*******************************************************************************
*
*  Mnemonic: AFIwrite
*  Abstract: This routine will write out the buffer that has been passed in to
*            the file that is described by the AFI file number passed in.
*            If the buffer to be written ends with a linefeed (\n) or a crlf
*            pair then nothing is done to the buffer. If the buffer doesnt end
*            with a linefeed then a line feed is added and the fwrite routine
*            is allowed to covert that to a crlf if the file is opened in
*            "text" mode. For files opened in binary mode, the max buffer size
*            is written unless the length parameter is not 0, then it is used.
*  Parms:    file - The AFI file number (index into file list)
*            buf  - The string that is to be written to the file
*  Returns:  The value that is retunred by fwrite which is currently the
*            number of items written. If this is less than 1 then an error
*            has occurred.
*  Date:     10 December 1992
*  Author:   E. Scott Daniels
*
*  Modified:  2 Jan 1994 - To process a stdout file correctly
*            28 Jan 1994 - To flush after each write if NOBUF flag set
*            11 Mar 1997 - To handle writing to pipes.
*             3 Apr 1997 - To up the operations coutner.
*		10 Apr 2007 - Memory leak repair
********************************************************************************
*/
#include "afisetup.h"

int AFIwrite( int file, char *rec )
{
	struct file_block *fptr;     /* pointer at file representation block */
	int len;                     /* length of string to write */
	int status;                  /* status of the users write */

	fptr = afi_files[file];          /* get the pointer to the file block */
	if( fptr == NULL)
		return( AFI_ERROR );

	fptr->operations++;                /* up the operations counter */
	
	if( fptr->flags & F_BINARY )       /* ignore contents of buffer and write */
		len =  fptr->max;                 /* use parm unless it is 0; this is silly, but difficult to change now-- grumble */
	else
		len = strlen( rec );          /* get the record length to write */

	if( len <= 0 )
		return 0;

	if( fptr->flags & F_STDOUT )  /* writing to stdout??? */
	{
		printf( "%s", rec );     /* just print it */
		status = len;            /* so we can let 'em know it went well */
	}
	else
	{
		if( fptr->flags & F_PIPE )         /* handle a pipe a bit differently */
			status = write( fptr->pptr->pipefds[WRITE_PIPE], rec, len );
		else
		{
			status = fwrite( rec, len, 1, fptr->file ); /* write users record out */
			if( status == 1 )        /* if one block written */
				status = len;           /* then we can assume that length was written */
		}
	}

	if( rec[len-1] != 0x0a && (fptr->flags & F_AUTOCR) == 1 && (fptr->flags & F_BINARY ) == 0 )
	{                                 		/* generate line feed if necessary */
		if( fptr->flags & F_STDOUT )
			printf( "\n" );
		else
			fwrite( "\n", 1, 1, fptr->file );     /* write the newline for them */
	}

	if( fptr->flags & F_NOBUF )      /* no buffering on write */
		fflush( fptr->file );           /* then flush them */

	return( status );                /* send back number written */
}                                 /* AFIwrite */
