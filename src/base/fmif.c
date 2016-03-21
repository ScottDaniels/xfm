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
****************************************************************************
*
*  Mnemonic: FMif
*  Abstract: This routine is responsible for processing the conditional
*            source inclusion .if command. If the next token on the line
*            following the .if command is a defined variable then the
*            lines between the .if and .fi commands are processed normally.
*				Original version only tested for variable defined or not. We
*				allow for several types of testing now:
*					.if foo			.** true of foo is defined
*					.if ! foo		.** true if foo is not defined
*					.if "&foo" "string" =  .** true if the contents of foo are string
*					.if "&foo" "string" ! =  .** true if cntents of foo are not string
*					.if &foo 1 >        .** true if contents of foo are numeric and > 1
*
*            If the variable is not defined, then the lines are skipped.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     28 July 1992
*  Author:   E. Scott Daniels
*
*  Modified: 20 Jul 1994 - To allow not processing
*            27 Jul 1994 - To allow nested ifs and elses
*            28 Jul 1994 - To allow ors and ands
*            19 Mar 2016 - Correct problem with stack index.
*****************************************************************************
*/

#define VTY_CHAR  0
#define VTY_NUM   1

#define COP_AND   0
#define COP_OR    1
#define COP_NONE  2

struct val_blk
{
	int type;
	long nval;          /* numeric value */
	char cval[1024];    /* character value */
} ;

/* get parms until we find end quote */
static void get_string( char *buf, char *tok )
{
	char	*p;
	int	len;
	int	tlen = 0;

	len = strlen( tok );
	*buf = 0;

	do
	{
		if( tlen + len > 1023 )
		{
			fprintf( stderr, "if: string too long: %s\n", buf );
			return;
		}

		strcat( buf, tok );
		strcat( buf, " " );

		if( index( tok, '"' ) )
		{
			*(buf+tlen + len - 1) = 0;	/* stomp on " at end */
			if( trace )
				fprintf( stderr, "if/get_str: built %s\n", buf );
			return;
		}

		tlen += len + 1;
	}
	while( (len = FMgetparm( &tok )) != 0 );    /* get parm - if none then return */
}

