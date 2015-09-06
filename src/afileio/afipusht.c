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
*****************************************************************************
*
*  Mnemonic: AFIpushtoken
*  Abstract: This routine will push a buffer onto the token management
*            input list. The caller can push an empty buffer onto the
*            stack if buf is NULL.
*  Parms:    file - The afi file handle of the open file
*            buf  - Pointer to the EOS terminated buffer to push
*  Returns:  AFI_OK if all is well, AFI_ERROR if bad file or other error.
*  Date:     28 March 1997
*  Author:   E. Scott Daniels
*
******************************************************************************
*/
#include "afisetup.h"
#include	"parse.h"

int AFIpushtoken( AFIHANDLE file, char *buf )
{
	int status = AFI_ERROR;       /* status of local processing */
	struct file_block *fptr;      /* pointer at the file structure */
	struct inputmgt_blk *imb;     /* pointer at the new input management block */

	if( (fptr = afi_files[file]) != NULL &&   /* valid file? */
		fptr->tmb != NULL              )  /* and setup for tokenized input */
	{
		fptr->flags &= ~F_EOF;              /* technically not eof yet */

		if( (imb = (struct inputmgt_blk *) AFInew( INPUTMGT_BLK )) != NULL )
		{
//fprintf( stderr, "afipushtoken: %s (%s)\n", fptr->name, buf );
			if( buf != NULL )         /* user can cause an empty blk to be added */
			{                        /* mostly for the afi internal rtns */
				imb->buf = strdup( buf );
				imb->idx = imb->buf;
				imb->end = imb->buf + strlen( buf );   /* set up indexes into buffer */
				imb->next = fptr->tmb->ilist;          /* push it on the stack */
				fptr->tmb->ilist = imb;
			}

			status = AFI_OK;                  /* set good return code and scram */
		}                            /* end if imb allocated ok */
	}                              /* end if file ok and setup for tokens */

	return( status );
}                                        /* AFIpushtoken */
