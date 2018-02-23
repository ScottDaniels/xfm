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
***************************************************************************
*
*  Mnemonic: 	FMcapture
*  Abstract: 	capture all lines until .ca end command is noticed.
*				Lines are written to the file named on the command. If
*				'extend' is used instead of 'start' the file is opened and
*				appended to. If 'shift' is given, then the first whitespace
*				character is shifted off allowing for nested capture commands.
*				If 'expand' is given, then all captured lines are searched for 
*				simple variables (&name, not macros) and those are expanded into
*				the capture file. 
*				The '.ca end' MUST be in column 0.
*  Parms:   
*  Returns:  	Nothing.
*  Date:     	06 November 2007
*  Author:   	E. Scott Daniels
*  Modified: 	03 Jul 2016 - Added extend and shift options.
*				17 Jul 2016 - Changes for better prototype generation.
*				23 Feb 2018 - Support expanding &{var} in addition to &var
*
*  Command syntax:  .ca [expand] [shift] {start|extend} filename
*					.ca end
*
*****************************************************************************
*/

/*
	Get next parameter, assumed to be a filename, and return a dup'd string.
	The current string is freed if passed in.
*/
static char* get_fname( char* cfname ) {
	char* buf;

	FMgetparm( &buf );
	if( cfname != NULL ) {
		free( cfname );
	}
	cfname = NULL;

	if( buf != NULL ) {
		cfname = strdup( buf );
	}

	return cfname;
}

#define F_SHIFT		0x01
#define F_EXPAND	0x02
extern void FMcapture( void )
{
	FILE 	*f = NULL;
	char 	*fname = NULL;
	char 	*buf;
	char	*cp;
	char*	expanded;					// expanded buffer
	int		exp_sz = 0;					// expanded buffer size
	int		exp_used = 0;				// used in expanded buffer
	const char	*mode = "w";			// capture file open mode; default to truncate/write
	int		i = 0;
	int		skip;						// skip opening { if expanding &{foo}
	int		flags = 0;					// various flags for processing (F_ const)

	while( FMgetparm( &buf ) > 0 ) {
		switch( *buf ) {
			case 'e':
				if( strcmp( buf, "expand" ) == 0 ) {
					flags |= F_EXPAND;
				} else {
					if( strcmp( buf, "extend" ) == 0 ) {
						fname = get_fname( fname );
						mode = "a";
					} else {
						if( strcmp( buf, "end" ) == 0 )
							return; 						// shouldn't happen, but an extra ca end would trigger this
					}
				}
				break;

			case 's':
				if( strcmp( buf, "shift" ) == 0 ) {
					flags |= F_SHIFT;
				} else {
					if( strcmp( buf, "start" ) == 0 ) {
						fname = get_fname( fname );
				}
			}
		}
	}

	if( fname == NULL || !*fname ) {
		FMmsg( E_CANTOPEN, "filename or start/extend missing on capture command" );
	} else {
		if( (f = fopen( fname, mode )) == NULL )
			FMmsg( E_CANTOPEN, fname );					// we skip what was to be captured, so we must contintinue
	}

	while( FMread( inbuf ) >= 0 )
	{
		if( inbuf[0] == CMDSYM )			/* must find .ca end in first spot */
		{
			if( inbuf[1] == 'c' && inbuf[2] == 'a' )  
			{
				for( cp = &inbuf[3]; *cp && (*cp == ' ' || *cp == '\t'); cp++ );
				if( *cp && strncmp( cp, "end", 3 ) == 0 )
				{
					iptr = (cp+3) - inbuf;		/* should not be anything, but leave pointed past end */
					if( f )
						fclose( f );
					TRACE( 1, "capture: %d lines captured in %s\n", i, fname );
					if( fname ) {
						free( fname );
					}
					return;                                     /* get out now */
				}
			}
		} 

		if( f ) {
			TRACE( 2, "capture: adding: %s expand=%d\n", inbuf, !!(flags & F_EXPAND) );
			if( (flags & F_SHIFT) && isspace( *inbuf ) ) {
				cp = inbuf+1;
			} else {
				cp = inbuf;
			}	

			if( flags & F_EXPAND ) {		// look for simple variables and expand them into the capture file
				char*	vp;					// at next variable

				while( (vp = strchr( cp, '&' ))  != NULL ) {
					*vp = 0;
					fprintf( f, "%s", cp );								// send out everything before &
					cp = vp+1;
					if( *cp == '{' ) {										// suss name upto the closing }
						for( vp = cp+1; *vp && *vp != '}'; vp++ );		// suss name up to first whitespace
					} else {
						for( vp = cp+1; *vp && !isspace( *vp ); vp++ );		// suss name up to first whitespace
					}
					if( *vp != 0 ) {									// terminate var name and advance vp if not end of buffer.
						*vp = 0;
						vp++;
					}
					skip = *cp == '{' ? 1 : 0;
					if( (buf = sym_get( symtab, cp+skip, 0 )) ) {		// find expansion of variable name
						TRACE( 2, "capture: expanded: %s --> %s\n", cp, buf );
						fprintf( f, "%s%s", buf, skip ? "" : " " );		// and put it out with trailing space if not &{foo}
					} else {
						if( *cp == '{' ) {
							fprintf( f, "&%s}", cp );			// didn't expand, just leave it and ensur still wrapped in {}
						} else {
							fprintf( f, "&%s ", cp );			// didn't expand, just leave it
						}
					}
					cp = vp;
				}
			}

			TRACE( 2, "capture: final buffer out: (%s)\n", cp );
			fprintf( f, "%s\n", cp );						// all of buffer if not expanding, bit after last var if expanding
		}

		i++;
	}

}
