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
****************************************************************************
*
*  Mnemonic: FMtoksize
*  Abstract: This routine will calculate an estimated token size in points
*            based on the current text size set.
*  Parms:    tok - Pointer to the start of the token
*            len - lengtht of the token
*  Returns:  Integer number of points required to display the token.
*  Date:     6 April 1994
*  Author:   E. Scott Daniels
*	Mods:	17 Jul 2016 - Bring decls into the modern world.
*
*****************************************************************************
*/
/* best guess if we cannot figure out a table in font.h based on name */
int unknown[ ] =  /* point sizes for 100 point characters */
 {
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
  32, 30, 38, 62, 62, 90, 80, 22, 30, 30, 44, 60, 32, 40, 32, 60,
  62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 32, 32, 60, 60, 60, 54,
  82, 68, 74, 74, 80, 72, 64, 80, 80, 34, 60, 72, 60, 92, 74, 80,
  62, 82, 72, 66, 62, 78, 70, 96, 72, 64, 64, 30, 60, 30, 60, 50,
  22, 58, 62, 52, 62, 52, 32, 54, 66, 30, 30, 62, 30, 94, 66, 56,
  62, 58, 44, 52, 38, 68, 52, 78, 56, 54, 48, 28, 60, 28, 60, 32,
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
  32, 30, 62, 62, 14, 62, 62, 52, 62, 22, 40, 36, 24, 24, 62, 62,
  32, 50, 54, 54, 32, 32, 60, 46, 22, 40, 40, 36,100,128, 32, 54,
  32, 34, 34, 42, 44, 44, 46, 26, 42, 32, 32, 32, 32, 38, 32, 42,
 100, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
  32,126, 32, 42, 32, 32, 32, 32, 60, 80,124, 42, 32, 32, 32, 32,
  32, 86, 32, 32, 32, 30, 32, 32, 32, 56, 90, 66, 32, 32, 32, 32,
 };          /* end character width table */

#include	"fonts.h"		/* widts for various fonts */

extern int FMtoksize( char* tok, int len )
{
 int i;            /* loop index */
 int size = 32;    /* calculated size - start assuming a blank seperates */
 int *charwidth = unknown;

 	if( len <= 0 )
  		return 0;				/* empty token has no size */

	switch( *curfont )
	{
		case 'T':
				charwidth = TimesRoman;
				break;
				/*
 				if( strcmp( curfont, "Times-roman" ) == 0 || strcmp( curfont, "Times-Roman" ) == 0 )
					charwidth = TimesRoman;
 				else
 				if( strcmp( curfont, "Times" ) == 0 )
					charwidth = TimesRoman;
 				else
 				if( strcmp( curfont, "Times-bold" ) == 0 || strcmp( curfont, "Times-Bold" ) == 0 )
					charwidth = TimesBold;
 				else
 				if( strcmp( curfont, "Times-italic" ) == 0 || strcmp( curfont, "Times-Italic" ) == 0 ||
					strcmp( curfont, "Times-oblique" ) == 0 || strcmp( curfont, "Times-Oblique" ) == 0 )
					charwidth = TimesItalic;
				break;
*/

		case 'C':
 			if( strcmp( curfont, "Courier" ) == 0 )
				return ((80 * strlen( tok ))* textsize)/100;		/* all chars are the same width */
			break;

		case 'H':
 				if( strcmp( curfont, "Helvetica" ) == 0 )
					charwidth = Helvetica;
 				else
 				if( strcmp( curfont, "Helvetica-Bold" ) == 0 )
					charwidth = HelveticaBold;
 				else
 				if( strcmp( curfont, "Helvetica-Italic" ) == 0 || strcmp( curfont, "Helvetica-italic" ) == 0 ||
 				 	strcmp( curfont, "Helvetica-oblique" ) == 0 || strcmp( curfont, "Helvetica-oblique" ) == 0 )
					charwidth = HelveticaItalic;
 				else
 				if( strcmp( curfont, "Helvetica-Oblique" ) == 0 )
					charwidth = HelveticaItalic;
				break;
	}
	
	for( i = 0; i < len; i++ )
		size += charwidth[tok[i]];       /* add value from table */

	size = (size * textsize) / 100;   /* adjust size for text size */

	return size; 
}               
