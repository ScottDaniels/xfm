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
**************************************************************************
*
*   Mnemonic: FMfigure
*   Abstract: This routine places a blank line and then the word "figure"
*             and figure number into the file. The line following this
*             line will contain the user input statement if entered on the
*             command line. This routine should later be modified to contain
*             the logic if a figure table will ever be generated. Figure
*             numbers are the section number followed by a sequential number.
*
*				If 'nosect' is given on the command, then the command is
*				used just to set the flags which control whether or not
*				section style numbering is used when paragraph numbering is
*				turned on.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     9 December 1988
*   Author:   E. Scott Daniels
*
*
*				Two styles:
*				.fg nosect
*				.fg [s=size] [f=font] [v=var_name] [num=number] [t=type] [x=<string>] <text>
*					type is either table or figure, default is figure.
*					x=<string> allows "flags" such as x=center to be set. Supported
*					flags are:
*						x=center  == center the figure string
*						x=reset	  == reset counters to 1
*
*   Modified:	30 Oct 1992 - To reduce figure text size by two from current size
*				26 May 1993 - To reduce space between figure text lines.
*				22 Feb 1994 - To ensure text font is set properly
*			    13 Oct 2001 - To setup for fonts using the font blocks
*				04 Mar 2014 - To allow for preallocation of figure numbers and
*					to support their use on the .fg command
*				17 Jul 2016 - Bring decls into the modern world.
*				11 Aug 2017 - Add size (s=), font (f=), and nosect parm support.
*				20 Jun 2019	- Add x= support
*****************************************************************************
*/
extern void FMfigure( void )
{
	int i;              /* loop index and parameter length */
	char *buf;          /* pointer at parameter list entered */
	int oldspace;       /* old text space value */
	char *oldfont;      /* hold old font to restore at end */
	int  oldsize;       /* hold old text size to restore at end */
	int fnum = -1;	 /* possible number from the command */
	int	tok_len;
	char	*cp;
	char	*type = "Figure";
	int	*num_src;				/* pointer to source for table or figure number */
	char	fbuf[100];			/* format buffer */
	char	vbuf[100];			// build .dv command in if vname given
	char*	vname = NULL;		// var name to put fig number into
	int		center = 0;			// if x=center given we will flip center on
	int 	nop = 0;			// set if x=noop is given to just set things and not add text


	tok_len = FMgetparm( &buf );				// get early so we can check for nosect command and bail before any real work
	if( strcmp( buf, "nosect" ) == 0 ) {
		flags3 &= ~F3_FIG_SECT_NUM;				// turn off section style even if para numbers are on
		return;
	}

	num_src = &fig;			/* default to using figure number */

	FMflush( );            /* end current working line */
	FMfmt_add( );			// start the next line

	oldspace = textspace;  /* save stuff we need to restore when leaving */
	oldfont = curfont;     /* save old font for later */
	oldsize = textsize;    /* save old size for end */

	flags2 |= F2_SETFONT;  /* force flush to select our figure font */
	curfont = strdup( ffont == NULL ? curfont : ffont );   	// default to figure font if there, text font if not, f= overrides later

	if( textsize > 8 )
		textsize -= 2;         /* default size for the font string is slightly smaller than current */

	while( tok_len > 0 && ((cp = strchr( buf, '=' )) != NULL) )
	{
		switch( *buf )
		{
			case 'f':
				if( *(cp+1) ) {
					free( curfont );				// free what we created before
					curfont = strdup( cp+1 );		// dup user font so we always free something at end
				}
				break;

			case 'n':			// n[um]=n  or na[me]=name
				if( *(cp + 1) ) {
					fnum = atoi( cp+1 );
				}
				break;

			case 'p':					// p[refix]=<string>
				if( fig_prefix ) {
					free( fig_prefix );
				}
				fig_prefix = strdup( cp+1 );
				break;

			case 's':			// override text size
				if( *(cp + 1) ) {
					textsize = atoi( cp+1 );
				}
				break;
		
			case 't':			// set type (figure or table)
				if( *(cp+1) == 't' )
				{
					type = "Table";
					num_src = &table_number;		// refrence the correct table number if needed
				}
				break;

			case 'v':			// var-name
				if( *(cp + 1) ) {
					vname = strdup( cp+1 );
				}
				break;

			case 'x':
				if( strcmp( cp+1, "center" ) == 0 ) {
					center = 1;
				} else {
					if( strcmp( cp+1, "reset" ) == 0 ) {
						table_number = 1;
						fig = 1;
					} else {
						if( strcmp( cp+1, "noop" ) == 0 ) {
							nop = 1;
						}
					}
				}
		}

		tok_len = FMgetparm( &buf );
	}

	FMfmt_add();				/* must push the size on the stack */

	if( fnum < 0 )				// not set on the command line
	{
		fnum = *num_src;		// pull from proper spot and bump up
		(*num_src)++;
	}

	if( (flags & PARA_NUM) && (flags3 & F3_FIG_SECT_NUM) ) {                      /* if numbering the paragraphs, add it to the number */
		snprintf( fbuf, sizeof( fbuf ), "%s %d-%d: ", type, pnum[0], fnum );
		if( vname ) {
			snprintf( vbuf, sizeof( vbuf ), ".dv %s %d-%d :", vname, pnum[0], fnum );
		}
	} else {
		if( fig_prefix ) {
			snprintf( fbuf, sizeof( fbuf ), "%s %s%d: ", type, fig_prefix, fnum );   /* gen fig number */
		} else {
			snprintf( fbuf, sizeof( fbuf ), "%s %d: ", type, fnum );   /* gen fig number */
		}
		if( vname ) {
			snprintf( vbuf, sizeof( vbuf ), ".dv %s %d :", vname, fnum );
		}
	}


	FMaddtok( fbuf, strlen( fbuf ) );                    /* add it to the output buffer */
	while( tok_len > 0 )		// add tokens, first already in the buffer
	{
		FMaddtok( buf, tok_len );                    /* add it to the output buffer */
		tok_len = FMgetparm( &buf );
	}

	if( center ) {
   		if( flags2 & F2_CENTER ) {
			center = 0;						// user has flag set, do nothing
		} else {
			flags2 |= F2_CENTER;
		}
	}

	FMflush( );                             /* send user line on the way */

	if( center ) {
   		flags2 &= ~F2_CENTER;				// if we set it, we flip it off now
	}

	flags2 |= F2_SETFONT;  /* next flush will restore the font */

	if( vname ) {
		AFIpushtoken( fptr->file, vbuf );		// cause the desired variable to be set
		free( vname );
	}

	free( curfont );
	textspace = oldspace;  /* restore original text space value */
	textsize  = oldsize;   /* restore the size that was set on entry */
	curfont = oldfont;     /* restore the font set on entry */
	FMfmt_add( );		/* initial font info for next buffer */
}
