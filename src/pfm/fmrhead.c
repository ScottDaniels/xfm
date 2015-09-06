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

this should cause errors if compiled -- deprecated ----
**** warning **** this routine is no longer used. see fmsetstr.c

/*
****************************************************************************

*   Mnemonic:  FMrhead
*   Abstract:  This routine builds a string that is to be used for the
*              running header.
*              that the user enteres.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Modified:  22 Apr 1991 - To make header adjustable on the page
*              29 Oct 1992 - To null pointer when removed and to write EOS
*                            inside of the array.
*              27 Nov 1992 - To remove reference to rheadlen
***************************************************************************
*/
void FMrhead( )
{
	char *buf;          /* pointer at the token */
	int i;
	int ridx = 0;

	if( rhead == NULL )        /* no buffer yet */
 		rhead = (char *) malloc( (unsigned) HEADFOOT_SIZE );

 	i = FMgetparm( &buf );       /* get parameter */

 	if( i == 0 || (strncmp( "off", buf, 3 ) == 0) )
  	{
   		free( rhead );
   		rhead = NULL;
   		return;
  	}

 	*rhead = 0;
 	do
	{
		if( (len = FMgetparm( &buf )) )	
		{
fprintf( stderr, "adding header token: %s\n", buf );
			if( ridx )
				rhead[ridx++] = " ";

			for( i = 0; i < len; i++ )
				rhead[ridx++] = buf[i];
		}
	}
 	while( len && strlen( buf < HEADFOOT_SIZE - 1 )

	rhead[ridx] = 0;

	if( len )
 		while( FMgetparm( &buf ) != 0 );       /* input too long - clear rest */
}               /* FMrhead */
