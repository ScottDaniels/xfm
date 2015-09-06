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
*  Mnemonic: AFIsettoken (afisetto.c)
*  Abstract: This routine is reponsible for conveting the open AFI file 
*            to a tokenized file. After conversion, calls to AFI read
*            will return only the next token from the input stream to 
*            the caller. The caller may also supply a variable symbol
*            which is used to identify and expand variables encountered
*            in the token stream and an escape symbol which can be used
*            to escape special symbols. 
*  Parms:    file   - the AFI file handle of the file to convert
*            tsep   - Pointer to characters that define the start of a new
*                     token and are returned when encountered as a single
*                     character token.
*            varsym - The character used to indicate a variable to try to 
*                     expand.
*            escsym - The character used to force the next character to 
*                     be ignored by the tokenizer.
*            psep   - Parameter seperation character string for UTformat calls 
*            findvar- Pointer to user routine that can be called to look
*                     up a variable name and returns a buffer of characters
*                     that the variable expands to.
*            finddata-Pointer to data that will be passed to the find routine
*  Returns:  AFI_OK if all went well, AFI_ERROR if there was a problem
*  Date:     28 March 1997
*  Author:   E. Scott Daniels
*
* Modified: 	11 Jan 2002 - to add new parse/expansion with symbol table
*		10 Apr 2007 - Fixed memory leak.
***************************************************************************
*/

#include "afisetup.h"

int AFIsettoken( AFIHANDLE file, Sym_tab *st, char *tsep, char varsym, char escsym, char *psep )
{
 int status = AFI_ERROR;             /* status of processing here */
 struct tokenmgt_blk *tptr;          /* pointer at token management block */
 struct file_block *fptr;            /* pointer at file block */

 if( (fptr = afi_files[file]) != NULL )  /* valid file */
  {
	if( st )
		fptr->symtab = st;

   if( fptr->tmb == NULL )           /* no token block yet */
    fptr->tmb = (struct tokenmgt_blk *) AFInew( TOKENMGT_BLK );

   if( fptr->tmb != NULL )           /* allocated successful or there */
    {
	
	if( fptr->tmb->psep )
		free( fptr->tmb->psep );
	fptr->tmb->psep = strdup( psep );

	if( fptr->tmb->tsep )
		free( fptr->tmb->tsep );
     fptr->tmb->tsep = strdup( tsep );

     fptr->tmb->varsym = varsym;
     fptr->tmb->escsym = escsym;       /* fill out with user stuff */

     if( fptr->tmb->ilist == NULL )        /* need an ilist? */
      fptr->tmb->ilist = (struct inputmgt_blk *) AFInew( INPUTMGT_BLK );

     if( fptr->tmb->ilist != NULL )
      status = AFI_OK; 
    }
  }                                    /* end if file is ok */
  
 return( status );
}                      /* AFIsettoken */
