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

                       /* get include info supplied by compiler */
#include <stdio.h>     /* pickup null etc */
#include <fcntl.h>     /* pickup file control stuff */
#include <stat.h>      /* pickup defines for open etc */
#include <ctype.h>     /* get isxxx type headers */

#include "fmconst.h"   /* constants */
#include "fmstru2.h"   /* external vars and structure definitions */

#include "fmtopmar.c"  /* adjust topy value  12-3-88 */
#include "fmtc.c"      /* turn on or off the toc  12-21-88 */
#include "fmtoc.c"     /* process a toc entry  12-21-88 */
#include "fmjustif.c"  /* justify a line of text  11/12/92 */
#include "fmcenter.c"  /* center text in output 11-2-92 */
#include "fmrunout.c"  /* print running material 5-4-91 */
#include "fmnofmt.c"   /* no format processing 12-8-88 */
#include "fmsetstr.c"  /* set a string with input parameter info*/
#include "fmtmpy.c"    /* adjust topy/bot y values on temporary basis */
#include "fmgetpts.c"  /* convert token value to real points */
#include "fmep.c"      /* embedded postscript command processing */
#include "fmll.c"      /* set up line lenght change */
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

/* psfm only routines */
/*
******************************************************************************
*
*  Mnemonic: psfmonly.c
*  Abstract: This file compiles the PSFM routines that are used only by
*            psfm into one object file.
*
******************************************************************************
*/

#include <stdio.h>     /* pickup null etc */
#include <fcntl.h>     /* pickup file control stuff */
#include <stat.h>      /* pickup defines for open etc */
#include <ctype.h>     /* get isxxx type headers */

#include "fmconst.h"
#include "fmstruct.h"  /* structure and global definitions */
#include "fmcmds.h"    /* command definitions (this comp file only) */

#include "fmcd.c"      /* setup multiple/single column(s) */
#include "fmline.c"    /* draw a line in output */
#include "fmtoksiz.c"  /* calc size in points of a token */
#include "fmflush.c"   /* flush the build buffer to the output file */
#include "fmpflush.c"  /* cause headers/footers to be written and showpage */
#include "fminit.c"    /* initialize the stystem */
#include "fmspace.c"   /* add blank lines to the document */
#include "fmcmd.c"     /* cmd driver- cmd1 and cmd2 MUST follow */
#include "fmcmd1.c"    /*** must follow fmcmd.c do not move */
#include "fmcmd2.c"    /*** must follow fmcmd1.c do not move */
#include "fmheader.c"  /* paragraph header 12-1-88 */
#include "fmsetx.c"    /* adjust the current position on the line */
#include "fmbeglst.c"  /* begin a regular list  4-30-89 */
#include "fmendlis.c"  /* end/show a list */
#include "fmbox.c"     /* process a box command */
#include "fmbxst.c"    /* start a box  */
#include "fmbxend.c"   /* end a box  */
#include "fmsetfon.c"  /* set font in output file 5-11-92 */
#include "fmceject.c"  /* eject the current column 5-6-92 */
#include "fmaddtok.c"  /* add a tokin to the output buffer */
#include "fmditem.c"   /* process a definition item  1-1-89 */
#include "fmmain.c"    /* entry point and main driver for formatter */


/* tfm common */
/* Compliation file for the tfm routines that have remianed unchanged */
                       /* get include info supplied by compiler */
#include <stdio.h>     /* pickup null etc */
#include <fcntl.h>     /* pickup file control stuff */
#include <stat.h>      /* pickup defines for open etc */
#include <ctype.h>     /* get isxxx type headers */

#include "FMCONST.H"                /* constant definitons */
#include "FMSTR2.H"                 /* external definitions */

#include "..\tfm\fmif.c"            /* handle if processing */
#include "..\tfm\fmhn.c"            /* turn on/off header numbers 12-2-88 */
#include "..\tfm\fmpgnum.c"         /* page number toggle 12-3-88 */
#include "..\tfm\fmsetjus.c"        /* set the just flag  12-10-88 */
#include "..\tfm\fmvartok.c"        /* get a token from variable expan 7-7-8*/
#include "..\tfm\fmdv.c"            /* handle defination of a variable 7-7-89 */
#include "..\tfm\FMREAD.C"          /* read from current input file */
#include "..\tfm\FMCLOSE.C"         /* close the input file chain */
#include "..\tfm\FMOPEN.C"          /* Open a file and add to chain */
#include "..\tfm\FMIMBED.c"         /* imbed a file */
#include "..\tfm\fmGETvar.C"        /* get a variable definition pointer */
#include "..\tfm\FMMSG.C"           /* generate a message to the console */
