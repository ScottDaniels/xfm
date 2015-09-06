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

#include "../base/symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "xfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
**************************************************************************
*
*  TFM
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
*  Parms:    option - TRUE if block structure is completely initialized
*                     (ie a new box not continuation to next column)
*                     NOTE: The parameter is not used, but kept as a place
*                            holder as unmodified PSFM routines will call
*                            this routine
*  Returns:  Nothing.
*  Date:     15 October 192
*  Author:   E. Scott Daniels
*  Modified:  1 Jul 1994 - To convert to rtf
*            29 Apr 1997 - To make a one celled table for the box
*            26 Mar 2000 - To add border and width parms
*            23 Mar 2001 - To make inert for TFM; here we go again!
*****************************************************************************
*/
void FMbxstart( int option, char *colour, int border, int width, char *align )
{
 FMignore( );    /* ignore all parameters for the command */
}                       /* FMbxstart */
