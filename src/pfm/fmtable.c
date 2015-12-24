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
* --------------------------------------------------------------------------
*  Mnemonic: fmtable 
*  Abstract: Start a table
*  Date:     26 Oct 2001 - converted from the hfm flavour 
*  Author:   E. Scott Daniels
*  Mods:
*		10 Apr 2007 - Memory leak cleanup.
*  		10 Oct 2007 : Changed to keep the original col anchor
*		29 Oct 2007 - Adding some trace points.
*		06 Nov 2007 - Corrected table width calculation.
*		21 Mar 2013 - Fixed bug that was preventing borders in a single cell table
*						from being painted correctly.
*		09 Mar 2015 - Added l= and B options to allow line weight to be set to 0
*						and to force edges for tables in tables.
*		22 Dec 2015 - Corrected bug with top border if page eject.
* 
*  The n option allows user to turn off (no execute) the automatic
*  creation of the first cell. This allows them to call .tr and .cl 
*  straight off in order to supply special values for that cell and row.
*  The B option forces borders (edgges) for tables in tables. l=0 sets
*  table line weight to smallest possible.
* 	.ta [l=line-weight] [B] [b[=0|1]] [n] [a=align] [w=xx] [xi...]
*		[p=padding] [s=cell-space] [class=css-class] [a=align-style]
*		[t=text-colour] [v=valign-style] [w=width-percent]
*		(Some html options are ignored for compatability.)
*  	.et
* 	.cl [c=bgcolour] [t=fgcolour] [s=span-cols] [a=align-type] (ignored: r= v=  t=)
*	.tr [n] [c=bgcolour] [a=alignval] [r=space] [v=valignvalue]
*	.th string
*  omitting w= allows browser to scale table. xx is a percentage.
*
* --------------------------------------------------------------------------
*/
static void tab_vlines( struct table_mgt_blk *t, int setfree );

void FMpause_table( void );

