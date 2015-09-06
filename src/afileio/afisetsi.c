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
*  Mnemonic: AFIsetsize
*  Abstract: This routine will set the buffer size of the file passed in
*            such that future reads will be at maximum the number inidcated
*            here.
*  Parms:    file - File handle of the open file
*            num  - User number to be used as max buffer size (should always
*                   be one more than the user wants characters back to allow
*                   for a terminating EOS character.
*  Returns:  The new maximum. If the number is out of range then we will
*            ignore it and just return the current max buffer size.
*  Date:     29 December 1993
*  Author:   E. Scott Daniels
*
*  Modified:  5 Jan 1994 - To set size on all chained files.
*******************************************************************************
*/
#include "afisetup.h"   /* get necessary include files */

int AFIsetsize( file, num )
 int file;
 int num;
{
 struct file_block *fptr;     /* pointer at file representation block */
 int max = 0;

 if( file >= 0 && file < MAX_FILES )
  {
   for( fptr = afi_files[file]; fptr != NULL; fptr = fptr->chain )
    if( num > 0 )
     fptr->max = num;
   max = afi_files[file]->max;
  }

 return( max );         /* return the current setting */
}                       /* AFIsetsize */
