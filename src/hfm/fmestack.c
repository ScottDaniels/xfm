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
/*
* ---------------------------------------------------------------------------
* This source code is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* If this code is modified and/or redistributed, you must retain this
* header, as well as any other 'flower-box' headers, that are
* contained in the code in order to give credit where credit is due.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* Please use this URL to review the GNU General Public License:
* http://www.gnu.org/licenses/gpl.txt
* ---------------------------------------------------------------------------
*/

#include <string.h> 
#include <memory.h>
#include <time.h>

#include "symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
* --------------------------------------------------------------------------
*  Mnemonic: fmestack 
*  Abstract: Manage the element stack. Callled to push/pop an element from the 
*		current stack.  When an element is popped, all types up to the
*		next element that matches the desired type, or has a lower value
*		type. For example:
*		if the stack is  "div col row table div doc body" and a pop div is 
*		is invoked, the top div is removed and the close tag written to 
*		the output file. If a pop table call is made, the div, col and 
*		row elements would also be closed.  If the stack were: 
*		"col row table div doc body" and a pop div is called, nothing
*		happens because the top of stack has a lower type than div. 
*
*  Date:     25 July 2010
*  Author:   E. Scott Daniels
* 
*  Mod:	
* --------------------------------------------------------------------------
*/

static int elements[2048];
static int ele_idx = 0;

char *closers[] = {	/* must match defs */
	"</html>",
	"</body>",
	"</table>",
	"</tr>",
	"</td>",
	"</div>",
	"</span>"
};

extern void FMele_stack( int action, int type )
{
	switch( action )
	{
		case ES_PUSH:
			TRACE( 2, "ele_stack: push [%d] %s\n", ele_idx, closers[type] );
			elements[ele_idx++] = type;
			break;

		case ES_POP:
			while( --ele_idx > 0  && elements[ele_idx] > type )
			{
				TRACE( 2, "ele_stack: pop [%d] %s\n", ele_idx, closers[elements[ele_idx]] );
				AFIwrite( ofile, closers[elements[ele_idx]] );
			}

			if( elements[ele_idx] == type )			/* this is it; pop it too */
			{
				TRACE( 2,  "ele_stack: pop [%d] %s\n", ele_idx, closers[elements[ele_idx]] );
				AFIwrite( ofile, closers[type] );
			}
			else
				ele_idx++;				/* a blocking type, keep it */
			break;
	}
}
