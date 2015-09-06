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
*  Mnemonic: AFIsetflag
*  Abstract: This routine allows the user to set the flag values in the
*            file block.
*            here.
*  Parms:    file - File handle of the open file
*            flag - Bitmask flags AFI_F contained in AFI defs file.
*            opt  - If true (AFISET) then flags are ORd. If false (AFIRESET)
*                   then ~flag is ANDed with the current value.
*  Returns:  Nothing.
*  Date:     29 December 1993
*  Author:   E. Scott Daniels
*
*  Modified:  5 Jan 1994 - To set flag on all chained files too.
*******************************************************************************
*/
#include "afisetup.h"   /* get necessary headers */

void AFIsetflag( file, flag, opt )
 int file;
 int flag;
 int opt;
{
 struct file_block *fptr;     /* pointer at file representation block */

 if( file >= 0 && file <= MAX_FILES )
  for( fptr = afi_files[file]; fptr != NULL; fptr = fptr->chain )
   {
    if( opt )                 /* set the flags */
     fptr->flags |= flag;
    else
     fptr->flags &= ~flag;    /* reset the flag */
   }
}                             /* AFIsetsize */
