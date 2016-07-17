/*
All source and documentation in the xfm tree are published with the following open source license:
Contributions to this source repository are assumed published with the same license. 

=================================================================================================
	(c) Copyright 1995-2015 By E. Scott Daniels. All rights reserved.

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


#include <stdio.h>     
#include <stdlib.h>
#include <fcntl.h>    
#include <ctype.h>   
#include <string.h> 
#include <memory.h>
#include <time.h>

#include "symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"
#include "pfmproto.h"

/*
****************************************************************************
*
* **** warning **** this routine is no longer used. see fmsetstr.c
*
*   Mnemonic:  FMrfoot
*   Abstract:  This routine builds a string that is to be used for the
*              running footer.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Modified:  22 Apr 1991 - To remove fixed lenth adjustable footers.
*              29 Oct 1992 - To null pointer when removed and to write EOS
*                            in the array that is allocated.
*              27 Nov 1992 - Removed reference to rfootlen
*				17 Jul 2016 - Bring decls into the modern world.
***************************************************************************
*/
extern void FMrfoot( void )
{
 char *buf;          /* pointer at the token */
 int i;

 if( rfoot == NULL )        /* no buffer yet */
  rfoot = (char *) malloc( (unsigned) HEADFOOT_SIZE );

 i = FMgetparm( &buf );       /* get parameter */
 if( i == 0 )              /* assume off */
  {
   free( rfoot );
   rfoot = NULL;           /* no parameter */
   return;
  }

 if( strncmp( "off", buf, 3 ) == 0 )
  {
   free( rfoot );
   rfoot = NULL;
   return;
  }

 buf = &inbuf[4];           /* start just past the .rh command */

 for( i = 0; i < HEADFOOT_SIZE-1 && buf[i] != EOS; i++ )
  rfoot[i] = buf[i];               /* copy the header */
 rfoot[i] = EOS;             /* terminate the buffer */

 while( FMgetparm( &buf ) != 0 );       /* clear the input buffer */
}               /* FMrfoot */
