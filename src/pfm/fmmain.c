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

#define OWNS_GLOBALS 

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
*   Mnemonic:  FMmain
*   Abstract:  This routine is the driver for the FM formatter
*   Parms:     argc - Number of parameters passed to the routine
*              argv - Argument vectors
*                  [1] - input file name
*                  [2] - Output file name
*   Returns:   Nothing.
*   Date:      15 November 1988
*   Author:    E. Scott Daniels
*   Modified:   5 May 1992 - To support conversion to postscript
*              29 Oct 1992 - To call command only if .xx is followed by a
*                            blank so that token starting with a . can be
*                            included in the text (as long as they are more
*                            than 2 characters long.)
*              27 Nov 1992 - To support as is postscript commands
*              10 Dec 1992 - To use AFI routines for ansi compatability
*              15 Dec 1992 - To output a newline to stdout at end of run
*              21 Dec 1992 - To process the .toc file generated
*              13 Apr 1992 - To no longer break when blank as first char
*				18 Nov 2001 - To rewrite to allow for immediate ex of .im 
*				18 Dec 2015 - Call pflush at end only if cury not at top.
**************************************************************************
*/
main( int argc, char **argv ) {
 int len;          /* length of token */
 char *buf;        /* buffer pointer to token */

 if( FMinit( argc, argv ) < VALID )
	  return 1;

	FMrun( );							/* run the open file */

	if( tocfile >= OK )		/* if the toc file is still open */
	{				/* then close it, and simulate .im command on it */
		len = sprintf( inbuf, ".et\n" );
		AFIwrite( tocfile, inbuf );
		flags &= ~PAGE_NUM;	/* turn off page numbering */
		rhead = rfoot = NULL;	/* no headers or footer either */

		AFIclose( tocfile );	/* close the toc file */
#ifdef KEEP
		len = sprintf( inbuf, ".im %s", tocname );  /* create imbed command */
		FMopen( inbuf );
		FMrun( );
#endif
	}                      /* end if toc file open */

	FMflush( );            		 /* flush the current line to page buffer */
	if( cury != topy ) {
		TRACE( 3, "main flushing: %d %d\n", cury, topy );
		FMpflush( );			// flush the page out of the printer with headers
	}

	AFIclose( ofile );      /* close the output file */
	sprintf( obuf, "%ld\n", words );

	FMmsg( I_WORDCNT, obuf );     /* write number of words message */
	return 0;                     /* assume end of file reached */
}
