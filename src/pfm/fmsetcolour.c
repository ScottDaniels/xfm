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

// vim: sw=4 ts=4: 

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

/* --------------------------------------------------------------------------
*
* Mnemonic:	fmsetcolour
* Abstract: suss out a colour value of the form #rrggbb or 0xrrggbb from the 
*			parameter t and then add it to the format block.
* Date:		20 Nov 2002
* Author:	E. Scott Daniels
*			07 Jul 2013 - converted to setting up for FMfmt(), and added push
*				pop functions.
*			17 Jul 2016 - Bring decls into the modern world.
* --------------------------------------------------------------------------
*/

#define MAX_COLOUR_STACK 100
static	char *colour_stack[MAX_COLOUR_STACK];
static	int cs_idx = 0;
static char *lastcolour = NULL;  


//extern void FMsetcolour( char *colour );

/*
	pop the top of the colour stack and cause it to be set in the doc. 
*/
extern void FMpopcolour( )
{
	char	*colour;

	if( cs_idx <= 0 )
		return;

	if( (colour = colour_stack[cs_idx--]) == NULL )		/* shouldn't happen */
		return;

	if( lastcolour )
		free( lastcolour );
	lastcolour = NULL;

	TRACE( 2, "colour popped: %s", colour );
	FMsetcolour( colour );						/* push into output stream and set as last  colour */
	free( colour );
}

extern void FMsetcolour( char *t )
{

	double r;
	double g;
	double b;
	unsigned int c;

	char buf[1024];

	if( ! t )	/* internal functions that set constant colours for things like rh/rf may call w/o parm to reset though they should call pop() */
	{
		TRACE( 1,  "setcolour: colour was reset -- shouldn't get here.\n" );
		FMpopcolour( );
		return;
	}

	if( strcmp( t, "bg" ) == 0     || strcmp( t, "link" ) == 0   || strcmp( t, "vlink" ) == 0  || strcmp( t, "hlink" ) == 0  )
		return;

	if( *t == '#' )
		t++;
	else
		if( *t == '0' && *(t+1) == 'x' )
			t += 2;

	if( lastcolour )
	{
		if( strcmp( t, lastcolour ) == 0 )
		{
			TRACE( 2, "setcolour: same value, unchanged\n" );
			return;
		}
		else
			free( lastcolour );
	}

	lastcolour = strdup( t );

	sscanf( t, "%x", &c );

	b = c & 0xff;
	g = (c >> 8) & 0xff;
	r = (c >> 16) & 0xff;

	sprintf( buf,  "%0.2f %0.2f %0.2f ", (double) r/255, (double) g/255, (double) b/255 );
	TRACE( 2,  "setcolour: colour=[#]%s == %s (%02x %02x %02x) 0x%06x\n",  t ? t : "NULL", buf, (unsigned int)r,(unsigned int)g,(unsigned int)b,c );

	textcolour = buf;			/* temp set for fmt_add() call */
	FMfmt_add();
	textcolour = NULL;
}

/*
	save the last colour used; if new colour is given then we set the new colour
*/
extern void FMpushcolour(  char *new_colour )
{
	
	if( cs_idx >= MAX_COLOUR_STACK )
	{
		fprintf( stderr, "colour stack overflow\n" );
		return;
	}

	if( !lastcolour )
		lastcolour = strdup( "#000000" );
	colour_stack[cs_idx++] = lastcolour;
	TRACE( 2, "colour pushed: %s", lastcolour );
	lastcolour = NULL;

	if( new_colour )
		FMsetcolour( new_colour );
}
