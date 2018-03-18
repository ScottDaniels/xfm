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
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
****************************************************************************
*
*  Mnemonic: FMinit
*  Abstract: This routine opens the initial input file and the output file
*            and does other necessary house keeping chores.
*  Parms:    argc - Number of arguments passed to fm from command line
*            argv - Argument vector list passed from command line
*  Returns:  Valid if all is well, ERROR if system could not be initialized.
*  Date:     17 November 1988
*  Author:   E. Scott Daniels
*
*  Modified:  29 Jun 1994 - To convert to rfm.
*              2 Dec 1994 - To correct a problem in RFM init strings.
*              5 Dec 1994 - To properly terminate font strings.
*                           To look and use environment variable to define
*                            the font table. (RFM_FTABLE)
*              6 Dec 1996 - To convert to hfm
*              4 Apr 1997 - To put in support for AFI's new tokenizer!
*             15 Apr 1997 - To get tokens as commands from command line.
*             31 Mar 2000 - To init text colour
*             21 Mar 2001 - What goes round, comes round; back to TFM!
*			 	08 Nov 2006 - added version number, converted tocname build to sanity.
*				17 Jul 2016 - Bring prototypes into modern era.
******************************************************************************
*/

extern int FMinit( int argc, char **argv )
{
	int i;               /* loop index */
	char *ptr;           /* pointer to argument */
	char *ffilen;        /* pointer to font file name in environment */
	int file;            /* file number of font file */
	char buf[1024];      /* input buffer for reads */
	char mbuf[1024];
	char *ofname = "stdout";  /* default to standard output */
	char *ifname = "stdin";

	version = strdup( "tfm V2.3 " __DATE__ );

	if( argc >= 2 )
	{
		if( strcmp( argv[1], "-?" ) == 0 )
		{
			FMmsg( ERROR, version );
			FMmsg( ERROR, "Usage: tfm [input-file [output-file [inital command tokens]]]" );
			return ERROR;
		}

		if( *argv[1] != '-' )		/* allow - to be default to stdin */
			ifname = argv[1];
	}

	if( argc > 2 )
	{
		if( *argv[2] != '-' )		/* allow - to be default to stdout */
			ofname = argv[2];
	}

	if( strcmp( ifname, ofname ) == 0 )
	{
		FMmsg( ERROR, "input name cannot be same as output; this is just wrong");
		return( ERROR );
	}

	if( FMopen( ifname ) < VALID )       /* open the initial input file */
		return ERROR;                     /* getout if could not do it */

	symtab = sym_alloc( 4999 );			/* defined variables */
	AFIsettoken( fptr->file, symtab, " \t", '&', '^', ":"  );
	AFIsetflag( fptr->file, AFI_F_EOBSIG, AFI_SET );
	
	ofile = AFIopen( ofname, "w" );      /* open output file */
	if( ofile < VALID )
	{
		FMmsg( E_CANTOPEN, ofname );
		FMclose( );
		return( ERROR );
	}

	for( i = argc - 1; i > 2; i-- )         		/* run remaining args in reverse */
		AFIpushtoken( fptr->file, argv[i] );   		/* to push on input stack */

	snprintf( mbuf, sizeof( mbuf ), "+TFM text formatter (%s) started", version );
	FMmsg( -1, mbuf );

	difont = NULL;                             /* initially no list item font */
	ffont = NULL;                              /* no figure font defined */
	curfont = strdup( "f2" );
	runfont = strdup( "f4");                /* copy in the running font string */

	path = getenv( "TFM_PATH" );

	iptr = 0;
	optr = 0;                 /* start at beginning of the output buffer */
	linelen = 64 * 7;

	obuf = (char *) malloc( sizeof( char ) * IO_BUF_LEN );
	inbuf = (char *) malloc( sizeof( char ) * IO_BUF_LEN );
	if( !obuf )
	{
		printf( "malloc of obuf failed\n" );
		return ERROR;
	}
	*obuf = (char) 0;

	sprintf( inbuf, ".dv tfm 1 : " );      /* simulate command to make */
	AFIpushtoken( fptr->file, inbuf );     /* what processer we are var */

	cur_col = firstcol = (struct col_blk *) malloc( sizeof( struct col_blk ) );
	if( cur_col == NULL )
		return( ERROR );

	cur_col->lmar = 0;             /* .in command will set this */
	cur_col->width = 8.5 * 72;     /* set single column width */
	cur_col->next = NULL;          /* by default we are in single column mode */

	flags = PARA_NUM;              /* turn on paragraph numbering */

	memset( tocname, 0, sizeof( tocname ) );
	snprintf( tocname, sizeof( tocname ) - 5, "%s",  ifname );	/* copy and leave enough room for us to add .toc */
	if( (ptr = strrchr( tocname, '.' )) != NULL )
		*ptr = 0;				/* truncate @ their extension */
	strcat( tocname, ".toc" ); 

	memset( pnum, 0, sizeof( pnum ) );

	for( i = 0; i < MAX_HLEVELS; i++ )   /* allocate and init header blks */
	{
		headers[i] = (struct header_blk *) malloc( sizeof( struct header_blk ) );
		if( headers[i] == NULL )
			return( ERROR );

		/*
		headers[i]->font = (char *) malloc( (strlen( DEF_HEADFONT )) + 1 );
		strcpy( headers[i]->font, DEF_HEADFONT );  
		*/
		memset( headers[i], 0, sizeof( struct header_blk ) );
		headers[i]->font = strdup( DEF_HEADFONT );
		
		headers[i]->flags = HTOC;              /* initially only TOC flag set */
		headers[i]->indent = 0;                /* set default indention */
		headers[i]->level = i+1;               /* set the level */
		headers[i]->skip = 2;                  /* skip down 2 lines */
		headers[i]->hmoffset = 0;              /* initially no header margin off */
	}

	headers[0]->size = DEF_H1SIZE;   /* set default header text sizes */
	headers[1]->size = DEF_H2SIZE;
	headers[2]->size = DEF_H3SIZE;
	headers[3]->size = DEF_H4SIZE;
	headers[0]->flags |= HEJECTC+HTOUPPER;   /* header level 1 defaults */
	
	flags &= ~PAGE_NUM;        /* turn off page numbering as default */

	textcolour = 0;
	textspace = 1;             /* tfm space calculations are different */
	boty -= 65;                /* so adjust standard psfm defaults */
	
	
	return VALID;
}            /* FMinit */
