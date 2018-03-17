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
* If this code is modified and/or redistributed, please retain this
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
* Mnemonic: FMcss
* Abstract: Supports HTML cascading style sheets 
* Parms:    None.
* Returns:  Nothing.
* Date:     27 November 2002
* Author:   E. Scott Daniels
*
* Modified: 
*		05 Jul 2009 - Added :class to name token 
*		26 Jul 2015 - Allow multiple fields for something like border-right 1px,solid,grey
*		18 Jul 2016 - Add consistent, and sometimes modern, prototypes.
*		17 Mar 2018 - Fix compiler warnings on printf statements
* --------------------------------------------------------------------------------
* .cs file filename		set file name from which css style commands are loaded (must be before
*				the first command that causes a flush. 
* .cs start name[:class-name] {attrributes -- see below}  (starts a division)
* .cs table {atrributes} 		(builds the table style string)
* .cs cell {attributes}			(table cell style string)
* .cs end				(ends a divison) 
* Attributes: one or more may be defined.
	[bc=border-colour]
	[bs=border-style]
	[bw=border-width] (e.g. 1px)
	[bg=colour]
	[c=colour] 
	[fg=colour] 
	[f=fontfamily] 	(serif, sans-serif, cursive, fantasy, monospace)
	[fs=fontsize] 	(e.g. 10px)
	[l=left] 	(e.g. 10px)
	[t=top] 	(e.g. 10px)
	[w=width] 	(e.g. 10px)
	[s=fontstyle] 	(normal, italic, oblique)
	[h=height] 	(e.g. 10px)
	[p=positiontype] 	(relative, absolute)
	[i=textindent]		(e.g. 10px)
	[exact-name=value]	(there are just too many options to keep adding; so we do this now)
	[x=exact-name=value]	(old style exact value)

	Exact names of interest:
  		background;
  		position {absolute|relative}
   		top	(px)
   		left	(px)
   		right	(px)
   		bottom	(px)
		text-indent (px)
   		z-index 0,1,2...
   		opacity: (fully transparent)0.0...1.0 (fully opaque)
        	width
        	height
        	background-image url(pics/main_back.jpg)
        	background-repeat repeat|no-repeat
        	background-attachment fixed (dont' know what else)
        	background-position 0px 0px  (assume x,y)
        	border transparent|opaque
		border-style
		padding (px)
		color
		vertical-align:
		p{font-family:"Times New Roman", Times, serif;}

	border-style is one of 
		none : No border, sets width to 0. This is the default value. 
		hidden : Same as 'none', except in terms of border conflict resolution for table elements. 
		dashed : Series of short dashes or line segments. 
		dotted : Series of dots. 
		double : Two straight lines that add up to the pixel amount defined as border-width. 
		groove : Carved effect. 
		inset : Makes the box appear embedded. 
		outset : Opposite of 'inset'. Makes the box appear 3D (embossed). 
		ridge : Opposite of 'groove'. The border appears 3D (coming out). 
		solid : Single, straight, solid line.

****************************************************************************
*/
#define CSS_END 0
#define CSS_TABLE 1
#define CSS_DIV 2
#define CSS_CLASS 3

extern void FMcss( void )
{
	static int depth = 0;		/* if in a div then we set the no font flag */

	int type = CSS_END;		/* default if .cs is put in */
	int len;        /* length of parameter entered on .bx command */
	char *buf;      /* pointer at the input buffer information */
	char wbuf[2048];
	char	*cp;



	len = FMgetparm( &buf );   

	if( len > 0 )
	{
		if( strncmp( buf, "file", 4 ) == 0 )		/* dont flush before this check */
		{
			if( style_file )
				free( style_file );
			if( FMgetparm( &buf ) )
				style_file = strdup( buf );
			TRACE( 1, "css: style file set: %s\n", style_file );
			flags3 |= F3_NEED_STYLE;
			return;
		}

		FMflush( );
		*obuf = 0;
		if( toupper( buf[0] ) == 'S' )  /* start division */
		{
			char *cp;

			depth++;
			type = CSS_DIV;
			FMgetparm( &buf );		/* get name */
			if( (cp = strchr( buf, ':' )) != NULL )
			{
				*cp = 0;
				sprintf( obuf, "<div name=\"%s\" class=\"%s\" style=\"", buf, cp+1 );
			}
			else
				sprintf( obuf, "<div name=\"%s\" style=\"", buf );
			
		}
		else
		if( toupper( *buf ) == 'T' )	/* create a def for table */
		{
			type = CSS_TABLE;
			sprintf( obuf, "style=\"" );
		}
		else
		{
			if( strncmp( buf, "class", 5 ) == 0 )
			{
				type = CSS_CLASS;
				sprintf( obuf, "style=\"" );
			}
		}


		while( FMgetparm( &buf ) )  
		{
			*wbuf = 0;
			if( *(buf+1) == '=' || *(buf+2) == '=' )	/* assume old style x= or xx= short versions */
			{
				switch( *buf )
				{
					case 'b':
					switch( *(buf+1) )
					{
						case 'c':		/* border colour */
							sprintf( wbuf, "border-color: %s; ", buf+3 );
							break;

						case 'g':
							sprintf( wbuf, "background-color: %s; ", buf+3 );
							break;
							
						case 's':	/* border style */
							sprintf( wbuf, "border-style: %s; ", buf+3 );
							break;

						case 'w':		/* border width */	
							sprintf( wbuf, "border-width: %spx; ", buf+3 );
							break;
					}
					break;
					
					case 'c':
						sprintf( wbuf, "color: %s; ", buf+2 );
						break;
	
					case 'f':  			/* fs=fonsize | f=fontfamily */
						if( *(buf+1) == 's' )	/* font size */
						{
							sprintf( wbuf, "font-size: %s; ", buf+3 );
						}		
						else
						{
							sprintf( wbuf, "font-family: %s; ", buf+2 );
						}
						break;
	
					case 'h':
						sprintf( wbuf, "height: %s; ", buf+2 );
						break;
	
					case 'i':
						sprintf( wbuf, "text-indent: %s; ", buf+2 );
						break;
	
					case 'l':
						sprintf( wbuf, "left: %s; ", buf+2 );
						break;
	
					case 'p':
						sprintf( wbuf, "position: %s; ", buf+2 );
						break;
	
					case 's':
						sprintf( wbuf, "font-style: %s; ", buf+2 );
						break;
	
	
					case 't':
						sprintf( wbuf, "top: %s; ", buf+2 );
						break;
	
					case 'w':		/* w=nnn[px] */
						sprintf( wbuf, "width: %s; ", buf+2 );
						break;

					case 'x':		/* x=exact-name=value */
						if( (cp = strchr( buf+2, '=' )) != NULL )
						{
							*(cp++) = 0;
							sprintf( wbuf, "%s: %s; ", buf+2, cp );
						}
						break;
	
					default:
						break;
				}
			}
			else				/* assume if not x= or xx= then its keyword=value */
			{
				if( (cp = strchr( buf, '=' )) != NULL )
				{
					char *ccp;
					*(cp++) = 0;
					for( ccp = cp; *ccp; ccp++ )		// allow a,b,c and replace commas with spaces
						if( *ccp == ',' ) {
							*ccp = ' ';
						}
					sprintf( wbuf, "%s: %s; ", buf, cp );
				}
			}

			if( *wbuf != 0 )
			{
				TRACE(3, "css style added: %s\n", wbuf );
				strcat( obuf, wbuf );
			}
	
		}		/* end while */

		if( type == CSS_DIV )
			strcat( obuf, "\">" );
		else
			strcat( obuf, "\"" );
	}


	switch( type )
	{
		case CSS_END:
			sprintf( obuf, "</div>\n" );
			depth--;

		case CSS_DIV:
			TRACE(1, "css div: %s\n", obuf );
			AFIwrite( ofile, obuf );
			break;

		case CSS_TABLE:				/* deprecated */
			fprintf( stderr, "WARNING: use of .cs table is deprecated\n" );
			free( table_css );
			table_css = strdup( obuf );
				TRACE( 2, "fmcss: setting table_css = (%p) %s\n", table_css, table_css );
			break;

		case CSS_CLASS:
			break;
	}

	*obuf = 0;
	optr=0;

	if( depth > 0 )
		flags3 |= F3_NOFONT;		/* no font changes */
	else
		flags3 &= ~F3_NOFONT;

}                        /* fmcss */
