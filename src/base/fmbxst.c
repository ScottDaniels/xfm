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
**************************************************************************
*
*  Mnemonic: FMbxstart
*  Abstract: This box will setup for a box at the current y position.
*            with the left and right margin values set based on the info
*            in the current column block. If the option is TRUE then this
*            routine assumes that the veritcal column numbers are in the
*            input buffer, and will attempt to read them in and set them
*            up (this is so the routine can be called from columeject.
*            Calling routine is responsible for calling FMflush if necessary.
*            This routine must NEVER call FMflush, or call a routine that
*            calls FMflush.
*  Parms:    parms are mostly for hfm, but must be accepted and ignored in 
*		other flavours 
*
*  Returns:  Nothing.
*  Date:     15 October 192
*  Author:   E. Scott Daniels
*  Modified:  1 July 1994 - To convert to rtf
*				17 Jul 2016 - Changes for better prototype generation.
*****************************************************************************
*/
/*void FMbxstart( option )*/
extern void FMbxstart( int option, char *colour, int border, int width, char *align )
{
 int len;          /* length of paramters passed in */
 char *buf;        /* pointer to next option to parse */
 int i;            /* loop index */
 int j;

 if( cury == topy )              /* if this is the first line */
  {
   AFIwrite( ofile, "\\par" );   /* space down some */
   cury += textsize + textspace;
  }

  AFIwrite( ofile, "\\box\\brdrs\\brdrw20\\brsp100" );   /* start box */

 if( option )                             /* if not continuing box */
  while( FMgetparm( &buf ) > OK );        /* strip remainging parms */
}                       /* FMbxstart */