void FMif( )
{
	char *tok;                   /* pointer to the token */
	char *var;                   /* pointer to expanded var string */
	int vidx = 0;                /* value index */
	int len;                     /* length of token */
	int not = FALSE;             /* not indicated flag */
	int cop = TRUE;
	int status[100];
	int sidx = 1;                /* status index */
	int nested = 0;              /* number of nested .fis to look for */
	struct val_blk value[3];    /* value of variables */
	int i;
	int count = 0;

	if( (len = FMgetparm( &tok )) == 0 )    /* get parm - if none then return */
	{
		FMmsg( E_MISSINGPARM, "On .if statement" );
		return;
	}

	status[0] = FALSE;
	memset( status, FALSE, sizeof( status ) );

	do                        /* test each name on the if statment */
	{
		TRACE( 1, "if: working on token=(%s)\n", tok );
		if( sidx >= 100 ) {
			fprintf( stderr, "WARNING: stack in if expression exceeded depth of 100\n" );
			return;
		}

		switch( tok[0] )
		{
			case 0:
				break;

			case '!':                /* not symbol? */
				not = TRUE;
				tok++;
				len--;
				break;

			case '|':
				if( sidx >= 2 )
				{
					status[sidx-2] = status[sidx-1] | status[sidx-2];
					sidx -= 2;
				}
				else
					status[sidx] = FALSE;
	
				sidx++;
				tok++;
				len--;
				break;

			case '&':                 /* & or &name */
				if( tok[1] != 0 )		/* late evaluating variable name -- we must expand for value */
				{
					char *vp = NULL;

					TRACE( 1, "fmif: expanding variable %s\n", tok );
					if( (vp = sym_get( symtab, tok+1, 0 )) != NULL )
					{
						if( isdigit( *vp ) )
						{
							TRACE( 2, "fmif: variable expanded to number: %s\n", vp );
							value[vidx].nval = atol( vp );
							status[sidx] = not ? ! value[vidx].nval : value[vidx].nval;
							value[vidx].type = VTY_NUM;
							vidx++;
						}
						else
						{
							TRACE( 2, "fmif: variable expanded to string: %s\n", vp );
							strcpy( value[vidx].cval, vp );
							value[vidx].nval = not ? 0 : 1;
							value[vidx].type = VTY_CHAR;
							vidx++;
						}
					}
					else
						value[vidx].nval = not ? 0 : 1;
					len = 0;			/* force get parm to fire */
				}
				else
				{
					TRACE( 1, "fmif: AND\n", tok );
					if( sidx >= 2 )
					{
						status[sidx-2] = status[sidx-1] & status[sidx-2];
						sidx -= 2;
					}
					else
						status[sidx] = FALSE;

					sidx++;
					tok++;
					len--;
				}
				break;

			case '<':
				if( sidx > 2 )
					sidx -= 2;
				else
					sidx = 1;
				if( vidx >= 2 )
				{
					if( value[0].type == VTY_NUM && value[1].type == VTY_NUM )
						status[sidx] = value[0].nval < value[1].nval;
					else
						if( strcmp( value[0].cval, value[1].cval ) < 0 )
							status[sidx] = TRUE;
						else
							status[sidx] = FALSE;

					if( not )
						status[sidx] = !status[sidx];
				}
				else
					status[sidx] = FALSE;

				sidx++;
				tok++;
				len--;
				vidx = 0;
				not = FALSE;
				break;

			case '>':
				if( sidx > 2 )
					sidx -= 2;
				else
					sidx = 1;
				if( vidx >= 2 )
				{
					if( value[0].type == VTY_NUM && value[1].type == VTY_NUM )
						status[sidx] = value[0].nval > value[1].nval;
					else
						if( strcmp( value[0].cval, value[1].cval ) > 0 )
							status[sidx] = TRUE;
						else
							status[sidx] = FALSE;
			
					if( not )
						status[sidx] = !status[sidx];
				}
				else
					status[sidx] = FALSE;

				sidx++;
				tok++;
				len--;
				vidx = 0;
				not = FALSE;
				break;
	
				case '=':
					TRACE( 3, "if: testing for equality\n" );
					if( sidx > 2 )
						sidx -= 2;
					else
						sidx = 1;

					if( vidx >= 2 )
					{
						TRACE( 3, "if: testing two values\n" );
						if( value[0].type == VTY_NUM && value[1].type == VTY_NUM )
						{
							status[sidx] = ! (value[0].nval - value[1].nval);
							TRACE( 2,  "num (%d) (%d) == %d (not=%d)\n", value[0].nval, value[1].nval, status[sidx], not );
						}
						else
						{
							status[sidx] = ! strcmp( value[0].cval, value[1].cval );
							TRACE( 2,  "if: strcmp: (%s) (%s) == %d (not=%d)\n", value[0].cval, value[1].cval, status[sidx], not );
						}
	
						if( not )
							status[sidx] = !status[sidx];
					}
					else
					{
						TRACE( 3, "if: only one value, default to false\n" );
						status[sidx] = FALSE;
					}

					sidx++;
					tok++;
					len--;
					vidx = 0;
					not = FALSE;
					break;

				case '"':
					tok++;
					len -= 2;
					get_string( value[vidx].cval, tok );
					value[vidx].nval = not ? 0 : 1;
					value[vidx].type = VTY_CHAR;
					vidx++;
					not = FALSE;
					len = 0;
					break;

				default:                                 /* characters of somesort */
					value[vidx].type = VTY_NUM;
					if( isdigit( tok[0] ) )
					{
						tok[len] = 0;
						value[vidx].nval = atol( tok );
						status[sidx] = not ? ! value[vidx].nval : value[vidx].nval;
						vidx++;
					}
					else
					{
						if( sym_get( symtab, tok, 0 ) == NULL )   /* defined? */
							status[sidx] = not ? 1 : 0;                  /* save only status */
						else
							status[sidx] = not ? 0 : 1;
					}

					len = 0;                           /* done with the parameter */
					not = FALSE;
					sidx++;
					break;
			}

		if( sidx < 100  &&  status[sidx] != 0 )
			status[sidx] = TRUE;              /* set specifically to true */

		if( vidx > 2 )
			vidx = 0;



		if( len <= 0 )
			len = FMgetparm( &tok );       /* need another token to process */
	}
	while( len > OK );               /* while parameters - evaluate them */


	TRACE(2, "fmif: expression evaluated to: %s\n", status[sidx-1] ? "TRUE" : "FALSE" );

	if( sidx > 0 ) {
		if( ! status[sidx-1] )             /* statement evaluated to false */
		{                                              /* then skip until .fi */
			while( FMgettok( &tok ) > OK )   /* while tokens still left in the file */
			{
				if( strncmp( ".if", tok, 3 ) == 0 )  /* found nested if */
					nested++;                           /* increase nested count */
				else
					if( strncmp( ".fi", tok, 3 ) == 0 )  /* found an end if */
					{
						if( nested == 0 )                   /* and we need only it */
							return;                            /* then were done */
						else
							nested--;                          /* one less were looking for */
					}
					else
						if( (strncmp( ".ei", tok, 3 ) == 0) && (nested == 0) )  /* last else */
							return;
			}
		}
		else
			return;
	} else {
		fprintf( stderr, "WARNING: internal error processing if: sidx == 0\n" );
		return;
	}

	FMmsg( E_UNEXEOF, ".if not terminated" );
}