void FMtable( )
{
	struct table_mgt_blk *t;
	struct	col_blk 	*col;
	char colour[50];            /* spot to build colour string in */
	char align[50];
	char valign[50];
	char space[50]; 
	int do_cell = 1;         /* n option turns off */
	int len;
	char *ptr;
	int border = 0;          /* border size */
	char wstr[50];           /* spot to build width stirng in */
	int w = 100;

	space[0] = 0;
	align[0] = 0;
	wstr[0] = 0;
	sprintf( valign, "valign=top" );   /* default to vert as brows def is cent*/

	FMflush( );                         /* terminate the line in progress */

	if( ts_index >= MAX_TABLES ) {
		fprintf( stderr, "abort: too many tables max = %d\n", MAX_TABLES );
		exit( 1 );
	}
	
	t = table_stack[ts_index++] = (struct table_mgt_blk *) malloc( sizeof( struct table_mgt_blk) );

	memset( t, 0, sizeof( struct table_mgt_blk ) );
	t->col_list = firstcol;		/* hold things that need to be restored when done */
	t->cur_col = cur_col; 
	t->topy = cury;
	t->old_topy = topy;
	t->old_linelen = linelen;
	t->lmar = lmar;
	t->hlmar = hlmar;
	t->padding = 5;				/* padding inside of cell */
	t->maxy = cury;
	t->edge_borders = 0;			// default to no edges for table in table


	firstcol = NULL;
	cur_col = NULL;

	curcell = 1;        /* tableinfo[0] has cell count */
	colour[0] = (char) 0;

	while( (len = FMgetparm( &ptr )) != 0 )
		switch( *ptr )
		{
			case 'a': 
				sprintf( align, "align=%.13s", ptr + 2 );
				break;

			case 'B':
				t->edge_borders = 1;;
				break;

			case 'b':   
				if( *(ptr+1) == '=' )
					border = atoi( ptr + 2 );
				else
					border++;
				break;

			case 'c':
				if( strncmp( ptr, "class=", 6 ) == 0 )		/* ignore hfm class= */
					break;
/*
				t->bgcolour = strdup( ptr + 2 );
				sprintf( colour, "bgcolor=%.13s", t->bgcolour );
*/
				break;

			case 'l':				// line weight
				t->weight = atoi( ptr+2 );
				break;

			case 'n':
				do_cell = 0;    /* dont auto do a cell */
				break;

			case 'p':
				t->padding = atoi( ptr + 2 );
				break;
	
			case 's':
				sprintf( space, "cellspacing=%.13s", ptr +2 );
				//t->spacing = atoi( ptr+2 );
				break;

			case 't':
/*
				t->fgcolour = strdup( ptr + 2 );
				textcolour = t->fgcolour;
				flags2 |= F2_SETFONT;
*/
				break;

			case 'v': 
				sprintf( valign, "valign=%.13s", ptr + 2 );
				break;

			case 'w':          /* w=xxx percent of width to use for table */
				memcpy( wstr, ptr+2, len - 2 );
				wstr[len-2] = 0;
				w = atoi( wstr );
				sprintf( wstr, "width=%d%%", w );
				break;

			default:          /* assume column width definition */
				if( curcell < MAX_CELLS )
				{
					col = (struct col_blk *) malloc( sizeof( struct col_blk ) );
					memset( col, 0, sizeof( *col ) );
					col->next = NULL;
					col->width = FMgetpts( ptr, len ) - 4;
					/*t->border_width += col->width + (2 * t->padding );*/
					t->border_width += col->width + ( t->padding ) + 2;
					if( cur_col )
					{
						col->lmar = cur_col->lmar + cur_col->width + t->padding;
						cur_col->next = col;
						cur_col = col;
					}
					else
					{
						col->lmar = lmar + t->padding;
						cur_col = firstcol = col;
					}
					curcell++;                                 /* cell counter */
				}
				break;
		}

	firstcol->anchor =  t->col_list->anchor;		/* cary the anchor over for running head/feet */

	t->border = border;
	t->width = (linelen * w)/100;
	cur_col = firstcol;
	if( do_cell )
		linelen = cur_col->width - t->padding;
	else
		for( cur_col = firstcol; cur_col->next; cur_col = cur_col->next );

	lmar = firstcol->lmar + t->padding;

	if( t->edge_borders == 0 && ts_index > 1 )		/* if this is a table in a table w/o edge borders */
	{
		t->topy -= t->padding;
		topy = cury;			/* columns bounce back to here now */
	}
	else
	{
		topy = cury + t->padding;			/* columns bounce back to here now */
		if( border )						/* dont do outside for table in a table */
		{
			sprintf( obuf, "%d setlinewidth ", t->weight );
			AFIwrite( ofile, obuf );
			sprintf( obuf, "%d %d moveto %d %d rlineto stroke\n", t->lmar+t->padding, -cury, t->border_width-t->padding, 0 );
			AFIwrite( ofile, obuf );
		}
	}

	cury += t->padding;			/* allow a bit of room for the line */

	if( trace > 1 )
	{
		struct	col_blk 	*cp;
		int	i = 0;
		fprintf( stderr, "table: lmar=%d cury=%d borarder_width=%dp right_edge%dp\n", t->lmar, cury, t->border_width, t->lmar+ t->border_width );
		for( cp = firstcol; cp; cp = cp->next )
			fprintf( stderr, "table: cell=%d lmar=%dp width=%dp\n", i++, cp->lmar, cp->width );

	} 
}

/*
* --------------------------------------------------------------------------
*  Mnemonic: fmth
*  Abstract: save a table header placed into the table now, and if we page eject
*  Date:     02 Nov 2006 - converted from hfm
*  Author:   E. Scott Daniels
* 
*  .th string
* --------------------------------------------------------------------------
*/
void FMth( )
{
	struct table_mgt_blk *t;
	char	*buf;
	int	len; 
	int	totlen = 0;
	char	data[4096];

	if( ts_index <= 0 || (t = table_stack[ts_index-1]) == NULL )
	{
		while( (len = FMgetparm( &buf )) != 0 );		/* just dump the record */
		return;
	}
			
	*data = 0;
	while( (len = FMgetparm( &buf )) != 0 )
	{
		if( len + totlen + 1 < sizeof( data ) )
		{
			strcat( data, buf );
			strcat( data, " " );
		}
		totlen += len + 1;
	}

	if( t->header )
		free( t->header );
	t->header = strdup( data );

	TRACE( 2, "tab_header: header in place: %s\n", t->header );
	AFIpushtoken( fptr->file, t->header );			/* fmtr calls get parm; prevent eating things */
	
}

/* go to the next cell */
/*
* --------------------------------------------------------------------------
*  Mnemonic: fmcell
*  Abstract: Start  the next cell
*  Date:     26 Oct 2001 - converted from hfm
*  Author:   E. Scott Daniels
* 
*  .cl [c=bgcolour] [t=fgcolour] [s=span-cols] [a=align-type]
* --------------------------------------------------------------------------
*/

