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
*  Mnemonic: FMsetx
*  Abstract: This routine is responsible for adjusting the current x position
*            on the output page. The value entered may be specified in pts or
*            inches. A + or - prefix to the value indicates a relitive move,
*            and a value without + or - indicates an absolute position in
*            relation to the current column's left margin value.
*            NOTE: In RFM the +/- value cannot be supportd. The value will
*                  be treated as an absolute value.
*  Params:   None.
*  Returns:  Nothing.
*  Date:     3 December 1992
*  Author:   E. Scott Daniels
*
*  Modified: 13 Jul 1994 - To convert to rfm.
*				17 Jul 2016 - Changes for better prototype generation.
******************************************************************************
*/
extern void FMsetx(  void )
{
 char *buf;            /* pointer at the parameter user has entered */
 int len;              /* length of parameter entered */
 int i;                /* new x position */
 int relmove = FALSE;  /* new x is relative to current x position */
 char wbuf[50];        /* buffer to build string in to write to file */

 if( (len = FMgetparm( &buf )) > 0 )   /* if user entered a parameter */
  {
   if( buf[0] == '+' )       /* relative move right */
    relmove = TRUE;          /* set the local flag for later */

   i = FMgetpts( buf, len );  /* get user's value in points */

   if( i < cur_col->width )  /* ensure its in the column */
    {
     if( relmove )           /* need to figure exact i to move to */
      {
       i += osize + lmar;    /* calculate the real offset for tab stop */
      }

     FMflush( );             /* put out what is already there */

     if( relmove )           /* need to figure exact i to move to */
      {
       osize = i - lmar;     /* calculate the real space on the line */
      }

     sprintf( wbuf, "\\tx%d \\tab", i * 20 );

     AFIwrite( ofile, wbuf );                        /* write to file */
    }                                                /* end if in range */
   else            /* generate an error message */
    FMmsg( E_PARMOOR, ".sx" );
  }                                                  /* end if parm entered */
}                                 /* FMsetx */
