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
  All of the hfm routines are compiled into one object...some day I will
  rewrite this to generate a library (Ive been saying that for a LONG
  while now!)

  CAREFUL: some of these are included from ../rfm!
*/
                       /* get include info supplied by compiler */
#define _TFM_ 1
#include <stdio.h>     /* pickup null etc */
#include <stdlib.h>
#include <fcntl.h>     /* pickup file control stuff */
#include <ctype.h>     /* get isxxx type headers */
#include <string.h>    /* def for strtok et al. */
/*#include <malloc.h> */   /* absolutely required for MSC large */
#include <time.h>
#include <memory.h>

#include "../tools/../base/symtab.h"
#include "../afileio/afidefs.h"   /* alternate file I/O routines */

#include "fmconst.h"               /* constant definitons */

#define RF_PAR     0x01            /* rfm only flags */
#define RF_RUNNCHG 0x02            /* running item has changed */
#define RF_SBREAK  0x04            /* section break has occurred sinc flush */

#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "hglobals.h"              /* globals specific to hfm */
#include "fmproto.h"

                                   /* the first set must be included first */
#include "fmshowvars.c"              /* get a variable definition pointer */
#include "fmskip.c"

#include "fmdv.c"            /* handle defination of a variable 7-7-89 */
#include "fmif.c"            /* handle if processing */
#include "fmelse.c"          /* process .ei command encountered */
#include "fmhn.c"            /* turn on/off header numbers 12-2-88 */
#include "fmpgnum.c"         /* page number toggle 12-3-88 */
#include "fmsetjus.c"        /* set the just flag  12-10-88 */
#include "fmvartok.c"        /* get a token from variable expan 7-7-8*/
#include "fmread.c"          /* read from current input file */
#include "fmclose.c"         /* close the input file chain */
#include "fmopen.c"          /* Open a file and add to chain */
#include "fmimbed.c"         /* imbed a file */
#include "fmmsg.c"          /* generate a message to the console */


#include "fmignore.c"  /* special routine to ignore command parms */

#include "fmcell.c"    /* next table cell */
#include "fmtr.c"      /* next table row */
#include "fmtable.c"   /* start table */
#include "fmjump.c"     /* jump to a label in the input stream */
#include "fmtopmar.c"  /* adjust topy value  12-3-88 */
#include "fmjustif.c"  /* justify a line of text  11/12/92 */
#include "fmcenter.c"  /* center text in output 11-2-92 */
#include "fmrunout.c"  /* print running material 5-4-91 */
#include "fmsetstr.c"  /* set a string with input parameter info*/
#include "fmtmpy.c"    /* adjust topy/bot y values on temporary basis */
#include "fmgetpts.c"  /* convert token value to real points */
#include "fmep.c"      /* embedded postscript command processing */
#include "fmgetval.c"  /* get a value and put into a variable */
#include "fmsety.c"    /* set cury to given position */
#include "fmdefhea.c"  /* adjust header definiton information */
#include "fmcpage.c"   /* conditional page eject 12-3-88 */
#include "fmformat.c"  /* handle format command 12-8-88 */
#include "fmasis.c"    /* begin processing PostScript source */
#include "fmfigure.c"  /* generate figure ending  12-9-88 */
#include "fmbd.c"      /* begin a definition list 1-1-89 */
#include "fmccol.c"    /* condtional col eject  12-3-88 */
#include "fmindent.c"  /* indent a margin */
#include "fmevalex.c"  /* evaluate an expression in a parm list */
#include "fmgetpar.c"  /* get next parameter frominput buffer */
#include "fmgettok.c"  /* get next token from input buffer */
#include "fmll.c"      /* set the linelength based on .ll command */



int rflags = RF_SBREAK;            /* global rfm only flags */
int col_gutter = 0;                /* current column gutter */



/* ------------------- rfm only definitions ------------------------*/
#include "fmtc.c"       /* process tc command */
#include "fmtoc.c"      /* put an entry into toc file */
#include "fmright.c"    /* right justify a string */

#include "fmdo.c"       /* loop command */
#include "fmsectio.c"   /* add a non distructive section break */
#include "fmtoksiz.c"   /* calc size in points of a token */
#include "fmsetcol.c"   /* setup column information in output file */
#include "fmceject.c"   /* eject to anew column */
#include "fmsetx.c"     /* "tab" to an x position on the current line */
#include "fmrunset.c"   /* set the running string */
#include "fmaddtok.c"   /* add a token to the output buffer */
#include "fmnofmt.c"    /* no format of text routine */
#include "fmline.c"     /* draw a line in the text */
#include "fmsetmar.c"   /* set the margins based on col width */
#include "fmlisti.c"    /* setup for a list item */

#ifdef NOT_UNIX
#include "fmcmd.c"      /* command routine start */
#include "fmcmd1.c"     /* must follow fmcmd */
#include "fmcmd2.c"     /* must follow cmd1 */
#else
#include "fmcmd_a.c"   /* all command parsing slammed into one file */
#endif

#include "fmendlis.c"   /* terminate listitem list */
#include "fmbeglst.c"   /* begin a listitem list */
#include "fmheader.c"   /* generate a header */
#include "fmbxst.c"
#include "fmbox.c"      /* box support routines */
#include "fmbxend.c"
#include "fmpara.c"    /* setup for new paragraph */
#include "fmpflush.c"  /* flush the page */
#include "fmflush.c"   /* flush the buffer */
#include "fminit.c"    /* initalize the system */
#include "fmspace.c"   /* add blank space tothe doc */
#include "fmsetfon.c"  /* set the curent text font and text size */
#include "fmditem.c"   /* add a definition item to the document */
#include "fmmain.c"    /* main driver */
#include "fmcd.c"      /* change column definitions */
