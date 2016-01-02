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
*  Mnemonic: FMoneject
*  Abstract: This saves the commands entered and executes them when the next 
*	     page eject occurs. Useful for restoring two column mode after 
*	     entering single column mode for a table or something.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     17 Aug 2001
*  Author:   E. Scott Daniels
*
*  Mods:	01 Jan 2016 - Added ability to avoid state saving (needed if .fm
*				used in an on eject command. Corrected potential buffer 
*				overrun bug.
*
* .oe [n=name] [nostate] [all] [list|del|col|page] <commands>
*
****************************************************************************
*/
struct eject_info {
	struct eject_info  *next;
	struct eject_info  *prev;
	char	*name;
	char	*cmd_str;
	int	flags;
};

#define EF_ALL	0x01		/* apply to all, not just next */
#define EF_COL	0x02		/* at next/all column ejects */
#define EF_PAGE 0x04		/* at next/all page ejects */
#define EF_DEL	0x08
#define EF_NOSTATE 0x10		// don't save state 

static struct eject_info *ej_head = NULL;
static struct eject_info *ej_tail = NULL;

static struct eject_info *find_ej( char *name )
{
	struct eject_info *ep;

	for( ep = ej_head; ep; ep = ep->next )
		if( strcmp( name, ep->name ) == 0 )
			return ep;

	return NULL;
}

static del_ej( struct eject_info *ep, char *name )
{
	struct eject_info *dp;

	if( ! ep && (! name || ! *name) )
		return;				/* should write a message */

	if( ep )
		dp = ep;			/* user had one in mind; may be one time, and unnamed */
	else
		dp = find_ej( name );

	if( !dp )
		return;
	
	if( dp->prev )
		dp->prev->next = dp->next;
	if( dp->next )
		dp->next->prev = dp->prev;

	if( ej_head == dp )
		ej_head = dp->next;
	if( ej_tail == dp )
		ej_tail = dp->prev;

	TRACE( 2, "fmoneject/del_ej: deleted: name=%s cmd=%s\n", name ? name : "unnamed", dp->cmd_str );
	if( dp->name )
		free( dp->name );

	free( dp->cmd_str );
	free( dp );
}

/* do the ejects for the specified type */
void FMateject( int page )
{
	FILE	*f = NULL;
	struct eject_info *ep;
	struct eject_info *nxt;		/* next so we can delete as we go if needed */
	char	*fname = "./.pfm_eject";
	int	type = 0;
	char	*tok = NULL;
	char	wrk[1024];

	if( page )
		type = EF_PAGE;
	else
		type = EF_COL;

	nxt = NULL;
	for( ep = ej_head; ep; ep = nxt )
	{
		nxt = ep->next;
		if( ep->flags & type )	
		{
			if( ! f && (f = fopen( fname, "w" )) == NULL  )
			{
				fprintf( stderr, "abort: ateject cannot open tmp file %s\n", fname );
				exit( 1 );
			}
	
			if( ep->flags & EF_NOSTATE == 0 )
				fprintf( f, ".pu\n" );					// save the current state before command

			fprintf( f, "%s\n", ep->cmd_str );

			if( ep->flags & EF_NOSTATE == 0 )
				fprintf( f, ".po\n" );					// restore state if saved

			TRACE( 1,  "ateject: queued for execution: %s [%s state]\n", ep->cmd_str, ep->flags & EF_NOSTATE ? "no" : "saved"  );

			if( ! (ep->flags & EF_ALL) )		/* if all flag is not on, then trash it after first use */
				del_ej( ep, NULL );
		}
	}

	if( f )
	{
		fclose( f );
		sprintf( wrk, ".im %s", fname );
 		AFIpushtoken( fptr->file, wrk );  	/* push to imbed our file and then run it */

		//FMpush_state();
		
   		flags = flags & (255-NOFORMAT);      /* turn off noformat flag */
   		flags2 &= ~F2_ASIS;                  /* turn off asis flag */

		if( FMgettok( &tok ) )
		{
			TRACE( 2, "ateject: running: %s\n", tok );
			FMcmd( tok );
		}

		//FMpop_state();
		unlink( fname );
	}
}

static list_ej( )
{
	struct eject_info *ep;

	for( ep = ej_head; ep; ep = ep->next )
		fprintf( stderr, "oneject data: type=%s name=%s cmd=%s\n", ep->flags & EF_PAGE ? "page" : "col", ep->name ? ep->name : "unnamed", ep->cmd_str );

}

void FMoneject( )
{
	int 	flags = 0;
	int		len;
	int	recognised = 1;
	char	*buf;
 	char	wrk[2048];  		/* tmp buffer to build new name in */
	char	*name = NULL; 
	int		count = 0;				// prevent buffer overrrun
	struct eject_info *ep = NULL;

	if( FMgetparm( &buf ) <= 0 )     /* get the first command or "all" */
 		return;                     /* no name entered then get out */

	wrk[0] = 0;
	while( recognised ) {
		switch( buf[0] ) {
			case 'a':
				if( strcmp( "all", buf ) == 0 ) {
					flags |= EF_ALL;
				} else {
					recognised = 0;
				}
				break;
	
			case 'c':
				if( strncmp( "col", buf, 3 ) == 0 ) {
					flags |= EF_COL;
				} else {
					recognised = 0;
				}
				break;
		
			case 'p':
				if( strncmp( "page", buf, 4 ) == 0 ) {
					flags |= EF_PAGE;
				} else {
					recognised = 0;
				}
				break;
		
			case 'd':
				if( strncmp( "del", buf, 3 ) == 0 ) {
					flags |= EF_DEL;
				} else {
					recognised = 0;
				}
				break;
		
			case 'n':
				if( strncmp( "n=", buf, 2 ) == 0 ) {
					name = strdup( buf+2 );
				} else {
					if( strncmp( "nostate", buf, 7 ) == 0 ) {
						flags |= EF_NOSTATE;
					} else {
						recognised = 0;
					}
				}
				break;

			case 'l':
				if( strncmp( "list", buf, 4 ) == 0 ) {
					list_ej( );
					return;
				} else {
					recognised = 0;
				}
				break;

			default:
				recognised = 0;
		}
		
		if( ! recognised ) {
			strcat( wrk, buf );				/* save first token of command and force exit */
		} else {
			if( FMgetparm( &buf ) <= 0 )     
				if( ! flags & EF_DEL )		/* if not deleting, then */
 					return;					/* there must be at least one command token if not deleting */
		}
	}

	while( (len = FMgetparm( &buf )) > 0 )
	{
		count += len;
		if( count > sizeof( wrk ) - 2 ) {
			FMmsg( -1, "abort: onejet: buffer overrun processing command tokens (2k limit)" );
			exit( 1 );
		}

		if( *wrk )						/* if not the first thing added */
			strcat( wrk, " " );			/* add a space before next thing */
		strcat( wrk, buf );
	}

	if( flags & EF_DEL )
	{
		del_ej( NULL, name );
		return;
	}

	if( ! flags )				/* default to column, one time */
		flags = EF_COL;

	ep = (struct eject_info *) malloc( sizeof( struct eject_info ) );
	if( !ep )
	{
		FMmsg( -1, "abort: onejet: unable to allocate ep struct" );
		exit( 1 );
	}

	memset( ep, 0, sizeof( struct eject_info ) );
	ep->flags = flags;
	ep->cmd_str = strdup( wrk );
	if( name )
		ep->name = name;

	TRACE( 1, "fmoneject: added: flags=x%02x name=%s cmd=%s\n", flags, name ? name : "unnamed", ep->cmd_str );

	if( ej_head )			/* list exists, add at the end of the list */
	{
		ej_tail->next = ep;
		ep->prev = ej_tail;
		ej_tail = ep;
	} else {
		ej_head = ej_tail = ep;
	}
}
