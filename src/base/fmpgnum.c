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
*   Mnemonic:  FMpgnum
*   Abstract:  This routine sets the page number flag as requested by the user.
*			.pn off
*			.pn on [center] [noline] [fmt=`string`] [starting-pgnum]
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Mod:		29 Aug 2011 -- Added center, format and noline support
*
***************************************************************************
*/
void FMpgnum( )
{
 	char *buf;          /* pointer at the token */
 	int len;            /* length of parameter entered */
	char	 *fmt = NULL;

	flags = flags | PAGE_NUM;				/* default to on */
	flags3 &= ~F3_PGNUM_CENTER;				/* default to off */
	flags3 |= F3_RUNOUT_LINES;				/* default to lines */

	while( (len = FMgetparm( &buf )) > 0 )
	{
		switch( *buf )
		{
			case 'o':
				if( strncmp( "off", buf, 3 ) == 0 )
				{
					flags = flags & (255 - PAGE_NUM);       /* turn off and short circuit out of here */
					return;
				}

				break;

			case 'c':									/* assume center */
					flags3 |= F3_PGNUM_CENTER;	
					break;

			case 'f':
					fmt = strchr( buf, '=' );
					if( fmt )
					{
						if( pgnum_fmt )
							free( pgnum_fmt );
						pgnum_fmt = strdup( fmt + 1 );
					}
					break;

			case 'n':									/* assume noline */
					flags3 &= ~F3_RUNOUT_LINES; 
					break;

			default:
					page = atoi( buf );           		/* set the next page number */
					break;
		}

	}

}