void FMcell( int parms )
{
	struct table_mgt_blk *t;
	int	span = 1;	/* number of defined columns to span */
	char buf[256];
	char buf2[256];
	char colour[50];        /* buffer to build bgcolor= string in */
	char rspan[50];          /* buffer to build rowspan= string in */
	char align[50];         /* buffer for align= string */
	char valign[50];         /* buffer for align= string */
	int len;
	char *ptr;

	align[0] = 0;
	buf[0] = 0;
	buf2[0] = 0;
	rspan[0] = 0;
	valign[0] = 0;
	colour[0] = 0;
	sprintf( valign, "valign=top" );

	if( ts_index <= 0 || (t = table_stack[ts_index-1]) == NULL ) {
		char *b;
		while( (len = FMgetparm( &b )) != 0 );		/* just dump the record */
		return;
	}

	FMflush( );                       /* flush what was in the buffer first */
 

	/* at this point we recognise but ignore some hfm things */
	while( parms && (len = FMgetparm( &ptr )) != 0 )
		switch( *ptr )
		{
			case 'a':
				sprintf( align, "align=%s", ptr + 2 );
				break;
			case 'c':   
				if( strncmp( ptr, "class=", 6 ) == 0 )			/* ignore hfm class */
					break;

				sprintf( colour, "bgcolor=%s", ptr + 2 );
				break;
			case 'r':
				sprintf( rspan, "rowspan=%s", ptr + 2 );
				break;

			case 's':
				span = atoi( ptr+2 );
				break;

			case 't':      /* really does not make sense to support this */
				break;
		
			case 'v':
				sprintf( valign, "valign=%s", ptr + 2 );
				break;
		
			default: 
				break;
		}


	if( flags2 & F2_BOX )    /* if a box is inprogress */
		FMbxend( );             /* then end the box right here */

	FMendlist( FALSE );      /* putout listitem mark characters if in list */

				
	TRACE(2, " table/cell: cury=%d textsize=%d textspace=%d font=%s boty=%d topy=%d maxy=%d\n", cury, textsize, textspace, curfont, boty, topy, table_stack[ts_index-1]->maxy );

	if( cury > table_stack[ts_index-1]->maxy )
		table_stack[ts_index-1]->maxy = cury;

	if( cur_col->next != NULL )    		/* if this is not the last in the table */
		cur_col = cur_col->next;	/* then select it */
	else
		cur_col = firstcol;

	cury = topy;
	lmar = cur_col->lmar + t->padding;   /* set lmar based on difference calculated */
	hlmar = cur_col->lmar + t->padding; /* earlier and next columns left edge */

	if( lilist != NULL )           /* if list then reset xpos for next col */
		lilist->xpos = cur_col->lmar + t->padding;

	if( span < 1 )
		span = 1;

	linelen = 0;
	while( span )
	{
		linelen += cur_col->width;
	
		if( --span && cur_col->next )		/* still more to span */
		{
			cur_col = cur_col->next;
			cur_col->flags |= CF_SKIP;
		}
	}

	linelen -= t->padding;
}


