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
*****************************************************************************
*
*  Mnemonic: FMcenter
*  Abstract: This routine will put the rest of the line of text entered with
*            this command (.ce) in the center of the current column using
*            the cen macro defined in the postscript output file.
*            If the first two tokens are numbers then the text is centered
*            between these points.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     2 November 1992
*  Author:   E. Scott Daniels
*
*            .ce [x1 x2] <text to center> <eos>
*
*  Modified: 	25 Mar 1993 - To accept points between which text is centered
*            	28 Apr 1993 - To set same y flag if move to not necessary
*             	06 Dec 1996 - To convert to hfm (ignore x1, x2 if there)
*				05 Jul 2013 - Support in tfm now. 
*				17 Jul 2016 - Bring prototypes into modern era.
*****************************************************************************
*/
extern void FMcenter( void )
{
	char *buf;          /* pointer at parameters */
	int i;              /* length of the token */
	
	FMflush( );                      /* write what ever might be there */

	flags2 |= F2_CENTER;   /* make flush center rather than show */

	while( (i = FMgetparm( &buf )) )    /* until all parameters gone */
		FMaddtok( buf, i );          /* add it to the output buffer */

 	FMflush( );                             /* send user line on the way */

	flags2 &= ~(F2_CENTER | F2_SAMEY); 		/* ditch flags -- one time centering */
}  
