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
*****************************************************************************
*
*  Mnemonic: FMdv
*  Abstract: This routine adds a new variable to the variable list or
*            modifies an existing variable on the list. The escape character
*            is removed from the front of a parameter ONLY if the next
*            character is in the set: [ : `
*            This allows these characters to not have effect as parms to
*            the dv command, but to have effect when the variable is
*            expanded.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     7 July 1989
*  Author:   E. Scott Daniels
*
*  Modified: 16 Mar 1990 - To eliminate expansion of var on redefiniton.
*            10 Feb 1991 - To use MAX_VARxxx constatnts on name and expansion
*                          string copies.
*            28 Jul 1994 - Not to leave a trailing blank.
*            24 Apr 1997 - To allow escaped [ characters
*            21 Nov 1999 - To make string not use fixed buffer size
*			12 Jan 2002 - To convert to symbol table 
*			21 Aug 2011 - Added support for internal setting. 
****************************************************************************
*/

/* set a value now -- allows internal functions (like macro expansion or .gv) to 
   set the value in the middle of processing rather than effective with the next 
   command line read.
	2011/08/21
*/
void FMset_var( char *vname, char *buf )
{
	char *value = NULL;			/* dup buffer to set name to */
	char *ovalue = NULL;	 	/* original value */

	if( ! vname || ! *vname || !buf  )
		return;

	ovalue = sym_get( symtab, vname, 0 );		/* must free last */
	if( ovalue )
		free( ovalue );

	value = strdup( buf );
	sym_map( symtab, vname, 0, value );		/* add to symbol table */

	TRACE(2, "set_var: name=%s val=(%s)\n", vname, buf );
}

/* parses input buffer for .dv command and defines the variable */
void FMdv( )
{
	char *buf;               /* parameter pointer */
	char *value;	 	/* pointer to current value - if prev defined */
	char *ovalue;	 	/* original value */
	int len;                 /* length of the parameter */
 	char str[MAX_VAREXP+1];               /* pointer at the expansion string */
	char name[128];		/* variable name */
	int i;                   /* index vars */
	int j;
	char tbuf[MAX_VARNAME+1];  /* tmp buffer to build new name in */

	len = FMgetparm( &buf );     /* get the name */
	if( len <= 0 )
		return;                     /* no name entered then get out */

	strncpy( name, buf, 127 );
	name[128] = 0;				/* ensure its marked */

	ovalue = sym_get( symtab, name, 0 );		/* must free last incase referenced in the .dv string */

	varbuf = NULL;        /* not going to execute it so remove pointer */

	i = j = 0;       /* prep to load the new expansion information */

	while( (len = FMgetparm( &buf )) > 0 )   /* get all parms on cmd line */
	{
		if( trace > 1 )
			fprintf( stderr, "dv adding tok: (%s)\n", buf );

		for( i = 0; i < len && j < MAX_VAREXP; i++, j++ )
		{
			if( buf[i] == '^' )                      /* remove escape symbol */
			switch( buf[i+1] )                      /* only if next char is special*/
			{
				case ':':
				case '`':
				case '[':
				case '^':
					i++;                              /* skip to escaped chr to copy */
					break;

				default:                              /* not a special char - exit */
					break;
			}                                      /* end switch */

			str[j] = buf[i];                        /* copy in the next char */
		}

		str[j++] = BLANK;          /* seperate parms by a blank */
	}

	j = j > 0 ? j - 1 : j;    /* set to trash last blank if necessary */
	str[j] = EOS;             /* and do it! */

	if( trace )
		fprintf( stderr, "dv created var: %s = (%s)\n", name, str ); 

	value = strdup( str );
	sym_map( symtab, name, 0, value );		/* add to symbol table */
	if( ovalue )
		free( ovalue );
}                         /* FMdv */
