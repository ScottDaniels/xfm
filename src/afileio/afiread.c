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
******************************************************************************
*
*    Mnemonic:   AFIread
*    Abstract:   This routine reads characters from the file assoicated
*                with the file number passed in and places them into the
*                buffer that is passed in. This routine assumes that
*                linefeed (newline) characters in the file and/or carriage
*                return linefeed character combinations in the file
*                indicate the end of a record if the file is not opened
*                with the binary flag in the options string or the binary
*                flag is not set with a call to AFIsetflag.
*                When in regular mode (not binary) the characters up to the
*                new line (or CR LF) are returned as a NULL terminated string
*                in the supplied buffer. If maxbuffer is reached prior to a
*                new line character then the entire buffer will contain
*                valid data and will be terminated by a NULL.
*                When in binary mode an entire buffer is returned to the
*                caller with a partial buffer returned only when the end of
*                file has been reached. NO substitutition for any characters
*                is done when in binary mode.
*    Parameters: file - AFI file number to read from
*                rec  - callers buffer to fill
*    Returns:    The number of characters placed in the string, ERROR if
*                end of file encountered (no valid info in the buffer).
*    Date:       25 June 1985 - Converted to AFI on 12/10/92
*    Author:     E. Scott Daniels
*
*    Modified:   29 Jan 1990 - To return partial line if eof detected.
*                29 Dec 1993 - To check for binary flag and handle right.
*                 2 Jan 1994 - To handle chained files correctly.
*                30 Jan 1994 - To support reading from stdin
*                11 Mar 1997 - To support pipes
*                 3 Apr 1997 - To count i/o operations
*		10 Apr 2007  - Fixed check for hard notification at eof of 
*			file in the chain.
*		31 Oct 2009 - corrected problem; test for queued msg buffer 
*			after closing a chained file had not reset the pointer.
****************************************************************************
*/
#include "afisetup.h"   /* get necessary include files */

int AFIread( file, rec )
 int file ;
 char *rec;
{
 struct file_block *fptr;     /* pointer at file representation block */
 int i;                       /* for loop index */
 int lastlen;                 /* last length returned by fread */

 fptr = afi_files[file];        /* get the pointer to the file block */
 if( fptr == NULL || (fptr->flags & F_EOF) )  /* if no pointer or at end */
  return( AFI_ERROR );

 fptr->operations++;                     /* up the # of i/o done */

 if( fptr->flags & F_PIPE )              /* file is really a pipe */
  return( AFIreadp( file, rec ) );       /* let read pipe routine handle */

 if( fptr->flags & F_BINARY )       /* if in binary mode read to max buffer */
  {
   for( i = 0; i < fptr->max &&
             (lastlen = fread( &rec[i], 1, 1, fptr->file )) > 0; i++ );
  }
 else                               /* assume regular ascii read */
  {
   for( i = 0; i < fptr->max && (lastlen = fread( &rec[i], 1, 1, fptr->file )) > 0 && rec[i] != 0x0A; i++ );    /* read up to end, max or next lf */
  }


	if( i == 0 && lastlen == 0 ) {       /* then were at the end of the file */
		if( fptr->chain == NULL ) {       /* if no more */
			fptr->flags |= F_EOF;
			return( AFI_ERROR );             /* then return error indication */
		} else {
			int state = fptr->flags & AFI_F_EOFSIG;		/* hold user request for hard notify on each chained file */
		
			AFIclose1( file );   			
			if( state ) {					/* user needs to know on each one */ 
				return -1;
			} else {
				fptr = afi_files[file];		/* get the new pointer to the file block */
				if( fptr->tmb )			/* if tokenising, return -1 as there may be unparsed tokens from the previous file  */
					return -1;
				else
					return( AFIread( file, rec ) );  /* and try to read again */
			}
		}
	}

 if( (fptr->flags & F_BINARY) == 0 )   /* if not binary */
  {
   if( rec[i] == 0x0a )                /* if line terminated by new line */
    {
     if( i > 0 && rec[i-1] == 0x0d ) {  /* 0x0d 0x0a combination in the record */
      i--;                            /* back up to write over CR as well */
	 }
     rec[i] = EOS;                    /* return to user with end of string */
    } else {
    	rec[i++] = EOS;       /* if not terminated with new line add eos at end */
	}
  }

 return( i );                       /* return number of characters in string */
}                                       /* AFIread */
