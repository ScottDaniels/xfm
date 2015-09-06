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
*---------------------------------------------------------------------------
* Mnemonic: afiisvar
* Abstract: This routine will check a token to see if there is an 
*           un escaped variable in the token. It will return the addr
*           of the variable symbol if there is.
* Parms:    tok - pointer into tmb->ilist of start of token
*           tmb - Pointer to token management block
* Returns:  Pointer to first vc found or NULL.
* Date:     13 October 1997
* Author:   E. Scott Daniels
*
*---------------------------------------------------------------------------
*/
#include "afisetup.h"

char *AFIisvar( char *tok, struct tokenmgt_blk *tmb )
{
	char *vtok;                /* pointer at variable character in token */

	for( vtok = tok; vtok < tmb->ilist->end; vtok++ )
		if( *vtok == tmb->escsym )
			vtok++;				/* skip next - its escaped */
		else
			if( *vtok == tmb->varsym )
				return( vtok );
	return NULL;
}

#ifdef KEEP
char *AFIisvar( char *tok, struct tokenmgt_blk *tmb )
{
 char *vtok;                /* pointer at variable character in token */

 for( vtok = tok; vtok < tmb->ilist->end && *vtok != tmb->varsym; vtok++ )
  if( strchr( tmb->tsep, *vtok ) )
    return( NULL );                     /* end of token before var */
  

 if( vtok < tmb->ilist->end && 
     (vtok == tok  || *(vtok-1) != tmb->escsym) )     /* not escaped */
  return( vtok );

  return( NULL );
}
#endif

