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
*************************************************************************
*
*   Mnemonic: FMheader
*   Abstract: This routine is responsible for putting a paragraph header
*             into the output buffer based on the information in the
*             header options buffer that is passed in.
*   Parms:    hptr - Pointer to header information we will use
*   Returns:  Nothing.
*   Date:     1 December 1988
*   Author:   E. Scott Daniels
*
*   Modified:	30 Jun 1994 - Conversion to rfm (not much changed)
*				15 Dec 1994 - To seperate skip before/after numbers.
*				03 Jul 2016 - To capture the most recent section number 
*					and header string for .gv command.
*				17 Jul 2016 - Changes for better prototype generation.
**************************************************************************
*/

static char* last_snum = NULL;		// last section number generated
static char* last_stxt = NULL;		// last section text generated

extern char* FMget_header_num( ) {
	if( last_snum == NULL ) {
		return NULL;
	}

	return strdup( last_snum );
}

extern char* FMget_header_txt( ) {
	if( last_stxt == NULL ) {
		return NULL;
	}

	return strdup( last_stxt );
}

/*
	Creates the section number string.
*/
extern char* FMmk_header_snum( int level ) {
	char	wbuf[100];
	char	sbuf[10];
	int i;

	if( last_snum ) {
		free( last_snum );
	}

	if( flags & PARA_NUM ) {
		
		wbuf[0] = 0;
		for( i = 0; i < level; i++ ) {
			if( i ) {
				strcat( wbuf, "." );
			}
			snprintf( sbuf, sizeof( sbuf ), "%d", pnum[i] );
			strcat( wbuf, sbuf );
		} 
	} 

	last_snum = strdup( wbuf );

	return strdup( last_snum );
}

/*
	Save a copy of the last header text.
*/
extern void FMmk_header_stxt( char* txt ) {
	if( last_stxt ) {
		free( last_stxt );
	}

	last_stxt = strdup( txt );
}


