/*
=================================================================================================
	(c) Copyright 1995-2011 By E. Scott Daniels. All rights reserved.

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
*  Mnemoic:	vreplace
*  Abstract:	Replace variables (e.g. %name) in a buffer with the value that the name
*		maps to in the symbol table.  Assuming the percent sign is used as the derefence 
*		operator, the following are legal expansions:
*			%scooter
*			%{scooter}junk
*			junk%scooter
*			%scooter(p1:p2:p3:p4)
*
*		The last will substitute the paramters (p1-p4) into the variable value where it 
*		finds $1-$4 in any order or with any number of occurances. Thus if the variable 
*		scooter had been defined as "now is the time for $1 to come to the aid of their $2"
*		and the string "%scooter(aardvarks:fellow aardvarks)" the result would be:
*		"now is the time for aardvarks to come to the aid of their fellow aardvarks"
*  Parms:	st - pointer to symbol table
*		namespace - symboltable name space key (a special key 43086) is used for things that
*			are put into the table by this routine (parameter expansion). This can be 
			changed via the replace_set_pvt() function.
*		sbuf - source buffer 
*		vsym - variable indicator % $ etc
*		esym - escape symbol so that \% will cause a % to go into the expanded buf
*		psym - Parameter seperator for %name(p1:p2p3) expansions
*		flags:	0x01 - create a new buffer for expansion; user must free
*			0x02 - Do NOT do recursive expansion on variables
*  Returns:	Pointer to the expanded buffer
*  Date:	6 January 2001
*  Author: 	E. Scott Daniels
*  Mods:	15 Dec 2005 (sd) : corrected parsing of ^&
*		30 Apr 2006 (sd) : corrected parsing of ^: (HBD KAD!)
*		03 Apr 2007 (sd) : extended name length to 128 (for no good reason other than the day)
*		10 Apr 2007 - Memory leak repair
*		21 Aug 2011 - Fixed bug that was causing lead portion of buffer to be overwritten when 
*			a variable didn't expand during recursion. 
*			03 Aug 2019 - Fixed bug that was returning garbage if unset parm was referenced.
* -----------------------------------------------------------------------------------------------
*/

#include	<unistd.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>

#include	"../base/symtab.h"
#include	"parse.h"
#include	"aficonst.h"

static	char vreplace_buffer[4096];
static	unsigned pvt_class = 43086;			// class in caller's symtab  that we use for parms

#define F_PARMS	0x01			/* internal flag - expand parms first */

/*
	Allow user to set the namespace in the symtab that we are allowed to use.
	If caller doesn't invoke, then the default 43086 is used.
*/
extern void replace_set_pvt( unsigned int value ) {
	pvt_class = value;
}

