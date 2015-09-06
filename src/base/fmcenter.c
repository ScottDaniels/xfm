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
*            .ce [x1=p1 x2=p2] <text to center> <eos>
*
*			p1 and p2 are values in points from left column edge. If p2 is negative
*			then the distance is from the right edge of the current column.
*
*  Modified: 25 Mar 1993 - To accept points between which text is centered
*            28 Apr 1993 - To set same y flag if move to not necessary
*			08 Feb 2002 - To ditch the x1 x2 parms as it seems to always cause 
*					issues. now accepts x1= x2= parameters if user really
*					wants to do this. Should cause less headaches
*			06 Jul 2013 - negative p2 value now supported.
*****************************************************************************
*/
void FMcenter( )
{
	char *buf;          /* pointer at parameters */
	int i;              /* length of the token */

	FMflush( );                      /* write what ever might be there */

	flags2 |= F2_CENTER;   /* make flush center rather than show */

	if( (i = FMgetparm( &buf )) > 0  && strncmp( buf, "x1=", 3 ) == 0 )
	{
		flags2 |= F2_SAMEY;      /* indicate same y value */
		cenx1 = cur_col->lmar + atoi( buf+3 );     /* convert first point to integer */

		if( FMgetparm( &buf ) > 0 && strncmp( buf, "x2=", 3 ) == 0 )
		{
    		cenx2 = cur_col->lmar + atoi( buf+3 );    /* convert second digit */
			if( cenx2 < 0 )				/* assume neg value is distance from the right edge rather than left */
				cenx2 +=  cur_col->width;
		}
		else
		{
			FMmsg( E_MISSINGPARM, inbuf );   /* missing parameter error */
			return;
		}
		if( cenx2 <= cenx1 )   /* cannot have this */
		{
			FMmsg( E_PARMOOR, inbuf );   /* send error and exit from here */
			return;
		}
		i = FMgetparm( &buf );    /* get first text parm to center */
	}
	else                        /* no columns defined - center between margins */
	{
		cenx1 = cur_col->lmar;
		cenx2 = cur_col->lmar + cur_col->width;  /* defalult center */
	}

	TRACE( 2, "center: x1=%d x2=%d\n", cenx1, cenx2 );

	while( i > 0 )    /* until all parameters gone */
	{
		FMaddtok( buf, i );          /* add it to the output buffer */
		i = FMgetparm( &buf );
	}                             /* end while */

	FMflush( );                             /* send user line on the way */
	
	flags2 &= ~(F2_CENTER | F2_SAMEY);  /* turn off flags */
}      /* fmcenter */
