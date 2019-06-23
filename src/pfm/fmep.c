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
#include <errno.h>

#include "symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"
#include "pfmproto.h"

/*
*****************************************************************************
*
*  Mnemonic: FMep
*  Abstract: This routine is called when the .ep command is encountered to
*            setup for and include an encapsulated PostScript file. The EP
*            file must contain a %%BoundingBox: directive that indicates the
*            box that binds the graphics.
*            Space is reserved for the ep drawing at the current y location
*            for a length that is defined by the l= parameter on the command
*            (to the end of the page if omitted). The drawing is scaled to
*            fit the area based on the bounding box directive in the ep file.
*            if the stretch option is present on the .ep command then the
*            drawing is stretched in either the x or y direction to attempt
*            to use all of the reserved space. If the proportional option is
*            indicated then the x and y scales will be equal and set such that
*            the smaller one based on bounding box info is used. Options on
*            the .ep command are NOT positional.
*  Parms:    None
*  Returns:  Nothing.
*  Date:     24 March 1994
*  Author:   E. Scott Daniels
*  Mods:
*			10 Apr 2007 - Memory leak cleanup.
*			29 Oct 2007 - Handles BeginData/EndData ignoring comment like 
*				statements that appear inside of them.
*			23 Mar 2011 - correccted problem not closing file.
*			17 Jul 2016 - Bring decls into the modern world.
*			12 Mar 2019 - Add y offset
*
* .ep filename [l=n] [x=n] [center|close|stretch|proportional] [noadvy]
*     l=10i  -length to reserve is 10 inches
*     l=100p -length to reserve is 10 points
*     l=10   -length to reserve is 10 lines at current text size
*     x=     -inches/points to shift x value of org to right
*     center -center the drawing between current column margins
*     close  -adjust the y value to a max of l, but close to use what is needed
*     noadvy -Dont advance y value after including ep file
*     stretch-Attempt to fill all of the reserved area
*     proport-keep drawing proportional in x and y direction
*****************************************************************************
*/
extern void FMep(  void )
{
	float xscale = 1.0;            /* scale calculation variables */
	float yscale = 1.0;
	int efile = -1;                /* input ep file */
	char *ebuf;                    /* input buffer */
	char *buf;                     /* pointer to info in input buffer */
	int length;                    /* length of insertsion area in document */
	int xoffset = 0;               /* amount to scoot over in x direction */
	int yoffset = 0;               /* amount to scoot up */
	int flags = 0;                 /* local flags */
	int plen;                      /* parameter length */
	int width;                     /* width of the area */
	char *fname;                   /* file name of the input file */
	char *tok;                     /* pointer to token in string */
	int llx = 0;                   /* bounding box coords lower left */
	int lly = 0;
	int urx = 0;                   /* bounding box coords upper right */
	int ury = 0;
	int inside_data = 0;	/* prevent screwing with stuff between %%BeginData and %%EndData */

	FMflush( );                    /* terminate line if stuff there */


	if( (length = FMgetparm( &buf )) > 0 )    /* get file name (required) */
	{
		iptr++;                      /* bump up since we will insert eos */
		fname = strdup( buf );                  /* point to it */
		TRACE( 1, "ep: fname=%s\n", fname );
	}
	else
	{
		FMmsg( E_MISSINGNAME, ".EP" );   /* generate error message */
		return;                          /* and get lost */
	}

	length = (boty - 5) - (cury + 2 * (textsize + textspace));  /* default */

	while( (plen = FMgetparm( &buf )) > 0 )    /* process other parameters */
	{
		switch( buf[0] )      /* parms are: l= CLose x= CEnter STretch    */
		{                    /*           NOadvy PRoportional            */
			case 'l':           /* length parameter entered */
			case 'L':
				length = FMgetpts( &buf[2], plen-2 );  /* get distance */
				break;
	
			case 'x':                                /* x offset entered */
			case 'X':
				xoffset = FMgetpts( &buf[2], plen-2 );   /* get value */
				break;

			case 'Y':                                /* y offset entered */
			case 'y':
				yoffset = FMgetpts( &buf[2], plen-2 );   /* get value */
				break;


			case 'n':            /* no advance of cury at end */
			case 'N':
				flags |= LF_NOADV;
				break;

			case 'c':                  /* close or center entered */
			case 'C':
				if( buf[1] == 'e' )
					flags |= LF_CENTER;     /* center if room in x direction */
				else
					if( buf[1] == 'l' )
				flags |= LF_CLOSE;     /* close space in y direction if possible */
				break;

			case 'p':                   /* proportional */
			case 'P':
				flags |= LF_PROPORT;
				break;

			case 's':                  /* stretch drawing */
			case 'S':
				flags |= LF_STRETCH;    /* set the stretch flag */
				break;

			default:
				FMmsg( E_UNKNOWNPARM, buf );   /* generate error message */
				break;
		} 
	}

	width = cur_col->width;           /* insert in the column text area */

	TRACE( 1, "fmep: open (%s)\n", fname );
	if( (efile = AFIopen( fname, "r" )) > OK )       /* if file opened ok */
	{
		ebuf = (char *) malloc( sizeof( char) * 1024 );  /* get read buffer */
		AFIsetsize( efile, 1023 );               	/* max read size */

		while( AFIread( efile, ebuf ) >= OK )    /* look for %%boundingbox: */
		{
			if( (tok = strstr( ebuf, "%%BoundingBox:" ) ) != NULL )
			{
				tok += 14;                 /* point past bounding box string */
				tok = strstr( tok, " " );  /* find the blank */
				if( tok != NULL )
				{
					while( *tok == ' ' )       /* find non blank */
						tok++;
					if( isdigit( *tok ) || tok[0] == '-' )  /* valid digit or -? */
					{
						llx = atoi( tok );         /* get lower left coords */
						tok = strstr( tok, " " );  /* find next blank */
						while( *tok == ' ' )       /* find non blank */
							tok++;

						lly = atoi( tok );
						tok = strstr( tok, " " );  /* find next blank */
						while( *tok == ' ' )       /* find non blank */
							tok++;

						urx = atoi( tok );         /* get upper right coords */
						tok = strstr( tok, " " );  /* find next blank */
						while( *tok == ' ' )       /* find non blank */
							tok++;

						ury = atoi( tok );
						break;
					}
				}                  /* end if tok not null */
			}
		}         /* end while - looking for bounding box */
		AFIclose( efile );       /* we'll reopen and reread header stuff later */
		efile = -1;

		if( urx != llx && ury != lly )  /* valid bounding box info found */
		{
			if( urx-llx > width ) {							/* need to reduce wid to fit*/ 
				xscale = (float) width / (float) (urx-llx);  /* calc scale to make fit */
			} else {
				if( flags & LF_STRETCH )   /* stretch to fill out? */
					xscale = (float) width / (float) (urx-llx); /* stretch scale */
			}

			if( ury-lly > length )     /* does drawing need y scale reduced to fit? */
				yscale = (float) length / (float) (ury-lly);  /* yes - calc to make fit*/
			else
				if( flags & LF_CLOSE )       /* len is larger than ep, close space ? */
					length = (ury - lly) + 10;  /* flag set so reduce the length */
				else
					if( flags & LF_CENTER )     /* center between top and length */
						yoffset = (length/2) - ((ury-lly)/2);
					else
						if( flags & LF_STRETCH )    /* stretch ? */
							yscale = (float) length / (float) (ury-lly);  /* calc stretch scale */
			TRACE( 2, "fmep: scale based on width=%d bbx=%d  bby=%d xscale=%.2f yscale=%.2f  \n",  width, urx-llx, ury-lly, xscale, yscale );

			if( flags & LF_PROPORT )   /* must we keep drawing proportional? */
			{
				if( yscale < xscale ) {     /* yes - we must select smallest */
					xscale = yscale;
				}else {
					if( yscale == 1 && (flags & LF_CLOSE) )  /* closing and room? */
						length = (length * xscale) + 10;        /* adjust length for scale */
					yscale = xscale;
				}
				TRACE( 2, "fmep: proportional options applied, scales now: xscale=%.2f yscale=%.2f  \n",   xscale, yscale );
			}

			if( flags & LF_CENTER ) {		// senter horizontally, but only after xscale is final
				xoffset = ((width - ((urx-llx) * xscale))/2);  /* calc xoffset */
			}

			/* postscript code to setup for ep file */
			AFIwrite( ofile, "100 dict begin\n" );    /* give them their own */
			sprintf( obuf, "gsave %d -%d translate %.3f %.3f scale\n", cur_col->lmar+xoffset, (cury+length)-yoffset, xscale, yscale );
			AFIwrite( ofile, obuf );                                /* set up */

			if( (efile = AFIopen( fname, "r" )) > OK )         /* reopen file to copy to output */
			{
				AFIsetsize( efile, 255 );              /* max read size */
	
				while( AFIread( efile, ebuf ) >= OK )
				{
					if( strncmp( ebuf, "%%BeginData:", 12 ) == 0 )
						inside_data = 1;
					else
						if( strncmp( ebuf, "%%EndData:", 10 ) == 0 )
							inside_data = 0;
						else
							if( strncmp( ebuf, "showpage" , 8 )  == 0 )	/* nasty -- should not be in an eps file */
								;
							else
								if( inside_data ||  *ebuf != '%' )    /* drop comments unless in data */
									AFIwrite( ofile, ebuf );              /* simple as is copy */
				}
				AFIclose( efile );
			}

			AFIwrite( ofile, "grestore end\n" );   /* clean up after */
			obuf[0] = EOS;                     /* clean up buffer */

			if( !(flags & LF_NOADV) )  /* if no advance flag is off */
				cury += length;           /* then advance cury past embedded stuff */
		}                           /* end if valid bounding info */
		else
			FMmsg( E_BBOX, fname );     /* could not find needed %% in ep file */

		free( ebuf );                      /* loose read buffer */
	}                                   /* end if file opened ok */
	else
		FMmsg( E_CANTOPEN, fname );         /* let user know */

	if( fname )
		free( fname );
}      
