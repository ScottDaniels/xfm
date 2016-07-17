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
*****************************************************************************
*
*   Mnemonic: FMasis
*   Abstract: This routine is called when the asis flag is set. It
*             assumes that the input in the file is postscript commands
*             and places the text from the input file directly into the
*             output file until the fo coommand is encountered.
*   Parms:    None.
*   Returns:  Nothing.
*   Date:     27 November 1992
*   Author:   E. Scott Daniels
*
*   Modified: 10 Dec 1992 - To use AFI routines for ansi support
*              9 Mar 1993 - To allow .im command to be processed too
*             20 Jul 1994 - To convert to rfm
*              1 Aug 1994 - To write directly from inbuf
*              3 Apr 1997 - To use AFI's new tokenizing support!
*				17 Jul 2016 - Changes for better prototype generation.
*******************************************************************************
*/
extern void FMasis( void  )
{
 int len;               /* length of string to put in outfile */

 while( FMread( inbuf ) >= 0 )
  {
   if( inbuf[0] == CMDSYM )
    {
     if( inbuf[1] == 'f' && inbuf[2] == 'o' )  /* start formatting command */
      {
       iptr = optr = 0;                     /* return pointing at beginning */
       AFIpushtoken( fptr->file, inbuf );          /* push to process */
       return;                                     /* get out now */
      }
     else
      if( inbuf[1] == 'i' && inbuf[2] == 'm' )  /* imbed file? */
       {
        cury += textspace + textsize;              /* assume one par mark */
        AFIpushtoken( fptr->file, inbuf+4 );       /* put file name on stack */
        FMimbed( );                                /* then go process it */
        *inbuf = EOS;                              /* reset buf to write nada*/
       }
    }                                       /* end if cmd symbol encountered */

   if( *inbuf != EOS )
    AFIwrite( ofile, inbuf );                /* write it as it came in */
  }                                         /* end while */

 iptr = optr = 0;              /* return pointing at beginning */
}              /* FMasis */
