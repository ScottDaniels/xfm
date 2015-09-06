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
*  Mnemonic: FMtmpy
*  Abstract: This routine is called to process the .tt and .tb commands which
*            set the topy/boty values to the current y position allowing
*            some flexability in including multi column artwork or sidebars.
*            if the real value (rtopy rboty) is 0 then the current value
*            is saved and the current y position is made the temp max value.
*            If the real value is not 0 then the top/boty values are restored.
*            A parameter may be supplied indicating when the value is to be
*            restored (after n column ejects) and if omited defaults to at the
*            next page eject.
*  Parms:    cmd - Command that caused this invocation.
*  Returns:  Nothing.
*  Date:     7 April 1994
*  Author:   E. Scott Daniels
*
*****************************************************************************
*/
void FMtmpy( cmd )
 int cmd;
{
 char *buf;         /* pointer to the parameter */

 switch( cmd )
  {
   case C_TMPTOP:   /* set temp top y value */
    rtopcount = -1;          /* force reset at top of page */
    if( rtopy == 0 )         /* if tmp not already set */
     {
      rtopy = topy;            /* save current topy in real topy */
      topy = cury;             /* set topy y here */
      if( FMgetparm( &buf ) > 0 )   /* parameter entered */
       rtopcount = atoi( buf );    /* convert to value and save */
     }
    else
     {
      topy = rtopy;          /* reset topy */
      rtopy = 0;             /* and mark as not set */
     }
    break;

   default:
     break;
  }
}                    /* FMtmpy */

