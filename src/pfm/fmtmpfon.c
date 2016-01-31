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
******************************************************************************
*
*  Mnemonic: FMtmpfont
*  Abstract: This routine is responsible for establishing a temporary font 
*			 or super/subscript change.
*  Parms:    The following parms are expected to follow the command (.tf)
*
*				[{super|sub}script] [<fontname>] {fontsize>|--} <text-tokens> [:]
*
*			For super/subscript fontsize can be a hard value (e.g. 8) or can 
*			be a relative division of the current fontsize (/2 for half, /3 for 
*			one third etc.). The trailing colon is needed as a command separator
*			if this command is not immediately followed by a newline. 
*
*  Returns:  Nothing.
*  Date:     20 Nov 2001
*  Author:   E. Scott Daniels
*  Mods:	09 Mar 2013 - added support for super/subscripts
*			11 Mar 2013 - added ability to change font for super/subscript.
*			31 Jan 2016 - added user controlled displacement (d=) for super/subscript.
*					Changed the default subscript displacement to tsize/4.
*
* 	Syntax:
* 			.tf [d=displacement] {font-name|superscript|subscript} {font-size|m/n} <text>
*
*	allows for macros like: 
*			.dv ital .sm .tf &italfont &textsize $1 ^:
*		or
*			.dv super .sm .tf superscript /2 $1 ^:
*			.dv zsuper .sm .tf ZapfDingbats 2/3 $1 ^:
*
*	the .sm command causes the whitespace to be removed between the last 
*	token placed into the document and the fonted character.
****************************************************************************
*/
#include <stdio.h>
#include <unistd.h>

/*
	allow /n where return is ref/n
	allow m/n where return is (ref * m)/n
	allow n where return is n
*/
static int parse_value(  char *b, int ref )
{
	char	*c;
	int		v;

	if( *b == '-' )
		return 0;

	if( (c = strchr( b, '/' )) )
	{
		if( c == b )
		{
			v = atoi( c+1 );
			return ref / v;
		}

		return (ref *atoi( b )) / atoi( c+1 );
	}	

	return atoi( b );
}

static int get_our_parms( char *buf, char **fname )
{
	if( buf || FMgetparm( &buf ) > 0 )
	{
		if( isalpha( *buf ) )				/* assume font name */
		{
			*fname = strdup( buf );
			if( FMgetparm( &buf ) <= 0 )	/* shouldn't be missing, but users are users */
				return 0;
		}
	}
	else
		return 0;

	return parse_value( buf, textsize );
}

void FMtmpfont( )
{
	char 	*buf = NULL;
	char	*ofont = NULL;		/* old font */
	char	*tok = NULL;
	int		osize = textsize;
	int		len = 0;
	int		ydisp = 0;

	TRACE( 2, "tmpfont: starts\n" );
	osize = textsize;
	ofont = curfont;
	curfont = NULL;

	if( FMgetparm( &buf ) > 0 )
	{
		int len = 1;
		while( len > 0 && buf[1] == '=' ) {					// pick up any x= parms
			switch( buf[0] ) {
				case 'd':
					if( strncmp( buf, "d=", 2 ) == 0 ) {
						ydisp = atoi( &buf[2] );
					}
					break;

				default:			// sliently ignore any other x= things
					break;
			}

			len = FMgetparm( &buf );
		}

		if( len > 0 ) {
			if( strcmp( buf, "superscript" ) == 0 )
			{
				if( (textsize = get_our_parms( NULL, &curfont )) <= 0 )
					textsize = osize;

				if( curfont == NULL )
				{
					curfont = ofont;
					ofont = NULL;
				}

				ydisp = textsize/2;
			}
			else
			{
				if( strcmp( buf, "subscript" ) == 0 )
				{
					if( (textsize = get_our_parms( NULL, &curfont )) <= 0 )
						textsize = osize;
		
					if( curfont == NULL )
					{
						curfont = ofont;
						ofont = NULL;
					}

					ydisp = -textsize /4;
				}
				else
				{
					if( (textsize = get_our_parms( buf, &curfont )) <= 0 )
						textsize = osize;
		
					if( curfont == NULL )
					{
						curfont = ofont;
						ofont = NULL;
					}

					FMfmt_add( );			/* add a format block to the list without y displacement */
				}
			}
		}
	}

	if( ydisp != 0 ) {
		FMfmt_yadd( ydisp );		// y displacement from d= or sub/superscript default
	}

	while( (len = FMgetparm( &buf )) )					/* rest of the parms go into the text */
	{
		TRACE( 2, "tmpfont: (%s)\n", buf );
		if( (tok = strchr( buf, '~' )) != NULL )		/* allow font change just to lead string e.g.  foo of foo~bar */
		{
			*tok = 0;

			FMaddtok( buf, tok-buf );
			tok++;
			len = (tok-buf) - 1;
			break;
		}
		else
			FMaddtok( buf, len );
	}

	if( ofont )
	{
		free( curfont );
		curfont = ofont;
	}	

	textsize = osize;

	FMfmt_add( );					/* restore original font */
	

	if( tok && *tok )	
	{
		flags2 |= F2_SMASH;
		if( *tok == '.' )
			AFIpushtoken(fptr->file, tok );
		else
			FMaddtok( tok, len );
	}

	TRACE( 2, "tmpfont: ends\n" );
}
