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
*****************************************************************************
*
*   Mnemonic: FMnofmt
*   Abstract: This routine is called when the noformat flag is set. It
*             reads in lines from the input file and if there is not a
*             command in the first column then it places the line of
*             text as is out to the page buffer. If a command is on the
*             input line control is returned to the caller to process the
*             command.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     8 December 1988
*   Author:   E. Scott Daniels
*
*   Modified:	29 Oct 1992 - To convert for postscript generation
*				09 Mar 1993 - To escape ( ) and \ characters and to insert
*                           blank lines where found in the input text.
*				06 Apr 1994 - To remove dependancy on linelength.
*				10 Apr 2007 - Fixed buffer underflow
*				03 Jul 2016 - Support for tabs.
*				17 Jul 2016 - Bring decls into the modern world.
*******************************************************************************
*/
extern void FMnofmt( void )
{
	int status;            /* status of the read */
	int i;                 /* loop index */

	status = FMread( inbuf );        /* get the next buffer */

	flags3 &= ~F3_CNPEND;			// ensure this is reset before going on

	while( status >= 0  &&  inbuf[0] != CMDSYM && *inbuf != vardelim ) 
	{
		for( i = 0; i < MAX_READ-1 && inbuf[i] != EOS; i++ )
		{
			if( inbuf[i] == '(' || inbuf[i] == ')' || inbuf[i] == '\\' )
			{
				obuf[optr++] = '\\';     /* escape the character first */
				obuf[optr++] = inbuf[i];         /* copy the buffer as is */
			} else {
				if( inbuf[i] == '\t' ) {
					int j;
					for( j = 0; j < 4; j++ ) {
						obuf[optr++] = ' ';
					}
				} else {
					obuf[optr++] = inbuf[i];         /* copy the buffer as is */
				}
			}
		}
	
		if( optr == 0 ) {		// if nil line, we need to give flush a blank
			obuf[optr++] = ' ';
		}
		obuf[optr++] = ' ';    /* flush assumes addtok adds a trail blank; flush will truncate */
		obuf[optr] = EOS;      /* terminate buffer for flush */
		
		FMflush( );            /* send the line on its way */
		TRACE( 1, "nofmt continues after flush flags3=%04x\n", flags3 );

		if( flags3 & F3_CNPEND ) {
   			flags = flags & (255-NOFORMAT);      			// must put back into formatting mode here
			TRACE( 1, "yield becasue cnpend flag was set\n" );
			iptr = optr = 0;              					/* return pointing at beginning */
			return;
		}

		status = FMread( inbuf );   /* get the next line and loop back */
	}           /* end while */

	inbuf[status > 0 ? status : 0] = 0;
	TRACE( 1, "nofmt -- pushing command/var: %s\n", inbuf );
	AFIpushtoken( fptr->file, inbuf );   				/* put command back into input stack */
	iptr = optr = 0;              					/* return pointing at beginning */
}      
