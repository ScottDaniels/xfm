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

#include <sys/types.h>
#include <stdio.h>     
#include <stdlib.h>
#include <unistd.h>
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
*****************************************************************************
*
*  Mnemonic: FMcolnotes
*  Abstract: Functions to support the .cn (column notes) command and the inclusion
*			of registered notes at the end of the column or at the end of the text.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     10 Mar 2013
*  Author:   E. Scott Daniels
*
*  Mods:	04 Jan 2016 - Force a break at the end of the text.
*			17 Mar 2018 - Correct compiler printf warnings
*			05 Jul 2018 - Add ability to set indent/line len and to turn block 
*				centering off as well.
*
* .cn start {atclose | atbot} [option(s)] font fontsize space
*		options may be any of:
*			s={<symbol> | none}
*			l=<linelen-value>{p|i}
*			i=<indent-value>{p|i}
* .cn end
* .cn show
*
*	Examples:
*		.cn start atbot s=none Times-Roman 8p 1i	.** no number/symbol added
*		.cn start atbot s=* Times-Roman 8p 1i
*		.cn start atbot  Times-Roman 8p 1i
****************************************************************************
*/

static struct cn_info *bot_list = NULL;
static struct cn_info *end_list = NULL;

static FILE	*bfile = NULL;
static FILE *efile = NULL;
static FILE *target = NULL;		/* current target for output */
static int	eid = 1;			/* note numbers */
static int  bid = 1;
static char bfname[1024];
static char efname[1024];


static void cnstart( )
{
	char	*buf;
	int		len;
	struct cn_info	*new;
	char	symbol = 0;
	int		i;
	int		*target_id;
	int		has_symbol = 1;			// if s=none given, then this is turned off and the column note text is unadorned
	int		indent = -1;			// i=value given
	int		llen = -1;				// l=value given
	int		need_init = 0;			// true if we need an initialisation string into target

	while( (len = FMgetparm( &buf )) > 0 )  // while at* keyword or ?=value 
	{
		if( strcmp( buf, "atclose" ) == 0 )
		{
			target_id = &eid;
			if( !(target = efile) )			/* yes this is an assignment! */
			{
				snprintf( efname, sizeof( efname ), "%d.ecnfile", getpid() );
				TRACE( 2, "colnotes: opening efile: %s\n", efname );
				target = efile = fopen( efname, "w" );
				if( target == NULL )
				{
					fprintf( stderr, "unable to open tmp file: %s\n", efname );
					exit( 1 );
				}
				need_init = 1;
				//fprintf( target, ".pu\n.ju off\n.sp .5\n.lw 0\n.hl\n" );
			} else {
				//fprintf( target, ".sp .5\n" );
			}
		} else {
			if( strcmp( buf, "atbot" ) == 0 ) {
				target_id = &bid;
				if( !(target = bfile) )			/* yes this is an assignment! */
				{
					snprintf( bfname, sizeof( bfname ), "%d.bcnfile", getpid() );
					TRACE( 2, "colnotes: opening bfile: %s\n", bfname );
					target = bfile = fopen( bfname, "w" );
					if( target == NULL )
					{
						fprintf( stderr, "unable to open tmp file: %s\n", bfname );
						exit( 1 );
					}
					//fprintf( target, ".pu\n.ju off\n.sp .5\n.lw 0\n.hl\n.sp .5\n" );
					//fprintf( target, ".pu\n.bc end\n.ju off\n.sp .3\n.lw 0\n.hl\n" );
					need_init = 1;
				} else {
					//fprintf( target, ".sp\n" );
					//fprintf( target, ".sp .4\n" );
				}
			} else {
				if( *buf && buf[1] == '=' ) {				// ?=parm is given
					switch( *buf ) {	
						case 's':							// symbol given rather than using one we'd generate
							if( strcmp( buf, "s=none" ) == 0 ) {
								has_symbol = 0;
							} else {
								symbol = *(buf+2);
							}
							break;

						case 'i':							// indent given
							indent = FMgetpts( buf+2, 0 );
							break;

						case 'l':							// line len given
							llen = FMgetpts( buf+2, 0 );
							break;
					}
				} else {
					break;					// assume positional parms remain if not at* or ?=
				}
			}
		}
	}

	if( target == NULL )
	{
		fprintf( stderr, "bad column notes (.cn) command parameters: expected: .cn start {atbot|atend} <font> <size> <space>\n" );
		exit( 1 );
	}

	if( need_init ) {				// we opened the file, so must write a complete initialisation command string
		//fprintf( target, ".pu\n.bc end\n.ju off\n.sp .3\n.lw 0\n" ); 		// every time
		fprintf( target, ".fo\n.pu\n.bc end\n.ju off\n.sp .3\n.lw 0\n" ); 		// every time

		if( llen > 0 ) {													// if options given
			fprintf( target, ".ll %dp\n", llen );
		}
		if( indent > 0 ) {
			fprintf( target, ".in %dp\n", indent );
		}

		fprintf( target, ".hl\n" );						// finally draw the line
	} else {
		fprintf( target, ".sp .5\n" );		// existing file, just space a bit
	}

	i = 0;
	while( len > 0 )		/* expect font, size and space (to reserve) */
	{
		switch( i++ )
		{
			case 0:
				fprintf( target, ".sf %s\n", buf );
				break;

			case 1:
				fprintf( target, ".st %s\n", buf );
				break;

			case 2:
				if( target_id == &bid )					/* a bottom of col rather than close */
					if( ! cn_space )					// add a gutter
						cn_space = 10;

					cn_space += FMgetpts( buf, len );
				break;

			default:		break;		/* ignore rest */
		}

		len = FMgetparm( &buf ); 
	}

	TRACE( 1, "colnotes: now reserving %dp at end of col for note(s)\n", cn_space );

	/* check symbol and use that */
	if( target_id == &eid )														/* end notes have [%d] rather than super script format */
		fprintf( target, ".tf superscript 2/3 ^[%d]\n", (*target_id)++ );		/* writes in default font */
	else
	{
		if( has_symbol ) {
			if( symbol != 0 )
				fprintf( target, ".ll -.2i .in +.15i .tf superscript ZapfDingbats 2/3 %c\n", symbol );		/* writes in default font */
			else
				fprintf( target, ".in +.15i .ll -.2i .tf superscript 2/3 %d\n", bid++ );		/* writes in default font */
		} else {
			fprintf( target, ".ll -.2i .in +.15i\n" );											// just indent
		}
	}


	/* continue to read input until we find end or the end command -- write stuff to the target file */
	while( FMgettok( &buf ) )
	{
		if( strcmp( buf, ".cn" ) == 0 )
		{
			FMgetparm( &buf );				/* we'll assume its .cn stop -- dont allow nested */
			break;
		}
		fprintf( target, "%s ", buf );
	}

	fprintf( target, "\n.br .in -.15i .ll +.2i\n" );

	TRACE( 2, "colnotes: end notes cn_space=%d\n", cn_space );
}

