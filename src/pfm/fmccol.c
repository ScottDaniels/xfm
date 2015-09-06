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
****************************************************************************
*
*   Mnemonic:  FMccol
*   Abstract:  This routine conditially ejects the current col based on
*              the number of lines remaining in the column and the parameter
*              that the user enteres.
*   Parms:     skip - Number of lines that must be present or skip to next
*                     col is done. If 0 then assumed that cc command entered
*                     and skip value should be read from input buffer.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*   Modified:  23 Nov 1992 - To support postscript conversion
*                            To accept a parameter - if not 0 dont read parm
*                            use parameter instead.
*				25 Jun 2013 - added FMcol_enough()
*				24 Jul 2013 - Takes into account any end notes now.
*				05 Mar 2014 - Now takes into consideration end note space
***************************************************************************
*/

/* 
	return true if there is enough space in the column for the 
	number of lines given. 
*/
extern int FMcol_enough( int lines )
{
	if( ((lines * (textsize + textspace)) + cury) >= (boty - cn_space) )
		return 0;

	return 1;
}


extern void FMccol( int skip )
{
	char *buf = NULL;          /* pointer at the token */
	int len;

	FMflush( );         /* put out what may be there */

	if( skip == 0 )          /* calling routine did not pass parameter to use */
	{
		if( (len = FMgetparm( &buf )) )       /* then assume user command and get it */
			skip = FMgetpts( buf, len );               /* convert to integer */
		else
			skip = 2 * (textsize + textspace); 	/* otherwise default to 2 lines */
	}

	TRACE( 2, "ccol: buf=%s skip=%d cnspace=%d size+space=%d cury=%d boty=%d   eject=%d(%s)\n", buf ? buf : "", skip, cn_space, textsize+textspace, cury, boty, skip + cn_space + cury, skip + cn_space + cury >= boty ? "true" : "false" );

	if( skip + cn_space + cury >= boty )
		PFMceject( );                        /* eject col if not enough there */
}               /* FMccol */
