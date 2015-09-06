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
* --------------------------------------------------------------------------------
* Mnemonic:	fmstate.c - push/pop state of things
* Abstract: 	At times during pfm processing its necessary to hold the current 
*		state of things. push saves the state and pop restores it to the 
*		last pushed value.
*
*		WARNING - The format stack is saved. If a set text/set font does
*		not happen right after the push then odd results will occur
*
* Date:		19 Nov 2001
* Author:	E. Scott Daniels
* Mods:		10 Apr 2007 - fixed leaks.
*		06 Nov 2007 - pushes a new format block when state is pushed
* --------------------------------------------------------------------------------
*/
static struct state {
	char	*obuf;
	char	*inbuf;
	char	*difont;
	int		optr;
	int 	lmar;
	int		llen;
	int		textsize;
	int		flags1;
	int		flags2;
	int		flags3;
	int		osize;
	char	*textfont;
	char	*colour;
} state_stack[25];
static int state_idx = 0;

void FMpush_state( )
{
	TRACE( 2, "fmstate: pushed: lmar=%d optr=%d obuf=(%s)\n", lmar, optr, obuf );
	if( state_idx < 25 )
	{
		FMfmt_save( );
		FMfmt_add( );
		state_stack[state_idx].obuf = obuf;
		state_stack[state_idx].inbuf = inbuf;
		state_stack[state_idx].osize = osize;
		state_stack[state_idx].optr = optr;
		state_stack[state_idx].lmar = lmar;
		state_stack[state_idx].llen = linelen;
		state_stack[state_idx].textsize = textsize;
		state_stack[state_idx].textfont = curfont ? strdup( curfont ) : NULL;
		state_stack[state_idx].difont = difont ? strdup( difont ) : NULL;
		state_stack[state_idx].flags1 = flags;
		state_stack[state_idx].flags2 = flags2;
		state_stack[state_idx].flags3 = flags3;
		state_stack[state_idx].colour = textcolour;

		obuf = (char *) malloc( 2048 );
		inbuf = (char *) malloc( 2048 );

		memset( obuf, 0, 2048 );
		memset( inbuf, 0, 2048 );

		optr = 0;
		*obuf = 0;
	}
	else
	{
		fprintf( stderr, "abort: push state too deep\n" );
		exit( 1 );
	}

	state_idx++;
}

void FMpop_state( )
{
	if( state_idx > 0 )
	{
		if( --state_idx <= 24 )
		{
			TRACE( 2, "fmstate: popped: oldlmar=%d newlmar=%d optr=%d obuf=(%s)\n", lmar, state_stack[state_idx].lmar, optr, obuf );
			free( obuf );
			free( inbuf );
			free( curfont );
			free( difont );

			obuf = state_stack[state_idx].obuf;
			osize = state_stack[state_idx].osize;
			inbuf = state_stack[state_idx].inbuf;
			optr = state_stack[state_idx].optr;
			lmar = state_stack[state_idx].lmar;
			linelen = state_stack[state_idx].llen;
			textsize = state_stack[state_idx].textsize;
			curfont = state_stack[state_idx].textfont;
			difont = state_stack[state_idx].difont;
			textcolour = state_stack[state_idx].colour;
			FMfmt_restore( );
		}
	}
}
