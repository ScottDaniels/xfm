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
*************************************************************************
*
*   Mnemonic: FMaddtok
*   Abstract: This routine adds a token to the output buffer. If necessary
*             the buffer is flushed before the token is added.
*   Params:   buf - pointer to the token to add
*             len - length of the token
*   Returns:  Nothing.
*   Date:     15 November 1988
*   Author:   E. Scott Daniels
*
*   Modified:  3 May 1992 - To support postscript output
*             11 Nov 1992 - To escape ( and ) in tokens
*             12 Nov 1992 - To call justify inplace of flush if flag is set
*              9 Mar 1993 - To  look for a ^ which is used to escape the
*                            next character.
*			06 Apr 1994 - To use point based line length
*			09 Feb 2002 - Added ability to chop words with a hiphenator
*			26 May 2002 - To use the hyphenation library function to see if
*				the word was in the hyph dictionary.
*			11 Oct 2015 - Fixed escaping bug.
*			03 Jan 2016 - Fixed bug which caused token to be dropped if
*				just/fush called with multiple pending ateject things.
*			17 Jul 2016 - Bring decls into the modern world.
*************************************************************************
*/
static char *hyph_get( char *buf )		/* routine is lost, for now we always return null */
{
	return NULL;
}

extern void FMchop( char *buf, int len, int remain )
{
	char	t1[1024];
	char	t2[1024];
	char 	*word;
	int	i;
	int	toksize;
	char 	*h;			/* already a hyphd word */

	flags3 &= ~ F3_HYPHEN;			/* turn off while inside here */

	if( (word = hyph_get( buf )) != NULL )
	{
		TRACE(1, "chop: hyph_get returned (%s)\n", word );
		if( h = strchr( word, '-' ) )
		{
			i = (h - word)+1;
			strncpy( t1, word, i );
			t1[i] = 0;
			strcpy( t2, h+1 );
		}
		else
		{
			strcpy( t1, word );
			t2[0] = 0;
			i = strlen( t1 );
		}
	}
	else
	{
		if( (h = strchr( buf, '-' )) )
		{
			i = (h - buf)+1;
			strncpy( t1, buf, i );
			t1[i] = 0;
			strcpy( t2, h+1 );
		}
		else
		{
			for( i = 0; i < len-1 && buf[i] != buf[i+1]; i++ );	/* split at double character */
			i++;
			if( i >= len )		/* no double charcter - find first vowel and split after that */
			{
				fprintf( stderr, "guessing at hyphen: %s\n", buf );
				for( i = 1; i < len-1 && ! strchr( "aeiou", *(buf+i) ); i++ );
				i++;
				}
		
			strncpy( t1, buf, i );
			t1[i] = '-';
			t1[i+1] = 0;
			strcpy( t2, buf+i );
		}
	}

	toksize = FMtoksize( t1, i+1 );
	if( toksize < remain )
	{
		FMaddtok( t1, i + 1 );
		FMaddtok( t2, strlen( t2 ) );
	}
	else
		FMaddtok( buf, len );

		flags3 |= F3_HYPHEN;
}

extern void FMaddtok( char *buf, int len )
{
	static int ok1 = 1; 	/* ok to add a single char at end w/o remain check (for font change .) */
	int remain;          /* calculated space remaining before the right margin */
	int i;               /* loop index */
	int toksize;         /* size of token in points */
	char wbuf[2048];
	int is_escaped = 0;	// prevent escape of backslant
	char* buf_start;		// may bounce buf up past start, so need for free

	buf_start = buf = strdup( buf );		// dup so that it survivees flush/just call if at eject pending
	len = strlen( buf );
 	words++;             /* increase the number of words in the document */

 	if( buf[0] == '^' )   /* escaping character? */
	{
		buf++;              /* skip over it and decrease the size */
		len--;
	}

	if( flags3 & F3_IDX_SNARF )		/* add to index if snarfing */
	{
		TRACE( 3, "adding index reference: %s\n", buf );

		di_add( buf, page + 1 );
	}

	if( optr == 0 ) {   		    /* if starting at beginning */
		osize = 0;					/* reset current line size */
		flags2 &= ~F2_SMASH;		// and ensure this is off
	}

	remain = linelen - osize;    /* get space remaining for insertion */

	if( (toksize = FMtoksize( buf, len )) > remain+4 )   /* room for it? */
 	{                                    /* no - put out current buffer */
		if( len == 1 && ok1 )
		{
			ok1 = 0;
		}
		else
		{
			TRACE( 1, "addtok: (%s) does not fit must flush: len=%d ok1=%d add (%s) to (%s)\n", buf, len, ok1, buf, obuf );
			ok1 = 1;
			if( (flags & JUSTIFY ) && (flags3 & F3_HYPHEN) && len > 5  )	/* might try hyphen */
			{
				words -= 2;		/* we already counted; prevent counting the two split tokens */
				FMchop( buf, len, remain );
				return; 
			}

			TRACE(2, "addtok: %s: (%s)\n", flags & JUSTIFY ? "justifying" : "flushing", obuf );
	
			if( flags & JUSTIFY )
				FMjustify( );            /* sendout in justification mode */
			else
			{
				FMflush( );        /* if not justify - send out using flush  */
			}

			osize = 0;          /* new buffer has no size yet */

			TRACE( 2, "addtok: done with just/flush buf=(%s) obuf=(%s) cury=%d topy=%d\n", buf, obuf, cury, topy );
		}
  	} else {
 		if( flags2 & F2_SMASH ) 
		{
			while( optr && obuf[optr-1] == ' ' )
			{
				optr--;
				osize--;
			}
			obuf[optr] = 0;
		}
	}
	flags2 &= ~F2_SMASH;

 	if( optr == 0 && len == 1 && *buf == ' ' ) {
		free( buf_start );
		return;
	}

 	if( optr == 0 && *buf == ' ' )			/* squelch lead blanks at the beginning of a buffer */
 	{
		while( *buf++ == ' ' );
		if( *buf == 0 ) {
			free( buf_start );
			return;
		}
 		toksize = FMtoksize( buf, len );		/* need to recalc */
 	}

 	osize += toksize;         /* add current token to current length */

	for( i = 0; i < len && buf[i]; i++, optr++ )     /* copy token to output buffer */
	{
		if( buf[i] == '^' ) {
			i++; 							// take next as is
		} else {
			if( buf[i] == '(' || buf[i] == ')' || buf[i] == '\\' ) /* need to escape */
			{
				obuf[optr] = '\\';       /* put in the PostScript escape character */
				optr++;                  /* and bump up the pointer */
			}
		}

		obuf[optr] = buf[i];         /* copy the next char to the output buffer */
	}

	obuf[optr]= BLANK;            /* terminate token with a blank */
	optr++;                       /* set for location of next token */

	if( flags2 & F2_PUNSPACE )    /* if need to space after punctuation */
		if( ispunct( obuf[optr-2] ) )  /* and last character is punctuation */
		{
			obuf[optr] = BLANK;           /* add extra space */
			optr++;                       /* and up the pointer */
		}

	obuf[optr] = EOS;             /* terminate buffer incase we flush */
	free( buf_start );
}                 /* FMaddtok */

