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
****************************************************************************
*
* Mnemonic: FMset_colour
* Abstract: process the .co command
* Date:     22 July 2010
* Author:   E. Scott Daniels
*
* Modified:
*
*	.co <textcolour>		# back compatable
*	.co text <string>
*	.co link <string>
*	.co vlink <string>
*	.co bg <string>
*
****************************************************************************
*/
void FMset_colour( )
{
	char	*buf;

	if( FMgetparm( &buf ) )
	{
		if( *buf == '0'  && *(buf+1) == 'x' )
		{
			buf++;
			*buf = '#';								/* allow 0xrrggbb and convert to html friendly #rrggbb */
		}

		if( strcmp( buf, "text" ) == 0 )
		{
			if( FMgetparm( &buf ) )
			{
				if( textcolour )
					free( textcolour );
				textcolour = strdup( buf );
			}
		}
		else
		{
			if( strcmp( buf, "bg" ) == 0 || strncmp( buf, "back", 4 ) == 0  )
			{
				if( FMgetparm( &buf ) )
				{
					if( backcolour )
						free( backcolour );
					backcolour = strdup( buf );
				}
			}
			else
			{
				if( strcmp( buf, "link" ) == 0 )
				{
					if( FMgetparm( &buf ) )
					{
						if( linkcolour )
							free( linkcolour );
						linkcolour = strdup( buf );
					}
				}
				else
				{
					if( strcmp( buf, "vlink" ) == 0 )
					{
						if( FMgetparm( &buf ) )
						{
							if( vlinkcolour )
								free( vlinkcolour );
							vlinkcolour = strdup( buf );
						}
					}
					else			
					{
						if( strcmp( buf, "hlink" ) == 0 )
						{
							if( FMgetparm( &buf ) )
							{
								if( vlinkcolour )
									free( vlinkcolour );
								vlinkcolour = strdup( buf );
							}
						}
						else			/* backwards compat -- just use the string */
						{
							if( textcolour )
								free( textcolour );
							textcolour = strdup( buf );
						}
					}
				}
			}
		}
	}
}

