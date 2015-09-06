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

/*
* ---------------------------------------------------------------------------
* This source code is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* If this code is modified and/or redistributed, you must retain this
* header, as well as any other 'flower-box' headers, that are
* contained in the code in order to give credit where credit is due.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* Please use this URL to review the GNU General Public License:
* http://www.gnu.org/licenses/gpl.txt
* ---------------------------------------------------------------------------
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


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
****************************************************************************
*
*   Mnemonic: FMflush
*   Abstract: This routine is responsible for flushing the output buffer
*             to the output file and resetting the output buffer pointer.
*   Parms:    None.
*   Returns:  nothing
*   Date:     15 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 	29 Jun 1994 - To convert to rfm.
*              	06 Dec 1994 - To reset font even if nothing to write.
*              	07 Dec 1994 - To prevent par mark at page top if in li list
*             	14 Dec 1994 - To prevent par mark at top if in di list
*              	06 Dec 1996 - To convert for hfm
*              	09 Nov 1998 - To remove </font> - Netscape seems unable to handle them properly
*             	22 Jan 2000 - To strncmp for color rather than strcmp
*		27 Nov 2002 - To add no font support
*		21 Jul 2010 - html 4.0/5.0 changes.
****************************************************************************
*/
int FMflush( )
{
	int fd;
	int len;           		/* length of the output string */
	char fbuf[2048];    		/* buffer to build flush strings in */
	//char colour[50];   /* colour string if textcolour defined */

	//colour[0] = 0;
	TRACE( 2, "flush: textsize=%d optr=%d\n", textsize, (int) optr );

	if( flags3 & F3_NEED_STYLE )
	{
		// setup style stuff and then header
		if( doc_title )
		{
			sprintf( fbuf, "<title>%s</title>", doc_title );
			AFIwrite( ofile, fbuf );
		}

		if( style_file )
		{
			TRACE( 1, "reading style file: %s\n", style_file );
			if( (fd = open( style_file, O_RDONLY )) >= 0 )
			{
				AFIwrite( ofile, "<style type=\"text/css\">" );
				while( (len = read( fd, fbuf, 2047 )) > 0 )	/* leave room to put an end of string */
				{
					fbuf[len] = 0;
					AFIwrite( ofile, fbuf );
				}
				close( fd );
				AFIwrite( ofile, "</style><body>" );
			}
			else
				fprintf( stderr, "ERROR: unable to open style file: %s\n", style_file ); 
		}
		else	/* no user styles, use colours that they might have set */
		{
			TRACE( 1, "no user style file, using defaults: textsize=%d optr=%x\n", textsize, optr );

			sprintf( fbuf, "<style> a.link { color: %s; } a.visited{ color: %s; } a.hover{ color: %s; } a.active{ color: %s; }</style>", linkcolour ? linkcolour : "blue", vlinkcolour ? vlinkcolour : "blue", hlinkcolour ? hlinkcolour : "blue", linkcolour ? linkcolour : "blue" );
			sprintf( fbuf, "<body style=\"color: %s; background-color: %s; font-size: %dpt; font-family: %s \">", textcolour ? textcolour : "white", backcolour ? backcolour : "black", textsize, curfont );	
			AFIwrite( ofile, fbuf );
		}

		flags3 &= ~F3_NEED_STYLE;
	}

	if( optr == 0 )  			/* nothing to do so return -- must be after style check */
		return;

	*fbuf = 0;
	if( flags2 & F2_SETFONT )			/* font/size/colour was changed; set before flushing text */
	{
		TRACE( 2, "before pop: textsize=%d\n", textsize );
		FMele_stack( ES_POP, ET_SPAN );		/* pop previous if we can */
		TRACE( 2, "after pop: textsize=%d\n", textsize );
		sprintf( fbuf, "<span style=\"color: %s; background-color: %s; font-size: %dpt; font-family: %s;\">", textcolour, backcolour, textsize, curfont );
		flags2 &= ~F2_SETFONT;
		FMele_stack( ES_PUSH, ET_SPAN );	/* pop previous if we can */
		AFIwrite( ofile, fbuf );		/* add stop/start division if it is there */
	}

#ifdef KEEP
	//if( flags2 & F2_SETFONT && ! (flags3 & F3_NOFONT) )    	/* no font means we are in a css div */
	if( flags2 & F2_SETFONT  )
	{                           
		if( textcolour )
			sprintf( colour, "color=%s ", textcolour );
		sprintf( fbuf, "</font><font size=%d %s>", textsize < 1 ? 2 : textsize, colour );
		AFIwrite( ofile, fbuf );
		flags2 &= ~F2_SETFONT;
	}
#endif

	AFIwrite( ofile, obuf );   		/* write queued text */

	*obuf = 0;             /* fresh start */
	optr = 0;

	if( flags2 & F2_CENTER )       /* need to write out end center tag */
		AFIwrite( ofile, "</center>" );

	return 0;
}

