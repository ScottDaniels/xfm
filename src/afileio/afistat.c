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
*  Mnemonic: AFIstat
*  Abstract: This routine will return a particular piece of information
*            about the file that is represented by the handle passed in.
*  Parms:    file - File number - index into the files array
*            opt  - Opion that describes the information that is to be
*                   returned.
*            data - Pointer to pointer if non integer data to be returned.
*  Returns:  An integer that is: TRUE if the file is chained, or the
*            number of files in the chain, or the pointer to the name
*            of the file, or the number of files opened (chained files
*            count as 1 open file, or the flags associated with the file.
*  Date:     9 February 1994
*  Author:   E. Scott Daniels
*
*  Modified:  3 Apr 1997 - To support operations counter
*****************************************************************************
*/
#include "afisetup.h"  /* get necessary header files */

long AFIstat( file, opt, data )
 int file;
 int opt;
 void **data;
{
 long i;                   /* loop index - return value */
 long j;                   /* loop index */

 struct file_block *fptr;   /* pointer to the file info block */

 if( opt != AFI_OPENED )   /* we need to select the block if not num opened */
  {
   fptr = afi_files[file];        /* get the pointer to the file block */
   if( fptr == NULL )         /* if no pointer then exit with error */
    return( AFI_ERROR );
  }

 switch( opt )            /* do what user has asked for */
  {
   case AFI_CHAINED:                  /* return true if file chained */
    i = fptr->chain != NULL ? TRUE : FALSE;
    break;

   case AFI_CHAINNUM:       /* return number on the chain */
    for( i = 0; fptr->chain != NULL; fptr = fptr->chain, i++ );
    break;

   case AFI_NAME:
    *data = (void *) fptr->name;    /* return pointer to name string */
    i = AFI_OK;                     /* set to ok just in case */
    break;

   case AFI_OPENED:                       /* return number of open files */
    for( i = 0, j = 0; j < MAX_FILES; j++ )
     if( afi_files[j] )
      i++;           /* count open file */
    break;

   case AFI_FLAGS:
    i = fptr->flags;
    break;
 
   case AFI_OPS:                /* return operations done on this file */
    i = fptr->operations;
    break;

   default:
    i = AFI_ERROR;              /* indicate error */
  }                             /* end switch */

 return( i );                   /* return what ever we calculated */
}             /* AFIstat */

