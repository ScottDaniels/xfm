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

/* DEPRICATED - with the implementation of the symbol table to map and 
   fetch variables, this routine is no longer necessary!
*/

errors should happend if compiled accidently
/*
****************************************************************************
*
*   Mnemonic: FMgetvar
*   Abstract: This routine is responsible for finding the name passed in
*             in the list of defined variables. If the name is not found
*             in the list an error message is generated.
*   Parms:    name - Pointer to the string to find.
*             len  - Length of the name string (not guarenteed to be EOSed)
*   Returns:  Returns a pointer to the buffer string that the variable 
*             expands to.
*   Date:     7 July 1989
*   Author:   E. Scott Daniels
*
*   Modified: 9-01-89 - To support user defined variable delimiter.
*            12-15-92 - To correct name for compatability with case sensitive
*                       linkers.
*             7-28-94 - To allow punctuation appended to var name
*             8-24-94 - To return pointer to found blk for redefining vars.
*            10-13-94 - To use the utformat routine to expand the var thus
*                       allowing for parameterized variables:
*                         .dv h1 .rh $1 ^: .h1 $1
*                         &h1(Header string also running header)
*                         generates: .rh Header string also running header
*                                    .h1 Header string also running header
*             2-06-95 - To find last ) in a paramaterized variable
*             4-03-97 - To use the new AFI tokenizer!
*****************************************************************************
*/
char *FMgetvar( name, len )       /* return pointer to expansion string */
 char *name;
 int len;
{
 char tbuf[80];              /* temp buffer for compare */
 struct var_blk *vptr;       /* pointer into the list of variable blocks */

 if( *name == vardelim )
  {
   name++;                  /* skip over the variable symbol */
   len--;
  }

 len = len > MAX_VARNAME ? MAX_VARNAME : len;  /* dont go max name len*/

 strncpy( tbuf, name, len );   /* get the name to look for */
 tbuf[len] = EOS;              /* make it a real string */

 for( vptr = varlist; vptr != NULL && strcmp( vptr->name, tbuf ) != 0;
      vptr = vptr->next );

 if( vptr != NULL )            /* found one */
  return( vptr->string );
 else
  return( NULL );

}         /* FMgetvar */