char *vreplace( Sym_tab *st, int namespace, char *sbuf, char vsym, char esym, char psym, int flags )
{
	char	name[MAX_NAME];		/* spot to save a var name for look up */
	char	pname[MAX_NAME];		/* parameter name buffer */
	char	plist[4096];		/* list of parameters that were passed with variable */
	char	*obuf = NULL;		/* pointer to buf to return */
	char	*optr = NULL;
	char	*vbuf;			/* pointer to variable value string */
	char 	*oend;			/* prevent buffer overruns */
	char	*ebuf;			/* full expansion of variable - ready for cp to obuf */
	char	*eptr;			/* pointer into ebuf */
	int	nidx;			/* index into name buffer */
	int	pidx;			/* parameter list index */
	int	p;			/* parameter number */
	int 	iflags = 0;		/* internal flags */
	int 	need;			/* number of ) needed to balance ( */
	static int depth = 0;		/* prevent a never ending recursion by aborting if we go too deep */


	if( ++depth > 25 )
	{
		fprintf( stderr, "vreplace: panic: too deep\n" );
		abort( );
	}

	if( flags & VRF_NEWBUF )
		obuf = (char *) malloc( 4096 * sizeof( char ) );
	else
		obuf = vreplace_buffer;

	optr = obuf;
	oend = obuf + 4095;

	while( sbuf && *sbuf && (optr < oend) )	
	{
		if( *sbuf == vsym )
		{
			nidx = 0;
			sbuf++;
			if( *sbuf == '{' )
			{
				sbuf++;
				while( nidx < MAX_NAME-1 && *sbuf && *sbuf != '}' )
					name[nidx++] = *sbuf++;
				name[nidx] = 0;
				sbuf++;					/* step over end } */
			}
			else
			{
				while( nidx < MAX_NAME-1 && *sbuf && (isalnum( *sbuf ) || *sbuf == '_' ) )
					name[nidx++] = *sbuf++;
				name[nidx++] = 0;

				if( *sbuf == '('  )		/* capture parameter after %name( */
				{
					sbuf++;				/* skip lead ( */
					iflags |= F_PARMS;				/* must expand for parms too */
					pidx = 0;
					p = 1;
					need=1;					/* number of ) needed */
					while( pidx < 4095 && *sbuf && need )		/* while we need ) and have room */
					{
						sprintf( pname, "%d", p );
						sym_map( st, (unsigned char *) pname, pvt_class, plist+pidx );

						while( pidx < 4095 && *sbuf && need && *sbuf != psym )
						{
							if( *sbuf == esym && *(sbuf+1) == psym )	/* skip ^: */
								sbuf++;
							if( *sbuf == esym && *(sbuf+1) == vsym )	/* skip ^& */
								sbuf++;
							else
							if( *sbuf == '(' )
								need++;
							else
							if( *sbuf == ')' )
								need--;

							if( need )
								plist[pidx++] = *sbuf++;

						}

						plist[pidx++] = 0;
						sbuf++;
						p++;
					}
				}
			}

			if( (vbuf = sym_get( st, (unsigned char *) name, namespace )) )
			{
				if( iflags & F_PARMS )
					vbuf = vreplace( st, pvt_class, vbuf, '$', esym, psym, VRF_LEAVE_EMPTY | VRF_NORECURSE | VRF_NEWBUF );

				if( ! (flags & VRF_NORECURSE) )		/* recursion is allowed (user did not set no flag) */
				{
					eptr = ebuf = vreplace( st, namespace, vbuf, vsym, esym, psym, VRF_NEWBUF );
				}
				else
				{				/* no recursion, just copy in the variable value */
					eptr = vbuf;
					ebuf = NULL;		/* free will not barf */
				}

				while( *eptr && (optr < oend) )
					*optr++ = *eptr++;	

				if( iflags & F_PARMS )
					free( vbuf );
				free( ebuf );
			}
			else
			{
				if( flags & VRF_LEAVE_EMPTY ) {			// if no value, return an empty buffer
					*optr = 0;
				} else {
					*optr++ = vsym;						/* 2011/08/21 fix */
					*optr = 0;
					strcat( obuf, name );
					optr = obuf + strlen( obuf );
				}
			}
		}
		else
		if( *sbuf == esym )		/* pass them both back; caller will strip ^& to & if needed */
		{
			*optr++ = *sbuf++;
			if( *sbuf )				/* if ^\000 then we do not copy the end of string */
				*optr++ = *sbuf++;
		}
		else
			*optr++ = *sbuf++;	/* nothing special, just add the character */
	}

	sym_clear_class( st, pvt_class );		// delete all parms we stuffed in to prevent garbage next time

	*optr = 0;
	depth--;
	return obuf;
}

#ifdef SELF_TEST

#include "../base/symtab.h"
#include "../base/symtab.c"

int main( int argc, char **argv )
{
	Sym_tab *st;
	char *e;
	int i;

	st = sym_alloc( 17 );
	sym_map( st, "fred", 0, "fred was here and he does not like %barney!" );
	sym_map( st, "wilma", 0, "wilma is cool %fred" );
	sym_map( st, "barney", 0, "barney is a dork" );
	sym_map( st, "betty", 0, "betty is married to a dork" );
	sym_map( st, "bambam", 0, "bambam is loud and has betty for a mom and a dork for a dad" );
	sym_map( st, "dino", 0, "p4=$4 p3=$3 p1=$1 p2=$2 p4=$4" );

	if( argc < 2 ) {
		fprintf( stderr, "self test for vreplace requires one or more input strings to expand\n" );
		fprintf( stderr, "e.g. replace %fred \"%fred %barney\" \"%dino(abc:123:ef:345)\"\n" );
		fprintf( stderr, "these variables are defined: fred, wilma barney betty bambam and dino\n" );
		fprintf( stderr, "the variable 'fred' has an imbedded reference to barney to test recursion\n" );
		fprintf( stderr, "the variable 'wilma' has an imbedded reference to fred  to test double recursion\n" );
		fprintf( stderr, "dino accepts up to 4 parameters and prints them out\n" );
		fprintf( stderr, "the variable deref symbol is %% (su use %%fred to expand fred)\n" );
		fprintf( stderr, "the var sep is :, so use %%dino( abc:def:ghi:jkl ) to expand dino\n" );
		fprintf( stderr, "\n" );
		exit( 1 );
	}
	
	for( i = 1; i < argc; i++ ) {
		//fprintf( stderr, "expanding = (%s) \n", argv[i] );
		e = vreplace( st, 0, argv[i], '%', '\\', ':', 0 );
		printf( "(%s) --> (%s)\n", argv[i], e );
	}
}
#endif
