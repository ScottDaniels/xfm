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

#include "../base/symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
****************************************************************************
*
*   Mnemonic:  FMspace
*   Abstract:  This routine processes the .SP command when entered.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      15 November 1988
*   Author:    E. Scott Daniels
*   Modified:  29 Jun 1994 - To support rfm conversion
*              18 Jul 1994 - To allow listitems in boxes
*               6 Dec 1996 - To convert for hfm
*			   15 Jan 2012 - pulled into tfm
*				05 Jul 2013 - to handle .5 pfm style skipping -- defaults to 1
*				17 Jul 2016 - Bring prototypes into modern era.
***************************************************************************
*/
extern void FMspace( void )
{
	char *buf;          /* pointer at the token */
	int len = 1;		/* number of lines to skip */

	FMflush( );          /* flush out the current line */

	if( FMgetparm( &buf ) > 0 )		/* number of lines supplied */
		len = atoi( buf );   

	if( len > 55 )			/* enforce some sanity */
		len = 55;           
	else
		if( len <= 0 )		/* default to 1 for partial line spacing suppored in pfm */
			len = 1;

	FMflush( );
	*obuf = 0;
	optr = 0;

	for( ; len > 0; len-- )          /* skip lines */
	{
		strcat( obuf, " " );
		optr += 1;
		FMflush( );
		*obuf = 0;
		optr = 0;
	}
}               
