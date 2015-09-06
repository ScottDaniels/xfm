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
*************************************************************************
*
*  Mnemonic: AFInew
*  Abstract: This routine is the central creation point for structures
*            used by the AFI routines.
*  Parms:    type - Type of structure that is to be allocated and
*                   initialized.
*  Returns:  The pointer to the structure, or NULL if an allocation error
*  Date:     28 March 1997
*  Author:   E. Scott Daniels
*  Mods:
*		10 Apr 2007 - Memory leak cleanup
*
**************************************************************************
*/

#include "afisetup.h"

void *AFInew( int type )
{
 struct inputmgt_blk *iptr;   /* pointers of things we can allocate */
 struct tokenmgt_blk *tptr;
 void *rptr = NULL;           /* pointer for return value */


 switch( type )
  {
   case INPUTMGT_BLK:
     iptr = (struct inputmgt_blk *) malloc( sizeof( struct inputmgt_blk ));
     if( iptr != NULL )
      {
	memset( iptr, 0, sizeof( *iptr ) );
       iptr->flags = 0;
       iptr->next = NULL;
       rptr = (void *) iptr;           /* convert for return */
      }
     break;

   case TOKENMGT_BLK:
     tptr = (struct tokenmgt_blk *) malloc( sizeof( struct tokenmgt_blk ) );
     if( tptr != NULL )
      {
	memset( tptr, 0, sizeof( *tptr ) );
       tptr->varsym = (char) -1;        /* default to something not likely */
       tptr->escsym = (char) -1;
       tptr->psep = strdup( ":" ); 
       tptr->ilist = NULL;
       rptr = (void *) tptr;
      }
     break;
  }                       /* end switch */

 return( rptr );

}                       /* AFInew */
