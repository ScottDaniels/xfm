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
***************************************************************************
*
*    Mnemonic: FMtoc
*    Abstract: This routine is responsible for putting the contents of the
*              output buffer into the table of contents file.
*              When this routine is called the header to be placed in the
*              output file must be in the obuf (output buffer).
*    Parms:    level - Paragraph level
*    Returns:  Nothging.
*    Date:     21 December 1988
*    Author:   E. Scott Daniels
*
*    Modified: 3 Feb 1989 - To not place page #s in toc if not numbering
*             15 Dec 1992 - To convert for postscript and AFI
*             11 Apr 1993 - To insert a newline on the last sprintf
*              6 Sep 1994 - Conversion for RFM
*				17 Jul 2016 - Changes for better prototype generation.
*				17 Mar 2018 - Fix printf format warnings
****************************************************************************
*/
extern void FMtoc( int level )
{
	char buf[MAX_READ];      /* buffer to build toc entry in */
	char fbuf[128];
	int i;                   /* pointer into buffer */
	int j;

 if( tocfile == ERROR )    /* if no toc file open then do nothing */
  return;

 if( (flags & TOC) == 0 )        /* if user has them turned off */
  return;                        /* then dont do anything */

 buf[0] = '`';                       /* quote lead blanks */
 for( i = 1; i < (level-1)*2; i++ )
  buf[i] = BLANK;                /* indent the header in toc */

 for( j = 0; j < optr && i < 70; j++, i++ )   /* copy into toc buffer */
  buf[i] = obuf[j];
 buf[i++] = '`';             /* terminate the quote */
 buf[i] = EOS;              /* terminate the string */

 if( level == 1 )               /* skip an extra space for first level */
  AFIwrite( tocfile, ".sp 1 .sf f4\\b .st 12\n" );     /* skip a line */
 else
  AFIwrite( tocfile, ".sf f4 .st 10\n" );  /* normal text */

 AFIwrite( tocfile, buf );  /* and output it to the toc file */

 if( flags & PAGE_NUM )       /* if numbering the pages then place number */
  {
	if( toc_pn_fmt != NULL ) {
		snprintf( fbuf, sizeof( fbuf ), ".sx 7i %s", toc_pn_fmt ); 
		sprintf( buf, fbuf, page+1 );
	} else {
		sprintf( buf, ".sx 7i %d .br\n", page+1 );
	}
   AFIwrite( tocfile, buf );            /* write the entry to the toc file */
  }                             /* end if page numbering */
}                   /* FMtoc */

