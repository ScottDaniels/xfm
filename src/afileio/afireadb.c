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
***** NOTE: This routine is obsolite as the AFIsetsize routine allows
*           the size to be set.
****************************************************************
*
*    Mnemonic:   aFIread
*    Abstract:   This routine reads characters from the file assoicated
*                with the file number passed in as does the AFIread rtn.
*                This routine allows the user to specify the number of
*                bytes to be placed into the buffer. This read routine is
*                unlike AFIread and AFIreadn insomuch as it will not
*                terminate the read when a newline (0x0a) is encountered.
*                The buffer is NOT converted into a string by adding
*                an end of string (\000) character.
*                The file should be opened in binary mode.
*    Parameters: file - AFI file number to read from
*                rec  - callers buffer to fill
*                n    - Number of bytes to read.
*    Returns:    The number of characters placed in the string, ERROR if
*                end of file encountered (no valid info in the buffer).
*    Date:       12 January 1993
*    Author:     E. Scott Daniels
*
****************************************************************
*/
#include "afisetup.h"
int AFIreadb( file, rec, n )
 int file ;
 char *rec;
 int n;
{
 struct file_block *fptr;     /* pointer at file representation block */
 int lastlen;                 /* last length returned by fread */

 fptr = afi_files[file];        /* get the pointer to the file block */
 if( fptr == NULL)
  return( -1 );

 lastlen = fread( rec, 1, n, fptr->file );   /* read bytes */

 if( lastlen == 0 )   /* then were at the end of the file */
  return( -1 );

 return( n );         /* return number of characters actually read */
}                                       /* AFIread */
