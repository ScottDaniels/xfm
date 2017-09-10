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
* ---------------------------------------------------------------------
* Mnemonic:		fmfmt.c contains routines to add, purge, and otherwise 
*				manipulate the format_blk list.
* Date: 		9 October 2001
* Author:		E. Scott Daniels
*				10 Apr 2007 - Memory leak cleanup 
*				06 Nov 2007 - Pops leftover blocks when restoring state.
*				07 Jul 2013 - cleanup 
*				22 Mar 2016 - Prevent out of range issue on restore.
*				17 Jul 2016 - Bring decls into the modern world.
* ---------------------------------------------------------------------
*/

#define FMT_STACK_SIZE	25

//void FMfmt_add( );

static struct format_blk *fmt_lst = NULL;	/* pointer to the current format list */
static struct format_blk *fmt_stack[FMT_STACK_SIZE];
static int fmt_idx = 0;

extern void FMfmt_dump( void )
{
	struct format_blk *f;
	
	for( f = fmt_lst; f; f = f->next )
		fprintf( stderr, "\tfmt: %s %d %s\n", f->font, f->size, f->colour ? f->colour : "no-colour" );
}

/*
	push the current list and start a new one
*/
extern int FMfmt_save( void )
{
	TRACE( 2, "fmt_save: idx=%d pushing list=%p\n", fmt_idx, fmt_lst );
	fmt_stack[fmt_idx++] = fmt_lst;
	fmt_lst = NULL;

	if( fmt_idx > FMT_STACK_SIZE )
	{
		fprintf( stderr, "abort: internal error in FMfmt: save/push stack overrun\n" );
		exit( 1 );
	}

	FMfmt_add( );		/* initiate a new list */
}

/*
	Reset the list to what was previously saved.
*/
extern int FMfmt_restore( void )
{
	int size;
	char *font;
	char *colour;
	int start;
	int end;
	int	ydisp;

	while( FMfmt_pop( &size, &font, &colour, &start, &end, &ydisp ) > 0 );		/* pop current things */
	if( fmt_idx > 0 ) {
		fmt_lst = fmt_stack[--fmt_idx];						/* point at old list */
	} else {
		FMfmt_add();
	}
	TRACE( 2, "fmt_restore: idx=%d list=%p\n", fmt_idx, fmt_lst );
}

extern int FMfmt_largest( void )				/* find the largest font in the list */
{
	struct format_blk *f;
	int 	size = textsize;
	
	for( f = fmt_lst; f; f = f->next )
	{
		if( f->size > size )
			size = f->size;
		// this was leaving notible gaps.  TODO - check displacement and adjust size only if it is really needed; general super/sub doesn't need this
	//	if( f->ydisp > 0 )							/* account for super/subscript */
	//		size += f->size - abs( f->ydisp );
	}

	TRACE( 2, "fmt_largest: list=%p size=%d\n", fmt_lst, size );
	return size;
}

/* mark the ending position of the top block */
extern void FMfmt_end( void )
{
	if( fmt_lst )
		fmt_lst->eidx = optr - 2;
}

/* add a block to the head of the list. current font and text size are used 
   we add to the head because we need to spit out the the stuff in reverse order
   because ps is stacked 
*/
static void add(  int ydisp )
{
	struct format_blk *new; 

	TRACE( 2, "fmt_add: optr=%d listbefore=%p\n", optr, fmt_lst );
	if( fmt_lst && fmt_lst->eidx == optr )		/* likely an .sf and .st command pair */
	{
		new = fmt_lst;			/* just reset the current one */
		if( new->font )
			free( new->font );
		new->font = strdup( curfont );
		new->size = textsize;
		new->ydisp = ydisp;
		new->sidx = optr ? optr-1: 0;
		if( textcolour )
			new->colour = strdup( textcolour );
	}
	else
	{
		if( (new = (struct format_blk *) malloc( sizeof( struct format_blk ) )) )
		{
			memset( new, 0, sizeof( struct format_blk ) );
			new->sidx = optr ? optr-1: 0;
			new->eidx = optr;
			new->ydisp = ydisp;
			if( curfont )
				new->font = strdup( curfont );
			new->size = textsize;
			if( textcolour )
				new->colour = strdup( textcolour );
	
			FMfmt_end( );
	
			new->next = fmt_lst;		/* push on */
			fmt_lst = new;
		}
		else
			fprintf( stderr, "*** no memory trying to get format block\n" );
	}
}

/*
	add with no y displacement
*/
extern void FMfmt_add( void )
{
	add( 0 );
}

/*
	add a block with a y displacement value 
*/
extern void FMfmt_yadd( int v )
{
	add( v );
}

/* 
	return the information from the top block and pop the block from the stack.
*/
extern int FMfmt_pop( int *size, char **font, char **colour, int *start, int *end, int *ydisp )
{
	struct format_blk *next;

	*colour = *font = NULL;

	if( fmt_lst )
	{
		TRACE( 2, "fmt_pop: %x cury=%d\n", fmt_lst, cury );
		*ydisp = fmt_lst->ydisp;
		*size = fmt_lst->size;
		*font = fmt_lst->font;			/* pass back strings we duped earlier */
		*colour =  fmt_lst->colour;
		*start = fmt_lst->sidx;
		*end = fmt_lst->eidx;

		next = fmt_lst->next;			/* remove from queue and purge */

		free( fmt_lst );
		fmt_lst = next;

		return 1;
	}
	else
	{
		TRACE( 2, "fmt_pop: end %x cury=%d\n", fmt_lst, cury );
		*ydisp = 0;
		*size = textsize;
		if( *font )
			free( *font );
		*font = strdup( curfont );
		*start = *end = -1;
		*colour = strdup( "000000" );
	}

	return 0;
}
