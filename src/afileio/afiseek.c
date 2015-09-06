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
*  Mnemonic: AFIseek
*  Abstract: This routine will position the next read in the buffer such that
*            the data at the offset passed in is read. Prior to the seek flush
*            is called to ensure that any buffered data is written at the
*            proper position.
*  Parms:    file - The AFI file number (index into file list)
*            offset - Position to seek to (in bytes) relative to location
*            loc -  Location in the file (top bot or current) to base offset
*                   on.
*  Returns:  OK if successful, ERROR if not
*  Date:     28 January 1994
*  Author:   E. Scott Daniels
********************************************************************************
*/
#include "afisetup.h"  /* get necessary header files */

int AFIseek( file, offset, loc )
 int file ;
 long offset;
 int loc;
{
 struct file_block *fptr;     /* pointer at file representation block */

 fptr = afi_files[file];        /* get the pointer to the file block */
 if( fptr == NULL)
  return( AFI_ERROR );

 fflush( fptr->file );               /* flush before position cmd */

 fseek( fptr->file, offset, loc );

 fptr->flags &= ~F_EOF;               /* turn off the end of file */
 return( AFI_OK );                    /* send back number written */
}                                     /* AFIseek */
