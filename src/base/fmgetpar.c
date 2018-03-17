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
*  Mnemonic: FMgetparm
*  Abstract: This routine gets a parameter from the current buffer.
*            If the end of the input buffer is reached before a
*            command parameter is located then a length of 0 is
*            returned. Unlike FMgettok this routine does not continue
*            on to the next input line looking for the parameter.
*            When a single colon (:) is encountered get parm interprets
*            it as the end of the parameter list and returns 0 as if the
*            end of the buffer was reached. This allows commands to be
*            "stacked" on the same line without having to supply all optional
*            parameters.
*            NOTE: This routine is recursive when a variable is encountered
*                  in the parameter list.
*  Parms:    buf - Pointer to a character pointer to return parm pointer in
*  Returns:  Length of the parameter, or 0 if no parameter encountered.
*  Date:     16 November 1988
*  Author:   E. Scott Daniels
*
*  Modified: 7-07-89 - To support variable expansion.
*            9-01-89 - To support user defined variable delimiter.
*            3-29-93 - To support expressions as variables in commands
*            4-26-93 - To allow starting [ to be butted with first token of
*                      the expression.
*            7 Apr 94- To allow : to stop processing.
*            3 Apr 97- To use new AFI tokenizer!
*            8 Apr 97- To support `xxx xxx` tokens in a parm list 
*			19 Oct 06- Addded parm string to TRACE.
*			22 Nov 2006 - corrected escape of [
*			20 Oct 2007 - corrected handling of backquotes
*			17 Jul 2016 - Changes for better prototype generation.
*			17 Mar 2018 - Fix printf warnings
****************************************************************************
*/
extern int FMgetparm( char **buf )
{
	int i;                  /* loop index */
	int j;
	int pstart;             /* starting offset of parameter in the buffer */
	int len = 0;            /* len of token from input stream; length of buffer we are sending via inbuf */
	char	*cp;
	char exbuf[2048];         /* expression return buffer */
	
	*buf = inbuf;                             /* next parm placed in input buf */
	
	len = AFIgettoken( fptr->file, inbuf );   /* get next token from stream */
	 
	
	if( len > 0 )                             /* if not at the end of the line */
	{
		inbuf[len] = EOS;                       /* ensure termination */

		TRACE( 3,  "getparm: len=%d (%s)\n", len, inbuf );

		switch( *inbuf )     
		{
			case CONTINUE_SYM:                 /* continuation mark? */
				TRACE( 1, "getparm: continue symbol\n" );
				while( AFIgettoken( fptr->file, inbuf ) != 0 );  /* skip to e of buf */
				len = AFIgettoken( fptr->file, inbuf );       /* then get next token */
				break;
   
			case EX_SYM:                          /* expression start symbol? */
				if( len > 1 )                       /* allow [4 to be treated as [ 4 */
					AFIpushtoken( fptr->file, inbuf+1 );
				TRACE( 2, "getparm: expression evaluation starts\n" );
				FMevalex( exbuf );                      /* parse expression */
				strcpy( inbuf, exbuf );
				*buf = inbuf;
				len = strlen( inbuf );
				break;

			case '`':
				TRACE(5, "sussing out back quoted parameter: (%s)\n", inbuf );
				AFIsetflag( fptr->file, AFI_F_WHITE, AFI_SET ); 		/* get white space */
         		 
				i = 0;
				cp = inbuf + 1;			/* skip lead bquote */
				do
				{
					if( i + len >= sizeof( exbuf ) )
					{
						exbuf[50] = 0;
						fprintf( stderr, "buffer overrun detected in getparm: %s...\n", exbuf );
						exit( 1 );
					}

					for( ; *cp && *cp != '`'; cp++ )
					{
						if( *cp == '^'  &&  *(cp+1) == '`' )
							cp++;					/* skip escape and put in bquote as is */
						exbuf[i++] = *cp;	
					}

					exbuf[i] = 0;

					if( *cp == '`' )
						break;

					cp  = inbuf;						/* next token will be here */
				}
				while( (len = AFIgettoken( fptr->file, inbuf)) >  0 );


				strcpy( inbuf, exbuf );
				*buf = inbuf;
				len = strlen( inbuf );
				TRACE(2, "getparm returning backquoted parameter: len=%d (%s)\n", len, inbuf );
			
				
				AFIsetflag( fptr->file, AFI_F_WHITE, AFI_RESET ); 
				break;


			default:                               /* all other tokens */
       				if( len == 1 )
        			{
         				if( *inbuf == ':' )                /* simulate end of buffer */
						{
          					len = 0;                        /* by returning zero length token */
							TRACE(3, "getparm: forced end of parms (:)\n" );
						}
        			}               /* end if token length is one */
					else
					{
						if( (*inbuf == '^' && *(inbuf+1) == ':')  || (*inbuf == '^' && *(inbuf+1) == ':') || (*inbuf == '^' && *(inbuf+1) == '[') )
						{
							len--;
							*buf = inbuf+1;
						}
						else
							if( *inbuf == ':' )
							{
								AFIpushtoken( fptr->file, inbuf+1 );
								len = 0;				/* simulate end of line */
								flags2 |= F2_SMASH;
							}
					}
    		}               			/* end switch */
  	}                                    /* end if token len > 0 */
 	else                                   /* end of input line - end of parms */
  		inbuf[0] = EOS;                            /* ensure "empty" buffer */

 	return len;
}                 /* FMgetparm */