/*
---------------------------------------------------------------------------
 Mnemonic: fmtr
 Abstract: start a new row in the current table
 Date: 		26 Oct 2001 - converted from hfml stuff
 syntax:   .tr [n] [c=bgcolour] [a=alignval] [v=valignvalue]
 			The n option prevents cell from being called.

 Mods:		10 Apr 2007 -- fixed write of border to be in conditional.
---------------------------------------------------------------------------
*/
void FMtr( int last )
 {
	struct table_mgt_blk *t = NULL;
	char *ptr = NULL;             /* pointer at parms */
	int len = 0;
	int do_cell = 1;       /* turned off by n option */
	char colour[50];
	char align[50];
	char valign[50];
	char obuf[2048];
	int row_top = 0;		/* used to calc the depth of each row */
	int	old_cn_space = 0;

	colour[0] = 0;
	align[0] = 0;
	valign[0] = 0;

	if( ts_index <= 0 || (t = table_stack[ts_index-1]) == NULL ) {
		char *b;
		while( (len = FMgetparm( &b )) != 0 );			// no table, ditch parms and scoot
		return;
	}

	if( cur_col == firstcol )			/* we've not seen a .cl command (one col table) */
		t->maxy = cury;					/* force it to be set */
	row_top = cury;

	TRACE(2, "table/tr: cury=%d textsize=%d textspace=%d font=%s boty=%d topy=%d maxy=%d\n", cury, textsize, textspace, curfont, boty, topy, table_stack[ts_index-1]->maxy );

	while( cur_col != firstcol )
		FMcell( 0 );				/* flush and calc maxy for the row */
	
	t->tot_row_depth += t->maxy - row_top;		/* add in depth of this column */
	t->nrows++;
	t->ave_row_depth = t->tot_row_depth / t->nrows; 	/* average depth of a row to predict end of page */


   while( (len = FMgetparm( &ptr )) != 0 )
    {
     switch( *ptr ) 
      {
       case 'a':
         sprintf( align, "align=%s", ptr + 2 );
         break;

       case 'c':
		if( strncmp( ptr, "class=", 6 ) == 0 )			/* ignore hfm class */
			break;

         sprintf( colour, "bgcolor=%s", ptr + 2 );
         break;

       case 'n':
         do_cell = 0;
         break;

       case 'v':
         sprintf( valign, "valign=%s", ptr + 2 );
         break;


       default: 
         break;
      }
    }

	cury = t->maxy + t->padding;

	if( t->border )		// add top and vert for this row; bottom added at end if this is the last one
	{
		sprintf( obuf, "%d setlinewidth ", t->weight );
		AFIwrite( ofile, obuf );
		tab_vlines( t, 0 );					/* add vlines just for this row */

		TRACE( 2, "table/tr-border: cury=%d\n", cury );
		sprintf( obuf, "%d %d moveto %d %d rlineto stroke\n", t->lmar+t->padding, -cury, t->border_width-t->padding, 0 ); // top border
		AFIwrite( ofile, obuf );			// bottom line for the row
	}

	TRACE( 1, "table/tr: col note considered: cn_space=%d ard=%d cury=%d boty=%d\n", cn_space, t->ave_row_depth, cury, boty );
	
	// ??? Do we need to prevent eject if this is the last one?
	if( cn_space + cury + t->ave_row_depth + textsize + textspace >= (boty-8) )	// extra 8pts to have room for bottom line
	{
		FMpause_table();				// pause so we can eject to the next real column which might be a page eject
 		AFIpushtoken( fptr->file, ".rt" ); 	// must restart table;  push first so that col notes go before if needed
		old_cn_space = cn_space;			// eject will reset if set
		PFMceject(  );						// move to the top of the new col/eject page
		if( old_cn_space > 0 ) {			// if col note, must set it up and return so it is processes before new col is started
			t->maxy = topy;
			return;						// allow the col notes commands to play out first
		}

		topy = t->old_topy;
		cury = topy;
		t->maxy = t->topy;								// reset the mexy for the next col/page
	}

	t->topy = cury;
	if( ! last )
		cury += t->padding;

	topy = cury;			/* columns need to bounce back to here */
 }

/*
	Add vertical lines for a single row in the table.
*/
static void tab_vlines( struct table_mgt_blk *t, int setfree )
{
	struct col_blk *c;
	struct col_blk *next;

	for( c = (t->edge_borders || ts_index == 0) ? firstcol : firstcol->next; c; c = next )
	{
		sprintf( obuf, "%d setlinewidth ", t->weight );
		AFIwrite( ofile, obuf );

		next = c->next;
		if( (c->flags & CF_SKIP) == 0 )
		{
			sprintf( obuf, "%d %d moveto %d %d lineto stroke\n", c->lmar, -t->topy, c->lmar, -cury );
			AFIwrite( ofile, obuf );
		}

		c->flags &= ~CF_SKIP;

		if( setfree )
			free( c );
	}

	if( t->border && (t->edge_borders || ts_index < 2) )		// table in table gets edges only if forced on with B option
	{
		int x;

		TRACE( 2, "table/vlines: topy=%d cury=%d maxy=%d lw=%d\n", t->topy, cury, t->maxy, t->border );
		sprintf( obuf, "%d setlinewidth ", t->weight );
		AFIwrite( ofile, obuf );

		x = (t->lmar + t->border_width); 
		sprintf( obuf, "%d %d moveto %d %d lineto stroke\n", x, -t->topy, x, -cury );
		AFIwrite( ofile, obuf );
	}
}

