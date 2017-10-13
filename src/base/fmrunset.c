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
******************************************************************************
*
*  Mnemonic: FMrunset
*  Abstract: This routine is responsible for outputting the necessary commands
*            to get the header, footer and page number going.
*            This routine should be called any time a header footer or page
*            number option is changed. (Unfortunately the page numbering is
*            left to be done by the rtf device and thus the actual number
*            cannot be controlled by rfm.)
*  Parms:    none.
*  Returns:  Nothing.
*  Date:     11 July 1994
*  Author:   E. Scott Daniels
*
*  Modified:	12 Jul 1994 - To allow page number and footer.
*				17 Jul 2016 - Changes for better prototype generation.
*				13 Oct 2017 - Strip deprecated rtf formatting junk.
******************************************************************************
*/
extern void FMrunset( void ) {
// this was completely an rtf funciton and can be deleted!
}


#ifdef KEEP
extern void FMrunset( void )
{
 int right;               /* value for right indent of header */
 char obuf[81];           /* work buffer for output */
 char obuf2[81];          /* second work buffer for output */
 char *fmtstr;            /* format string for sprintf */

 fmtstr = "{\\%s \\pard\\plain %s\\s15\\brdrt\\brdrs\\brdrw20\\brsp20";

 right = 20;

 if( rfoot != NULL || (flags & PAGE_NUM) )   /* something for footer area? */
  {

  /* AFIwrite( ofile, "\\margl0\\margr0\\li0\\fi0\\ri0\\f4\\fs20" ); */
   AFIwrite( ofile, obuf );

   if( rfoot != NULL )             /* if there is a footer string write it */
    {
     AFIwrite( ofile, rfoot );
     AFIwrite( ofile, "\\par" );   /* and terminate with paragraph mark */
    }

   if( flags & PAGE_NUM )    /* if numbering pages - insert the page field */
     AFIwrite( ofile, "Page {\\field{\\*\\fldinst {\\cs17 PAGE}}} \\par" );
   AFIwrite( ofile, "\\par }" );        /* terminate the footer thing */

   if( flags2 & F2_2SIDE )   /* if in two sided mode generate second footer */
    {
     sprintf( obuf2, fmtstr, "footerl", " " );  /* set up with right just */
     AFIwrite( ofile, obuf2 );                     /* and send it out */

     AFIwrite( ofile, obuf );

     if( rfoot != NULL )             /* if there is a footer string write it */
      {
       AFIwrite( ofile, rfoot );
       AFIwrite( ofile, "\\par" );   /* and terminate with paragraph mark */
      }

     if( flags & PAGE_NUM )    /* if numbering pages - insert the page field */
      AFIwrite( ofile, " Page {\\field{\\*\\fldinst {\\cs17 PAGE}}} \\par" );
     AFIwrite( ofile, "\\par }" );      /* terminate the footer thing */
    }                                   /* end if two sided mode */
  }                                     /* end if footer or page number */
 else
  {
   AFIwrite( ofile, "{\\footerr\\pard\\plain\\fs5}" );  /* turn off footers*/
   AFIwrite( ofile, "{\\footerl\\pard\\plain\\fs5}" );  /* if missing */
  }

 if( rhead != NULL )         /* is there a header to write? */
  {
   if( flags2 & F2_2SIDE )   /* if we should flip headers for 2 sided */
    {
     AFIwrite( ofile,
         "{\\headerl\\pard\\plain \\s15\\brdrb\\brdrs\\brdrw20\\brsp20" );
     AFIwrite( ofile, obuf );
     AFIwrite( ofile, rhead );
     AFIwrite( ofile, "\\par\\pard\\plain\\par}" );
    }

   AFIwrite( ofile,
       "{\\headerr\\pard\\plain \\qr\\s15\\brdrb\\brdrs\\brdrw20\\brsp20" );
  /* AFIwrite( ofile, "\\li0\\fi0\\ri-200\\f4\\fs20" ); */
   AFIwrite( ofile, obuf );
   AFIwrite( ofile, rhead );
   AFIwrite( ofile, "\\par\\pard\\plain\\par}" );
  }
 else                                /* no header just turn off */
  {
   AFIwrite( ofile, "{\\headerr\\pard\\plain\\fs5}" );  /* turn off */
   AFIwrite( ofile, "{\\headerl\\pard\\plain\\fs5}" );  /* turn off */
  }
}                /* FMrunset */
#endif
