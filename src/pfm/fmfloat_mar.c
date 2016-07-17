/*
All source and documentation in the xfm tree are published with the following open source license:
Contributions to this source repository are assumed published with the same license. 

=================================================================================================
	(c) Copyright 1995-2016 By E. Scott Daniels. All rights reserved.

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
#include <errno.h>

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
*  Mnemonic:	fmfloat_mar
*  Abstract:	This module contains functions that support the floating margin
*				capaibilities.
*  Date:		01 January 2016
*  Author:		E. Scott Daniels
*  Mods:
*				17 Jul 2016 - Bring decls into the modern world.
*
* .fm [i=distance] [w=distance] l=distance [y=yvalue]
*     l = temp left margin indention from current left margin
*     w = temp column width (-value subtracts from current width)
*	  l = distance before resetting to original values (auto at col end if l > max y)
*	  y = sets the current y position to this value
* .fm pop  .** pop the current float and set y to the reset value
*****************************************************************************
*/
extern void FMfloat_mar( void )
{
	char *ebuf;						// input buffer 
	char *buf;						// pointer to info in input buffer
	int length = 0;					// length of floating margins (default to end of col)
	int	new_lmar = 0;				// temporary left margin value
	int	new_width = 0;				// temporary width value
	int plen;						// parameter token length
	int top_y = 0;					// top y if set woth y=

	FMflush( );						// ensure last bits flushed out

	new_width = linelen;
	new_lmar = lmar;

	while( (plen = FMgetparm( &buf )) > 0 ) {
		switch( buf[0] ) {
			case 'l':
			case 'L':
				length = FMgetpts( &buf[2], plen-2 );		// get distance
				break;

			case 'w':			// line length
			case 'W':
				if( buf[2] == '+' || buf[2] == '-' ) {
					new_width += FMgetpts( &buf[2], plen-2 ); 
				} else {
					new_width = FMgetpts( &buf[2], plen-2 ); 
				}
				break;

			case 'i':			// indention
			case 'I':
				if( buf[2] == '+' || buf[2] == '-' ) {
					new_lmar += FMgetpts( &buf[2], plen-2 ); 
				} else {
					new_lmar = FMgetpts( &buf[2], plen-2 ); 
				}
				break;

			case 'p':
				if( strncmp( buf, "pop", 3 ) == 0 ) {
					if( cur_col->flags & CF_TMP_MAR ) {
						lmar = cur_col->olmar;
						linelen = cur_col->olinelen;
						cur_col->flags &= ~ CF_TMP_MAR;		// reset flag, set cury and boogie out
						cury = cur_col->revert_y;
					}
					return;
				} else {
					FMmsg( E_UNKNOWNPARM, buf );
					FMignore( );
					return;
				}

			case 'y':
			case 'Y':
				top_y = FMgetpts( &buf[2], plen-2 ); 
				if( length == 0 )						// length not set we'll force it
					length = cury - top_y;
				cury = top_y;
				break;

			default:
				FMmsg( E_UNKNOWNPARM, buf );		// let them know they buggered something
				FMignore( );
				return;
		}
	}

	if( cur_col->flags & CF_TMP_MAR ) {				// cannot be nested
		FMmsg( E_PARMOOR, "floating margin is already set" );
		return;
	}

	if( new_width == linelen ) {			// width wasn't adjusted
		if( new_lmar == lmar )
			return;							// nothing changed, just bail silently

		new_width -= new_lmar - lmar;			// shrink width by indention amount
	}

	if( new_width <= 0 ) {
		char mbuf[1024];
		snprintf( mbuf, sizeof( mbuf ), "width is too small: %d", new_width );
		FMmsg( E_PARMOOR, mbuf );
		return;
	}

	TRACE( 1, "floating mar: cury=%d new_lmar=%d new_linelen=%d  revert=%d\n", cury, new_lmar, new_width, cury+length )
	cur_col->olmar = lmar;
	cur_col->olinelen = linelen;
	cur_col->flags |= CF_TMP_MAR;

	lmar = new_lmar;
	linelen = new_width;
	cur_col->revert_y = cury + length;

	return;
}
