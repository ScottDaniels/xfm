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

/*
* ---------------------------------------------------------------------------
* This source code is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* If this code is modified and/or redistributed, you must retain this
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
#include "../afileio/afidefs.h"   

#include "fmconst.h"               /* constant definitons */

#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"
#include "hfmproto.h"

/*
* --------------------------------------------------------------------------
*  Mnemonic: fmtable 
*  Abstract: Start a table
*  Date:     15 Feb 1999 - broken out of cmd 
*  Author:   E. Scott Daniels
* 
*  The n option allows uer to turn off (no execute) the automatic
*  creation of the first cell. This allows them to call .tr and .cl 
*  straight off in order to supply special values for that cell and row
*  .ta [b] [n] [a=align] [w=xx] [xi...]
*  omitting w= allows browser to scale table. xx is a percentage.
*
*	Mod:	02 Nov 2006 - ensured that table block is 0d before use.
*			10 Apr 2007 - fixed buffer overrun.
*			21 Jul 2010 - html 4.0 changes
*			18 Jul 2016 - Add consistent, and sometimes modern, prototypes.
* --------------------------------------------------------------------------
*/
extern void FMtable( void )
{
 struct table_mgt_blk *t;
	char style[256];
 char colour[50];            /* spot to build colour string in */
 char align[50];
 char valign[50];
 char pad[50];
 char space[50]; 
 int do_cell = 1;         /* n option turns off */
 int len;
 char *ptr;
 int border = 0;          /* border size */
 char wstr[50];           /* spot to build width stirng in */
 int w;

 space[0] = 0;
 pad[0] = 0;              /* nothing as a default */
 align[0] = 0;
 wstr[0] = 0;
 sprintf( valign, "valign: top;" );   /* default to vert as browser def is center */

 FMflush( );                         /* terminate the line in progress */

 curcell = 1;        /* tableinfo[0] has cell count */
 colour[0] = (char) 0;

 if( ts_index &&  table_stack[ts_index-1] )    /* save cur col for when we bump back */
  table_stack[ts_index-1]->curcell = curcell;

 t = table_stack[ts_index] = malloc( sizeof( struct table_mgt_blk) );
 memset( t, 0, sizeof( struct table_mgt_blk ) );
 tableinfo = table_stack[ts_index++]->cells = malloc( sizeof(int) * (MAX_CELLS+1) ) ;
 memset( tableinfo, 0, sizeof( int ) * MAX_CELLS+1 );
 
 t->fgcolour = t->bgcolour = NULL;
 t->oldfg = textcolour;
 
 for( w = 0; w < MAX_CELLS + 1; w++ )
  tableinfo[w] = 0;
 
 while( (len = FMgetparm( &ptr )) != 0 )
  switch( *ptr )
   {
    case 'a': 
       //sprintf( align, "align=%.13s", ptr + 2 );	
       sprintf( align, "align: %.13s;", ptr + 2 );	
       break;

	case 'B':			// ignore pfm edge border option
		break;

    case 'b':   
       if( *(ptr+1) == '=' )
        border = atoi( ptr + 2 );
       else
        border++;
       break;

	case 'C':			// ignore pfm line colour option
		break;

    case 'c':
       t->bgcolour = strdup( ptr + 2 );
       //sprintf( colour, "bgcolor=%.13s", t->bgcolour );
       sprintf( colour, "background-color: %.13s;", t->bgcolour );
       break;

    case 'n':
       do_cell = 0;    /* dont auto do a cell */
       break;

    case 'p':
       sprintf( pad, "cellpadding=%.13s", ptr + 2 );
       break;

    case 's':
       sprintf( space, "cellspacing=%.13s", ptr +2 );
       break;

    case 't':
       t->fgcolour = strdup( ptr + 2 );
       textcolour = t->fgcolour;
       //flags2 |= F2_SETFONT;
       break;

    case 'v': 
       //sprintf( valign, "valign=%.13s", ptr + 2 );
       sprintf( valign, "vertical-align: %.13s;", ptr + 2 );
       break;

    case 'w':          /* w=xxx percent of width to use for table */
       memcpy( wstr, ptr+2, len - 2 );
       wstr[len-2] = 0;
       w = atoi( wstr );
       //sprintf( wstr, "width=%d%%", w );
       sprintf( wstr, "width: %d%%;", w );
       break;

    default:          /* assume column width definition */
       if( curcell < MAX_CELLS )
        {
         tableinfo[curcell] = FMgetpts( ptr, len );
         curcell++;                                 /* cell counter */
        }
       break;
   }

 	tableinfo[0] = curcell;      /* save number of cells  */
 	curcell = 0;

 //sprintf( obuf, "<TABLE border=%d %s %s %s %s %s %s %s > ", border, wstr, colour, pad, align, valign, space, table_css ? table_css : "" );
 //sprintf( obuf, "<TABLE border=%d style=\"%s color: %s font-size: %dpx font-family: %s\" > ", border, table_css ? table_css : "", textcolour, text_size, curfont );

 	sprintf( obuf, "<TABLE border=%d style=\"%s %s %s %s color: %s font-size: %dpx font-family: %s\" >", border, wstr, align, colour, valign, textcolour, textsize, curfont );
 	AFIwrite( ofile, obuf );

	FMele_stack( ES_PUSH, ET_TABLE );

 	if( do_cell )      		  /* if user has not turned off, start first cell */
	{
		AFIwrite( ofile, "<tr>" );
		FMele_stack( ES_PUSH, ET_ROW );
		FMcell( 0 );
	}

	*obuf = EOS;
       flags2 &= ~F2_SETFONT;
 }


/*
* --------------------------------------------------------------------------
*  Mnemonic: fmth
*  Abstract: save a set of table header commands.  If we are a page oriented 
*		output then we put the header out with each page eject. When 
*		generating HTML we put the header in now and never again.
*  Date:     02 Nov 2006 - converted from hfm
*  Author:   E. Scott Daniels
* 
*  .th string
* --------------------------------------------------------------------------
*/
extern void FMth( void )
{
	struct table_mgt_blk *t;
	char	*buf;
	int	len; 
	int	totlen = 0;
	char	data[4096];

	if( (t = table_stack[ts_index-1]) == NULL )
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

extern void FMendtable( void )
{

	FMflush( );
	FMele_stack( ES_POP, ET_TABLE );		/* end everything */

	if( ts_index )
		ts_index--; 
	if( table_stack[ts_index] )
	{
		if( table_stack[ts_index]->oldfg )
			textcolour = table_stack[ts_index]->oldfg;

		if( table_stack[ts_index]->fgcolour )
			free( table_stack[ts_index]->fgcolour );

		if( table_stack[ts_index]->bgcolour )
			free( table_stack[ts_index]->bgcolour );

		if( table_stack[ts_index]->header )
			free( table_stack[ts_index]->header );

		free( table_stack[ts_index]->cells );
		free( table_stack[ts_index] );
		table_stack[ts_index] = 0;
		if( ts_index )
		{
			tableinfo =  table_stack[ts_index - 1]->cells;
			curcell = table_stack[ts_index - 1]->curcell;
		}
		else
			tableinfo = 0;
	}
//	AFIwrite( ofile, "</td></tr></table>" );    /*end everything */
}
