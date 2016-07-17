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
*   Mnemonic:  FMspace
*   Abstract:  This routine processes the .SP command when entered.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      15 November 1988
*   Author:    E. Scott Daniels
*   Modified:   5 May 1992 - To support postscript conversion
*		12 Nov 2007 - Added half space.
*		24 Aug 2011 - Now accepts fraction e.g. .5 rather than neg for halfspace
*		10 Mar 2013 - Corrected problem with optional parms; must be checked before 
*					the call to flush().
*		17 Jul 2016 - Bring decls into the modern world.
*
***************************************************************************
*/
extern void FMspace( void  )
{
	char *buf;          /* pointer at the token */
	float lines = 1;

	if( FMgetparm( &buf ) > 0 ) 		/* optional parm -- number of lines to skip */
		lines = atof( buf );

	FMflush( );          /* flush out the current line */

	if( lines > 55 )			/* enforce sanity */
		lines = 55;           

#ifdef KEEP
	if( lines <= 0 )					/* old school... .5 or 0.5 preferred */
		//cury += (textsize+textspace)/2;   	/* half space */
		cury += (textsize+textspace) * lines;   	/* partial space */
	else
#endif
	TRACE( 2, "space:  (%d + %d) * %.2f\n", textsize, textspace, lines );
	cury += (textsize+textspace) * lines;   
}      
