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
*****************************************************************************
*
*  TFM
*  Mnemonic: FMjustify
*  Abstract: This routine is responsible for sending out the current buffer
*            with the proper justification command and related parameters.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     12 November 1992
*  Author:   E. Scott Daniels
*
*  Modified: 04 Dec 1992 - To call flush if spaces are 0
*            10 Dec 1992 - To use AFI routines for ansi compatability
*             6 Apr 1994 - To take advantage of linelen being points now
*            23 Mar 2001 - Drastically modified to go back to TFM
*			07 Jul 2013 - Cleanup 
*			02 Jun 2015 - corrected problem with altrenateing end being 1 off.
*****************************************************************************
*/
void FMjustify( )
{
static int direction = 0;   /* direction of travel on spaces */

	int spaces = 0;        /* number of blank spaces */
	int i;             /* loop index */
	int j;
	int lnb;		/* index of last non-blank */
	int len;           /* length of the string for write */
	int need;          /* number of spaces that must be inserted */
	int add;           /* number to add to each existing space */
	int start = 0;     /* which token to start with */
	char *tok;         /* pointer at token */
	char jbuf[2048];   /* buffer to build in */

	if( ! (flags2 & F2_OK2JUST) )		// not enough tokens to justify
		return;

	if( ! (flags & JUSTIFY) )				// mode is off
		return;


	for( i = 0; obuf[i]; i++ )    /* count spaces and find last non blank */
		if( obuf[i] == ' ' )
			spaces++;
		else
			lnb = i;
	obuf[lnb+1] = EOS;                     /* ensure no blanks at end of str */

	need = (linelen/7) - strlen( obuf );   /* number of extra spaces at end */

	if( spaces == 0 )
		return;             /* none to do */

	add = need / spaces;

	if( add > 2 )             /* just cannot see the need to add more than this */
		return;                      /* just plain too many to add */

	while( add * spaces < need )   /* correct for rounding problem */
		add++;

	jbuf[0] = 0;
	tok = strtok( obuf, " " );
	start = direction ? (spaces - need)-1 : 0;
	direction = direction ? 0 : 1;

	while( tok )
	{
		strcat( jbuf, tok );
		strcat( jbuf, " " );
		if( --start < 0 )
		for( j = add; j > 0 && need > 0; j--, need-- )
		strcat( jbuf, " " );

		tok = strtok( NULL, " " );

		spaces--;                        /* makes things look nicer if we adjust */
		if( add * spaces > need && (add-1) * spaces >= need )
		add--;
	}

	strcpy( obuf, jbuf );
}  
