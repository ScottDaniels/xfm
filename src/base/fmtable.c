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
***************************************************************************
*
*  Mnemonic: FMtable
*  Abstract: This routine is called when a table command is encountered.
*            It gets the column sizes from the parameter list and 
*            initialized the table array.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     18 April 1997
*  Author:   E. Scott Daniels
*  Modified:	15 Feb 1999 - to support w=xx hfm table width
*				17 Jul 2016 - Changes for better prototype generation.
*
*  Command syntax:  .ta [b] [w=xx] <width1> <width2> ... <widthn>
*
*****************************************************************************
*/

extern void FMtable(  void )
{
 char *buf;        /* pointer at next input parameter */
 int len;          /* length of token */
 int i = 1;        /* loop index */

 table_stack[ts_index] = malloc( sizeof( struct table_mgt_blk) );
 tableinfo = table_stack[ts_index++]->cells = malloc( (sizeof(int) * MAX_CELLS)+1) ;
 *tableinfo = 0;  /* fake out next bit of original code before nested tables */
 
 if( *tableinfo == 0 )   /* ensure table not already started */
  {
   flags2 &= ~F2_BORDERS;     /* default to no borders */

   while( (len = FMgetparm( &buf) ) != 0  )
    switch( *buf )
     {
      case 'a':          /* hfm options we simply must ignore */
      case 'c':
      case 'n':
      case 'v':
      case 'p':
      case 's':
          break;

      case 'b':
          flags2 |= F2_BORDERS;   /* turn on borders */
          break;

      case 'w':          /* table width parm (hfm only for now */
          break;

      default:
          if( i < MAX_CELLS )
           {
            tableinfo[i] = FMgetpts( buf, len );
            i++;                                 /* cell counter */
           }
          break;
     }
  
   tableinfo[0] = i;         /* save number of cells in each row */
  
   AFIwrite( ofile, "{" );   /* enclose the table in braces */
   FMrow( FIRST );           /* start first row of table */
  }
}                /* fmtable */
