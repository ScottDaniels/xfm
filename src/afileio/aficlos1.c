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
***************************************************************************
*
*    Mnemonic: AFIclose1
*    Abstract: This routine is responsible for closing the first file
*              in the chain associated with the file number passed in.
*              If the "file" is a pipe both read and write ends of the
*              pipe are closed and the associated pipe block destroyed.
*    Parms:    file - Number of the file that is to be closed.
*    Returns:  VALID.
*    Date:     15 Feb 1989 - Converted to AFI 12/10/92
*    Author:   E. Scott Daniels
*
*    Modified: 11 Mar 1997 - To handle pipes
*              28 Mar 1997 - To handle tokenized input mgt blocks
*				10 Apr 2007 - Now free tsep.
*				12 Aug 2017 - Support dynamically allocated filenames.
*
***************************************************************************
*/
#include "afisetup.h"     /* get necessary headers */

int AFIclose1( int file )
{
	struct file_block *ptr;      /* pointer at the block representing the file */
	struct inputmgt_blk *imb;    /* pointer at the input mgt blk to free */

	ptr = afi_files[file];           /* get pointer to the file block */
	
	if( ptr == NULL )            /* bad file number passed in ? */
		return( AFI_ERROR );

	afi_files[file] = ptr->chain;     /* point at remaining if they are there */
	
	if( ptr->tmb != NULL )        /* token management block there? */
	{
		while( ptr->tmb->ilist != NULL )  /* while input mgt blocks on the list */
		{
			imb = ptr->tmb->ilist;          /* hold one to free */
			ptr->tmb->ilist = imb->next;
			free( imb->buf );
			free( imb );                     /* trash the storage */
		}

		if( ptr->tmb->psep )
			free( ptr->tmb->psep );          /* free the parameter seperator list */
		if( ptr->tmb->tsep )
			free( ptr->tmb->tsep );          /* free the parameter seperator list */

		free( ptr->tmb );                /* trash the token mgt blk */
	}                                  /* end if token management on the file */

	if( ptr->flags & F_PIPE && ptr->pptr != NULL )     /* valid  pipe ? */
	{
		close( ptr->pptr->pipefds[READ_PIPE] );      /* close both descriptors */
		close( ptr->pptr->pipefds[WRITE_PIPE] );

		if( ptr->pptr->cache != NULL );            /* some pipes dont cache */
			free( ptr->pptr->cache);                  /* trash the cache */
		free( ptr->pptr );                         /* trash the pipe block */
	}
	else
		if( ptr->file != NULL )                     /* valid file pointer */

	if( ptr->name ) {
		free( ptr->name );
	}

	fclose( ptr->file );                       /* do the close */

	free( ptr );                                 /* free the buffer */

	return( AFI_OK );
}                                             /* AFIclose1*/
