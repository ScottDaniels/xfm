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
**************************************************************************
*
*   Mnemonic: FMfigure
*   Abstract: This routine places a blank line and then the word "figure"
*             and figure number into the file. The line following this
*             line will contain the user input statement if entered on the
*             command line. This routine should later be modified to contain
*             the logic if a figure table will ever be generated. Figure
*             numbers are the section number followed by a sequential number.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     9 December 1988
*   Author:   E. Scott Daniels
*
*
*				.fg [n=number] [t=type] <text>
*					type is either table or figure, default is figure.
*
*   Modified:	30 Oct 1992 - To reduce figure text size by two from current size
*				26 May 1993 - To reduce space between figure text lines.
*				22 Feb 1994 - To ensure text font is set properly
*				04 Mar 2014 - To allow for preallocation of figure numbers and 
*					to support their use on the .fg command
*****************************************************************************
*/
void FMfigure( )
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
	int	*num_src;		/* pointer to source for table or figure number */

	
	num_src = &fig;			/* default to using figure number */

	FMflush( );            /* end current working line */
	osize = 0;             /* reset line size */

	oldspace = textspace;  /* save stuff we need to restore when leaving */
	oldfont = curfont;     /* save old font for later */
	oldsize = textsize;    /* save old size for end */
	
	flags2 |= F2_SETFONT;  /* force flush to select our figure font */

	textsize -= 2;         /* set to size for the font string */
	curfont = ffont == NULL ? curfont : ffont;   /* set figure font if there */

	tok_len = FMgetparm( &buf );
	while( tok_len >0 && (cp = strchr( buf, '=' )) != NULL )
	{
		switch( *buf )
		{
			case 'n':			// use this figure number
				fnum = atoi( cp+1 );
				break;

			case 't':			// set type (figure or table)
				if( *(cp+1) == 't' )
				{
					type = "Table";
					num_src = &table_number;		// refrence the correct table number if needed
				}
				break;
		}

		tok_len = FMgetparm( &buf );	
	}
	
	if( fnum < 0 )				// not set on the command line
	{
		fnum = *num_src;		// pull from proper spot and bump up
		(*num_src)++;
	}

	if( flags & PARA_NUM )                      /* if numbering the paragraphs, add it to the number */
 		sprintf( obuf, "%s %d-%d: ", type, pnum[0], fig );   
	else
 		sprintf( obuf, "%s %d: ", type, fig );   /* gen fig number */


	optr = strlen( obuf );    /* set pointer to be past the label */

	while( tok_len > 0 )		// add tokens, first already in the buffer
	{
		FMaddtok( buf, tok_len );                    /* add it to the output buffer */
		tok_len = FMgetparm( &buf );	
	}

	optr = 1;              /* this will cause flush to flush figure information */
	FMflush( );                             /* send user line on the way */

	flags2 |= F2_SETFONT;  /* next flush will restore the font */

	textspace = oldspace;  /* restore original text space value */
	textsize  = oldsize;   /* restore the size that was set on entry */
	curfont = oldfont;     /* restore the font set on entry */
}  