/*
	Pause the table, restoring columns etc to what they were. Allows column notes to be placed at the
	end of the page when a table spans a page boundary. We assume table row has been the source of 
	this and has cleaned up the last row, so no need to call.
*/
void FMpause_table( void ) {

	struct	table_mgt_blk *t;
	struct	col_blk *next;
	int	i;
	char	obuf[1024];

	if( ts_index <= 0 || ! (t = table_stack[ts_index-1]) )
		return;

	TRACE( 1, "pause_table: cury=%d rows=%d  total_depth=%d  ave_depth=%d\n", cury, t->nrows, t->tot_row_depth, t->ave_row_depth );

	t->paused_list = firstcol;				// hold the current list
	t->paused_col = firstcol;

	lmar = t->lmar;							// restore settings for the column note, or whatever needs regular setup
	hlmar = t->hlmar;
	linelen = t->old_linelen;
	topy = t->old_topy;
	cur_col = t->cur_col;
	firstcol = t->col_list;
}

/*
	Restart a paused table
*/
void FMrestart_table( ) {
	struct	table_mgt_blk *t;

	if( ts_index <= 0 || ! (t = table_stack[ts_index-1]) ) {
		return;
	}
	if( t->paused_list == NULL ) {
		fprintf( stderr, "abort: internal mishap restarting table; no paused list\n" );
		exit( 1 );
	}

	firstcol = t->paused_list;
	cur_col = firstcol;
	lmar = cur_col->lmar - textspace - t->padding;
	firstcol->anchor =  t->paused_list->anchor;			// cary the anchor over for running head/feet
	t->maxy = cury;

	if( t->header )										// new header on resume
	{
		AFIpushtoken( fptr->file, ".tr :" );			/* must drop a table row after the header command(s) */
		AFIpushtoken( fptr->file, t->header );			/* fmtr calls get parm; prevent eating things */
	}

	// cell start emulation
	cury = topy;
	t->topy = topy;
	lmar = cur_col->lmar + t->padding;   	/* set lmar based on difference calculated */
	hlmar = cur_col->lmar + t->padding; 	/* earlier and next columns left edge */

	if( lilist != NULL )					/* if list then reset xpos for next col */
		lilist->xpos = cur_col->lmar + t->padding;

	linelen = cur_col->width;
	t->paused_list = NULL;

	if( t->border )					// add top border to start the table in next col/page
	{
		//sprintf( obuf, "%d %d moveto %d %d rlineto stroke\n", t->lmar+t->padding, -(cury - t->padding), t->border_width-t->padding, 0 );
		sprintf( obuf, "%d %d moveto %d %d rlineto stroke\n", t->lmar+t->padding, -(cury ), t->border_width-t->padding, 0 );
		AFIwrite( ofile, obuf );
	}
}

void FMendtable( void )
{

	struct	table_mgt_blk *t;
	struct	col_blk *next;
	int	i;
	int 	x;
	char	obuf[1024];

	if( ts_index <= 0 || ! (t = table_stack[ts_index-1]) )
		return;

	TRACE( 1, "end_table: cury=%d rows=%d  total_depth=%d  ave_depth=%d\n", cury, t->nrows, t->tot_row_depth, t->ave_row_depth );

	AFIpushtoken( fptr->file, ":" );			/* fmtr calls get parm; prevent eating things */
	FMtr( ts_index > 1 ? 0 : 1 );				/* flush out last row (adds bottom and side borders if needed */

	for( cur_col = firstcol; cur_col; cur_col = next )
	{
		next = cur_col->next;
		free( cur_col );
	}

	cur_col = t->cur_col;
	lmar = t->lmar;
	hlmar = t->hlmar;
	linelen = t->old_linelen;
	firstcol = t->col_list;

#ifdef KEEP
/* setting the temp top is up to the user as the table might be in a left column and the next 
column needs to go to the top of the physical page.  If this is a page wide table, then 
the user must setup multiple columns after .et and set the temp top.  we cannot assume this 
to be the case
*/
	if( rtopy == 0 )			/* allow for multiple columns under the table */
		rtopy = t->old_topy;

	topy = cury;
#endif

	topy = t->old_topy;
	if( t->header )
		free( t->header );
	free( t );
	table_stack[ts_index-1] = NULL;
	if( ts_index > 0 )
		ts_index--;			/* keep it from going neg */
	if( ts_index )
		topy = table_stack[ts_index-1]->topy;
}