/*
	put the closing bits to the target file and turn off target.
*/
static void cnend( )
{
	if( target != NULL ) {
		fprintf( target, ".po\n" );
	}

	target = NULL;
}

/*
	show the col notes by closing the file and pushing an imbed 
	command onto the stack.  the last command in the file is a 
	.cn command to unlink the file (we'll leave droppings in the file
	system if the user causes us to crash, but thats low risk).

	returns true if something was pushed -- needed for the end;
*/
extern int FMcolnotes_show( int end )
{
	char	buf[1024];
	FILE	*target = NULL;
	int		dref_bfile = 0;
	char	*fname = NULL;
	int		status = 0;
	char	*end_cmd = "";

	if( bfile != NULL )		// target is always the b file if it's there
	{
		target = bfile;
		dref_bfile = 1;
		bfile = NULL;
		fname = bfname;
	}
	if( end )				/* end of doc (atclose) */
	{
		FMflush( );
		if( target == NULL ) {			// end and no bfile; try end file now
			target = efile;
			efile = NULL;
			fname = efname;
		}

		end_cmd = ".qu";				// needed to force end processing after our embed (might bring us back to do efile if bfile exists
	}
	
	TRACE( 1, "colnotes: showing at: %s cury=%d boty=%d cn_space=%d page=%d target=%p fl=%04x\n", 
		end ? "end of doc" : "bottom of col", cury, boty, cn_space, page, target, flags );

	if( target == NULL )
		return 0;

	if( cn_space > 0 )
		if( boty - cn_space > cury )
			cury = boty - cn_space;
	TRACE( 1, "colnotes: after adjustment cury=%d\n", cury );

	cn_space = 0;


	fprintf( target, ".br\n.po\n" );					// add the ending commands to the file (force flush and pop)

	if( ! end )
		fprintf( target, ".cb\n" )	;					// new col only if not at end; causes extra page eject if at end
	if( flags & JUSTIFY )
		fprintf( target, ".ju on\n" );					// justify back on if needed
	fprintf( target, ".cn unlink %s\n%s\n", fname, end_cmd );

	if( flags & NOFORMAT ) {							// if in no format mode, we must add one last to put it back
		fprintf( target, ".nf\n" );
		flags3 |= F3_CNPEND;							// signal to nofmt funcion that it needs to yield
		TRACE( 1, "colnotes: show: setting nf f3=%04x xx=%x\n", flags3, F3_CNPEND );
	}

	fclose( target );
	if( dref_bfile ) {
		bfile = NULL;		// cant use a closed file
	} else {
		efile = NULL;
	}
	target = NULL;

	snprintf( buf, sizeof( buf ), "\n.im %s", fname );				/* must have a guarding newline as lead (prevent accidents with .sp without optional parm etc.) */
 	status = AFIpushtoken( fptr->file, buf );  						/* push to imbed the file */
	TRACE( 2, "colnotes: pushing: stat=%d (%s)\n", status, buf );
	*fname = 0;

	return 1;
}

extern void FMcolnotes( )
{
	char	*buf;

	while( FMgetparm( &buf ) > 0 )
	{
		if( strcmp( buf, "start" ) == 0 )
			cnstart();
		else
		if( strcmp( buf, "showend" ) == 0 )
			FMcolnotes_show( 1 );
		else
		if( strcmp( buf, "show" ) == 0 )
			FMcolnotes_show( 0 );
		else
		if( strcmp( buf, "end" ) == 0 )
			cnend();
		else
		if( strcmp( buf, "unlink" ) == 0 )
		{
			if( FMgetparm( &buf ) > 0 )		/* unlink the used file we put on the imbed command */
				unlink( buf );	
		}
	}
}
