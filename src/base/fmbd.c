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
****************************************************************************
*
*   Mnemonic:  FMbd
*   Abstract:  This routine parses the .bd command to start a definition list
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      1 January 1989 (Happy New Year!)
*   Author:    E. Scott Daniels
*
*   Modified:  5 May 1992 - To support postscript conversion
*             12 Jul 1993 - To support right justification of terms and
*                           setting of a different font for terms.
*             21 Feb 1994 - To correct two col problem by keeping the amount
*                           indented in the stack and not the left marg value.
*              6 Apr 1994 - To call getpts to get point value of term size
*             10 Feb 2002 - To add auto skip option
*
*    .bd <termsize[p|i]> [right] [font name] [s=n]
*         right - indicates that terms are to be right justified in the field
*         font  - name of the font to show terms in
*	  s=n - skip a line before each di.
*   Copyright (c) 1989  E. Scott Daniels. All rights reserved.
***************************************************************************
*/
void FMbd( )
{
	char *buf;           /* pointer at the token */
	int len;             /* len of parameter entered */
	int skip = 0;    	/* set to true if user wants automatic .sp with each .di */
	int tsize = 0;          /* requested term size - default is 1 inch = 72pts */
	int j = 0;
	int	anumflag = 0;
	int	astart = 1;
	char title[255];     /* buffer to use to space over on write */

	FMflush( );     		/* sweep the floor before we start */

	if( dlstackp >= MAX_DEFLIST )   			/* too deep */
	{
		while( FMgetparm( &buf ) );
		fprintf( stderr, "fmbd: too deep - bailing out \n" );
		return;
	}

	
	dlstackp++;				/* increase the definition list stack pointer */
	memset( &dlstack[dlstackp], 0, sizeof( dlstack[0] ) );

	while( (len = FMgetparm( &buf )) > 0 )    /* is there a length? */
	{                                    /* process the parameter(s) entered */
		if( ! j++ )
		{
			tsize = FMgetpts( buf, len );              /* get the term size in points */
			dlstack[dlstackp].indent = tsize;
			lmar += tsize;                 /* set the new indented left margin */
			linelen -= tsize;              /* shrink ll as to not shift the line any */
		}
		else
		{
			if( strncmp( buf, "right", len ) == 0 )  /* right just? */
			{
				flags2 |= F2_DIRIGHT;        /* turn on the flag */
				len = FMgetparm( &buf );     /* get next parm if there */
			}
			else
			if( strchr( buf, '=' ) )		/* something = something */
			{
				switch( *buf )
				{
					case 'a':	
						if( isdigit( *(buf+2) ) )
						{
							dlstack[dlstackp].anum = DI_ANUMI;	/* integer numbering */
							dlstack[dlstackp].aidx = atoi( buf+2 );	/* where to start */
						}
						else
						{
							dlstack[dlstackp].anum = DI_ANUMA;	/* integer numbering */
							dlstack[dlstackp].aidx = *(buf+2);	/* where to start */
						}
						break;

					case 'f':					/* format string */	
							dlstack[dlstackp].fmt = strdup( buf+2 );
							break;
				
					case 's':	
						dlstack[dlstackp].skip = atoi( buf + 2 );
						break;

					default:	break;
				}
			}
			else
			{
				if( difont != NULL )    /* free the font buffer if there */
					free( difont );
				difont = strdup( buf );
			}
		}
	}               /* end else */

	if( ! tsize )			/* no parms supplied, or bad parm */
	{
		tsize = 72;
		dlstack[dlstackp].indent = tsize;
		lmar += tsize;                 /* set the new indented left margin */
		linelen -= tsize;              /* shrink ll as to not shift the line any */
	}

}                                /* FMbd */
