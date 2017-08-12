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
****************************************************************************
*
*    Mnemonic: AFIchain
*    Abstract: This routine chains a newly opened file with an existing
*              file (that is also open). This is ideal for file merging.
*    Parms:    file - File number of open file to chain to.
*              fname - Name of the file to open.
*              opts - options for open. If first character is p assume next
*                     parameter is present and dont pass p to AFIopen.
*              path - Path string to pass to AFIopenp if p is in options.
*    Returns:  AFI_ERROR if unable to chain the file, AFI_OK if successful.
*    Date:     12 Feb 1989 - Converted to be part of the AFI routines 12/10/92
*    Author:   E. Scott Daniels
*
*    Modified: 4 Jan 1994 - To correct call to open
*              8 Jan 1995 - To call AFIopenp if p is in options string.
*             28 Mar 1997 - To support tokenized input streams
*				12 Aug 2017 - Convert name to allocaed string.
****************************************************************************
*/
#include "afisetup.h"        /* include all needed header files */

int AFIchain( int file, char *fname, char *opts, char *path )
{
 struct file_block *ptr;        /* pointer at the existing file */
 struct file_block *tptr;       /* pointer at chained file block */
 int tfile;                     /* temp file number returned by open */

 if( file < 0  ||  file > MAX_FILES )
  return( AFI_ERROR );

 if( (ptr = afi_files[file]) == NULL )
  return( AFI_ERROR );

 if( *opts == 'p' )                /* assume path parm passed in */
  {
   opts++;                 /* dont pass p option to the open routine */
   tfile = AFIopenp( fname, opts, path );  /* open using path string */
  }
 else
  tfile = AFIopen( fname, opts );      /* open the requested file */

 if( tfile < AFI_OK )
  return( AFI_ERROR );

 tptr = afi_files[tfile];      /* get pointer to file block for new file */
 tptr->chain = ptr;            /* chain in front of existing file block */

 tptr->flags = ptr->flags;     // absorbe properties of the already open file
 tptr->flags &= ~F_STDIN;      /* cannot reopen the standard input file */
 tptr->max = ptr->max;

	if( tptr->name == NULL ) {
		tptr->name = strdup( fname );		// should have been saved by open, but chance not
	}

 if( ptr->tmb != NULL )         /* file was setup for tokenized input */
  {                             /* so do the same and copy existing environ */
   AFIsettoken( tfile, ptr->symtab, ptr->tmb->tsep, ptr->tmb->varsym, ptr->tmb->escsym, 
                ptr->tmb->psep );
  }

 afi_files[file] = afi_files[tfile];    /* put new block at head of open list */
 afi_files[tfile] = NULL;           /* no longer need this slot in file list */

 return( AFI_OK );              /* ok - lets go */
}                               /* AFIchain */
