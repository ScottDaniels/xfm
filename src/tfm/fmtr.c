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
#include "tfmproto.h"

/*
---------------------------------------------------------------------------
 Mnemonic: fmtr
 Abstract: start a new row in the current table
 The n option prevents cell from being called.
 syntax:   .tr [n] [c=bgcolour] [a=alignval] [v=valignvalue]
---------------------------------------------------------------------------
*/
extern void FMtr( void )
 {
   char *ptr;             /* pointer at parms */
   int len;
   int do_cell = 1;       /* turned off by n option */
   char colour[50];
   char align[50];
   char valign[50];
   char obuf[256];

   colour[0] = 0;
   align[0] = 0;
   valign[0] = 0;

   FMflush( );                               /* empty the cache */
   curcell = 0;

   while( (len = FMgetparm( &ptr )) != 0 )
    {
     switch( *ptr ) 
      {
       case 'a':
         sprintf( align, "align=%s", ptr + 2 );
         break;

       case 'c':
         sprintf( colour, "bgcolor=%s", ptr + 2 );
         break;

       case 'n':
         do_cell = 0;
         break;

       case 'v':
         sprintf( valign, "valign=%s", ptr + 2 );
         break;


       default: 
         break;
      }
    }

   sprintf( obuf, "</td></tr><tr %s %s %s>", colour, align, valign );
   AFIwrite( ofile, obuf );

   if( do_cell )
    FMcell( 0 );                               /* start 1st cell */
   flags2 |= F2_SETFONT;           /* need to change font on next write */
 }
