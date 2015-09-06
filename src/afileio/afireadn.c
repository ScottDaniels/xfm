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
**** NOTE: This routine is obsolite as the AFIsetsize routine allows the
*          size to be set.
****************************************************************
*
*    Mnemonic:   AFIread
*    Abstract:   This routine reads characters from the file assoicated
*                with the file number passed in as does the AFIread rtn.
*                This routine allows the user to specify the maximum
*                number of bytes to be placed into the buffer if
*                a line feed character is not enountered.
*                This routine assumes that
*                linefeed (newline) characters in the file and/or carriage
*                return linefeed character combinations in the file
*                indicate the end of a record. In either case either the
*                maximum number of characters (as set in the file block)
*                or characters up to the end of the line are placed in the
*                buffer. The buffer is converted into a string by adding
*                and end of string (\000) character. The linefeed and/or
*                carriage return, linefeed characters are not returned as
*                a part of the string.
*                The file can be opened in eithr "text" or binary mode.
*    Parameters: file - AFI file number to read from
*                rec  - callers buffer to fill
*                max  - Maximum number to read
*    Returns:    The number of characters placed in the string, ERROR if
*                end of file encountered (no valid info in the buffer).
*    Date:       25 June 1985 - Converted to AFI on 12/10/92
*    Author:     E. Scott Daniels
*
*    Modified:   29 Jan 1990 - To return partial line if eof detected.
****************************************************************
*/
AFIreadn( file, rec, max )
 int file ;
 char *rec;
 int max;
{
 struct file_block *fptr;     /* pointer at file representation block */
 int i;                       /* for loop index */
 int lastlen;                 /* last length returned by fread */

 fptr = afi_files[file];        /* get the pointer to the file block */
 if( fptr == NULL)
  return( ERROR );

 for( i = 0; i < max &&
             (lastlen = fread( &rec[i], 1, 1, fptr->file )) > 0 &&
             rec[i] != 0x0A; i++ );    /* read up to end, max or next lf */


 if( i == 0 && lastlen == 0 )   /* then were at the end of the file */
  return( ERROR );

 if( rec[i] == 0x0a )   /* if line terminated by new line */
  {
   if( i > 0 && rec[i-1] == 0x0d )  /* 0x0d 0x0a combination in the record */
    i--;                   /* back up to write over it too */
   rec[i] = EOS;         /* return to user with end of string */
  }
 else
  rec[++i] = EOS;       /* if not terminated with new line add eos at end */

 return( i );    /* return number of characters in string */
}                                       /* AFIread */
