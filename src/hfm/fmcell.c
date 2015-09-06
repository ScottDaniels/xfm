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


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
* --------------------------------------------------------------------------
*  Mnemonic: fmcell
*  Abstract: Start  the next cell
*  Date:     22 Feb 1999 - broken out of cmd 
*  Author:   E. Scott Daniels
* 
*  .cl [c=bgcolour] [t=fgcolour] [s=span-cols] [a=align-type]
* --------------------------------------------------------------------------
*/

void FMcell( int parms )
{
	char buf[256];
	char buf2[256];
	char colour[50];        /* buffers to build various style or parms in */
	char span[50];          
	char rspan[50];        
	char align[50];       
	char valign[50];     
	char	width[50];
	int len;
	char *ptr;


	span[0] = 0;
	align[0] = 0;
	buf[0] = 0;
	buf2[0] = 0;
	rspan[0] = 0;
	valign[0] = 0;
	width[0] = 0;
	sprintf( valign, "valign=top;" );		/* this is ok in html 4.0 */

	if( ! table_stack || ! ts_index )
	{
		while( parms && (len = FMgetparm( &ptr )) != 0 );
		return;
	}

	if( table_stack[ts_index-1]->bgcolour  )
		sprintf( colour, "bgcolor=%s", table_stack[ts_index-1]->bgcolour );
	else
		colour[0] = 0;

	colour[0] = 0;
	FMflush( );                       	/* flush what was in the buffer first */
	FMele_stack( ES_POP, ET_COL );		/* pop the last column marker and other lower priority elements that are below */

	if( ++curcell > MAX_CELLS )
		curcell = 1;

	while( parms && (len = FMgetparm( &ptr )) != 0 )
	{
		switch( *ptr )
		{
			case 'a':
				snprintf( align, sizeof( align ), "align=%s", ptr + 2 );
				break;
	
			case 'c':   
				//sprintf( colour, "bgcolor=%s", ptr + 2 );
				snprintf( colour, sizeof( colour ),  "background-color: %s;", ptr+2 );
				break;
			
			case 'r':
				snprintf( rspan, sizeof( rspan ), "rowspan=%s", ptr + 2 );
				break;
	
			case 's':
				snprintf( span, sizeof( span ), "colspan=%s", ptr + 2 );
				break;
	
			case 't':      /* really does not make sense to support this */
				break;
	
			case 'v':
				snprintf( valign, sizeof( valign ), "valign=%s", ptr + 2 );	/* this is ok in html 4.0 */
				break;
	
			default: 
				break;
		}
	}

	if( tableinfo &&  tableinfo[curcell] )
		sprintf( width, "width: %d%%;", (tableinfo[curcell]*100)/PG_WIDTH_PTS );
		//sprintf( buf2, " width=%d%%>", (tableinfo[curcell]*100)/PG_WIDTH_PTS );

	//sprintf( buf, "<td %s %s %s %s %s", colour, span, valign, align, rspan );
	sprintf( buf, "<td %s %s %s %s style=\"%s; color: %s; %s font-size: %dpx; font-family: %s;\">", align, valign, span, rspan, width, textcolour, colour, textsize, curfont );
	FMele_stack( ES_PUSH, ET_COL );

	strcat( buf, buf2 );

	AFIwrite( ofile, buf );
	flags2 &= ~F2_SETFONT;
}
