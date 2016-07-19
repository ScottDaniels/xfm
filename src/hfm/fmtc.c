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
*   Mnemonic:  FMtc
*   Abstract:  This routine parses the .tc command and opens the toc file
*              if it is not open already. As PSFM processes .h_ commands that
*              cause TOC entries to be generated have PSFM source commands
*              commands placed into the TOC file. When the last user source
*              file is completed the TOC file is read by PSFM and the actual
*              TOC is gerated. Thus PSFM generates PSFM code as is runs that
*              it will also interpret prior to its termination!
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      21 December 1988
*   Author:    E. Scott Daniels
*
*   Modified:  23 Apr 1991 - To open toc file based on buffer not const
*              15 Dec 1992 - To convert to postscript and AFI
*               7 Apr 1994 - To setup for TOC now that linesize/cd are points
*               6 Sep 1994 - Conversion for RFM
*			18 Jul 2016 - Add consistent, and sometimes modern, prototypes.
***************************************************************************
*/
extern void FMtc( void )
{
 char *buf;           /* pointer at the token */
 int len;             /* len of parameter entered */
 int i;               /* loop index */
 char title[255];     /* buffer to use to space over on write */

 len = FMgetparm( &buf );    /* is there a length? */

 if( len <= 0 )     /* no parameter entered */
  flags = flags | TOC;        /* assume that we should turn it on */
 else
  {
   switch( buf[1] )           /* see what the user entered */
    {
     case 'n':
     case 'N':                   /* user wants it on */
      flags = flags | TOC;        /* so do it */
      break;

     default:
      flags = flags & (255-TOC);       /* turn off the flag by default */
      return;
    }             /* end switch */
  }               /* end else */

 if( flags & TOC  &&  tocfile < VALID )      /* need to open and setup file */
  {
   tocfile = AFIopen( tocname, "w" );        /* open file for writing */
   if( tocfile < 0 )
    {
     FMmsg( E_CANTOPEN, tocname );
     return;
    }
                               /* setup initial psfm commands in toc file */
   AFIwrite( tocfile, ".pa\n" );
   AFIwrite( tocfile, ".cd 1 7.5i i=.75i : .st 15 .sf f4\\i\n" );
   AFIwrite( tocfile, ".rh : .rf : .ll 7i .pn off .tc off\n" );
   AFIwrite( tocfile, ".ce Table Of Contents\n" );
   AFIwrite( tocfile, ".sp 2\n" );
  }
}               /* FMtc */
