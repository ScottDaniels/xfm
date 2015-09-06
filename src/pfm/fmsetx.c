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
* *** WARNING: With the additon of the fmt blocks, this is likely broken!!!
* 
*  Mnemonic: FMsetx
*  Abstract: This routine is responsible for adjusting the current x position
*            on the output page. The value entered may be specified in pts or
*            inches. A + or - prefix to the value indicates a relitive move,
*            and a value without + or - indicates an absolute position in
*            relation to the current column's left margin value.
*  Params:   None.
*  Returns:  Nothing.
*  Date:     3 December 1992
*  Author:   E. Scott Daniels
*
*  Modified: 10 Dec 1992 - To use AFI routines for ansi compatability
*            25 Mar 1992 - To accept a + for relative moves
*            22 Apr 1993 - To set relmove flag initially to false
*             7 Apr 1994 - To use the getpts routine allowing user to enter
*                          x value in inches or points.
*            11 Apr 1994 - To take value relative to curcol's left margin
******************************************************************************
*/
void FMsetx( )
{
 char *buf;            /* pointer at the parameter user has entered */
 int len;              /* length of parameter entered */
 int i;                /* new x position */
 int relmove = FALSE;  /* new x is relative to current x position */
 char wbuf[50];        /* buffer to build string in to write to file */

 if( (len = FMgetparm( &buf )) > 0 )   /* if user entered a parameter */
  {
   if( buf[0] == '+' )    /* relative move? */
    relmove = TRUE;       /* then set local flag */

   i = FMgetpts( buf, len );

   if( i < cur_col->width )  /* ensure its in the column */
    {
     if( optr == 0 )   /* if nothing currently in the buffer */
      {
       optr = 1;
       obuf[0] = ' ';        /* force flush to setup the current y */
       obuf[1] = EOS;
       osize = 0;            /* reset output size */
      }

     FMflush( );             /* put out what is already there */

     if( relmove == TRUE ) /* if relative move */
      {
       osize += i;             /* indicate number of points skipped over */
       sprintf( wbuf, "%d 0 rmoveto\n", i );        /* ps to relative move */
      }
     else
      {
       osize = i;              /* indicate position in obuf in terms of pts */
       sprintf( wbuf, "%d %d moveto\n", cur_col->lmar + i, -cury );
      }

     AFIwrite( ofile, wbuf );                        /* write to file */
    }                                                /* end if in range */
   else            /* generate an error message */
    FMmsg( E_PARMOOR, ".sx" );
  }                                                  /* end if parm entered */
}                                 /* FMsetx */
