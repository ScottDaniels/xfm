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
*  Mnemonic: FMimbed
*  Abstract: This routine is responsible for parsing the imbed string and
*            calling FMopen to open a file to process.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     15 November 1988
*  Author:   E. Scott Daniels
*  Modified: 	25 Aug 2000 - to add NF option
*			13 Nov 207 - Added run/stop command to stream to mark pop of 
*				the fmrun() command and return to this function. Allows
*				this rouitine to drive the imbed which is needed
*				for things like oneject that imbed files and push/pop the 
*				environment before/after the file. Basically negates the 
*				AFIchain() feature where the imbed file was pushed onto 
*				the stack of open files. c'est la vie! 
*			17 Jul 2016 - Changes for better prototype generation.
*			12 Aug 2017 - better trace message, and better error handling.
*
* .im [nf] filename
***************************************************************************
*/
extern void FMimbed(  void )
{
	char *fp = 0;
	char *buf;      /* pointer into the imput buffer of the fname token */
	int len;        /* length of the token */
	char* name;
	int	reset_fmt = 0;					// if nf given, then we reset formatting at end of file

	len = FMgetparm( &buf );
	if( strcmp( buf, "nf" ) == 0 )
	{
		len = FMgetparm( &buf );       /* point to the next token in the buffer */
		FMflush( );                    /* send last formatted line on its way */
		flags = flags | NOFORMAT;      /* turn no format flag on */
		reset_fmt = 1;
	}

	if( len <= 0 )
	{
		FMmsg( E_MISSINGNAME, ".IM" );
		return;
	}

	FMmsg( I_IMBED, buf );

	fp = strdup( buf );

	AFIpushtoken( fptr->file, ".sr" );  	/* push the special runstop command to mark end of imbed file */
	if( FMopen( buf ) == OK ) {

		AFIstat( fptr->file, AFI_NAME, (void **) &name );  	// filename for trace
		TRACE( 2, "imbed: starting with file %s\n", name );
		name = strdup( name );								// must dup as it will be gone if we trace later

		FMrun( );				/* run until we hit the end of the file */

		TRACE( 2, "imbed: finished with file %s lmar=%d cury=%d\n", name, lmar, cury );
		free( name );
	} else {
		TRACE( 2, "imbed: open failed for\n", fp );
		len = FMgetparm( &buf );							// must pop the runstop token as we didn't invoke run
	}

	if( reset_fmt ) {
		flags &= ~NOFORMAT;				// turn  formatting back on
	}
	free( fp );
}                                 /* FMimbed */
