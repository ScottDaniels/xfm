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
**************************************************************************
*
*  Mnemonic: FMevalex
*  Abstract: This routine will evaluate a reverse polish expression and
*            return the character equivalent of the result. The
*            expression may contain constant and already defined variables.
*            Expressions have the following format:
*               [ &var &var + ]   (Add var to var)
*  Returns:  The integer result of the expression
*  Parms:    Optional - buf - Pointer to buffer to convert result into
*  Date:     28 March 1993
*  Author:   E. Scott Daniels
*
*  Modified: 21 Apr 1997 - To calc in floating point
*            22 Apr 1997 - To truncate trailing 0s from string.
*            10 Jun 1997 - To add TIME command
*                          To add FRAC command
*                          To add INT command
**************************************************************************
*/
int FMevalex( rbuf )
 char *rbuf;
{
 int i;             /* integer value */
 int sp = 0;        /* stack pointer */
 char *idx;         /* pointer into string */
 double stack[100]; /* stack */
 char *buf;         /* pointer at next token to process */
 char fmt[100];     /* sprintf format string */
 int usr_fmt = 0; 

 sprintf( fmt, "%%.4f" );   /* default format string */

 stack[0] = 0;

while( FMgetparm( &buf ) ) 
  {
	TRACE( 3, "expr: working on (%s)\n", buf );

   switch( *buf )
    {
     case 'T':                      /* convert top of stack to time */
      i = stack[sp-1];              /* make int */
      stack[sp-1] = i + ((stack[sp-1] - i) * 0.60);
      break;

     case 'I':                      /* make top integer */
      i = stack[sp-1];
      stack[sp-1] = i;
      break;

     case 'F':                      /* leave fractial portion of top */
      i = stack[sp-1];
      stack[sp-1] = stack[sp-1] - i;
      break;

     case 'S':                      /* sum all elements on the stack */
      while( sp > 1 )
       {
        stack[sp-2] += stack[sp-1];   /* add top two and push */
        sp--;                         /* one less thing on stack */
       }
      break;
       
     case '+':
		if( isdigit( *(buf+1) ) )
      		stack[sp++] = atof( buf ); 		/* assume +n and push on stack */ 
		else
		{
			if( trace )
				fprintf( stderr, "eval: [%.3f %.3f +] = ", stack[sp-2], stack[sp-1] );
      		stack[sp-2] += stack[sp-1];   /* add top two and push */
      		sp--;                         /* one less thing on stack */
			if( trace )
				fprintf( stderr, "%.3f\n", stack[sp-2] );
		}
		break;

     case '-':
		if( isdigit( *(buf+1) ) )
      		stack[sp++] = atof( buf );  		/* assume -n and push on stack */
		else
		{
			if( trace )
				fprintf( stderr, "eval: [%.3f %.3f -] = ", stack[sp-2], stack[sp-1] );
      		stack[sp-2] = stack[sp-2] - stack[sp-1];   /* sub top two and push */

			if( trace )
				fprintf( stderr, "%.3f\n", stack[sp-2] );
      		sp--;                         /* one less thing on stack */
		}
		break;

     case '*':
      	stack[sp-2] *= stack[sp-1];   /* mul top two and push */
      	sp--;                         /* one less thing on stack */
      	break;

     case '/':
      	if( stack[sp-1] != 0 )
			stack[sp-2] = stack[sp-2]  / stack[sp-1];   /* div top two and push */
		else
		{
			fprintf( stderr, "division by zero detected -- bad values generated on stack\n" );
			stack[sp-2] = 0;
		}
      	sp--;                         /* one less thing on stack */
      	break;

     case '%':                           /* mod operator */
      	if( stack[sp-1] != 0 )
				stack[sp-2] = (int) stack[sp-2] % (int) stack[sp-1];
		else
		{
			stack[sp-2] = 0;
			fprintf( stderr, "modular division by zero detected -- bad values generated on stack\n" );
		}
      sp--;
      break;

     case '?':
      usr_fmt++;
      strcpy( fmt,  buf+1 );
      break;

     case ']':
		if( rbuf != NULL )                      /* if caller passed a buffer */
			sprintf( rbuf, fmt, stack[sp-1] );  /* convert to character */
		for( idx = rbuf + strlen( rbuf ) -1; *idx == '0'; idx-- );  
		{
			if( ! usr_fmt )
			{
				if( *idx == '.' )
					*idx = EOS;                       /* no fractional amt, cut it out */
				else
					idx[1] = EOS;                    /* leave last non 0 there */
			}
		}

		TRACE( 1, "expr: result: %.3f\n", stack[sp-1] );
		return( (int) stack[sp-1] );                /* return top of stack */
		break;

     default:               /* assume its a parameter to push on stack */
		stack[sp++] = atof( buf );  /* convert to integer and push on stack */
		break;
    }     /* end switch */

  }       /* end while */

                        /* if we fall out of the while then return top value */
 if( rbuf != NULL )  /* if caller passed a buffer */
  sprintf( rbuf, "%d", (int) stack[sp-1] );  /* convert to character */
}           /* FMevalex */
