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
&scd_start
&title  AFIgettoken

&name   AFIgettoken - Get next token from input stream.


&synop
        #include "afidefs.h"    
&space
        int AFIgettoken( AFIHANDLE file, char *buf );

&desc   The AFIgettoken routine examines the input stream for the 
        associated file (opened using AFIopen and put into tokenized
        input mode using AFIsettoken) to find the next token which is 
        copied into the buffer supplied by the caller. 
        Tokens are determined using the token seperator list that the 
        user application supplied to the AFIsettoken function. 
	The gettoken routine will return all characters from the 
	current position in the input stream until the next token 
	seperation character.  The characters that the user appliaction 
	has identifed as token seperator characters are returned as 
	single character tokens.  Whitespace is ignored unless the 
	whitespace flag (AFI_F_WHITE) is set which causes white space
	characters to also be returned to the calling application. 

&subcat Variable Tokens
	The tokenizer supports the expansion of variable tokens. A variable
	token is one that has a lead character which matches the variable
	symbol passed to the AFI environment during the AFIsettoken call.
	Variable expansion is accomplished by recognizing the variable 
	symbol as the lead character of a token, and then invoking a user
	application supplied look up function. The user lookup function 
	is passed the token (variable name) and is expected to return a 
	pointer to a buffer containing the tokens that the variable 
	expands into. If there is no user lookup funciton, or the lookup
	function returns a NULL pointer, then the variable name (with the 
	variable symbol) is returned as the encountered token. 

&space  Variables may also be passed parameters.  The parameter list for 
	a variable must be enclosed within parentheses, and seperated using
	the parameter separation character supplied by the user application 
	when it invoked the AFIsettoken function. If a parameter list is 
	attached to a variable name, the same lookup process is taken to 
	find the expansion buffer, and then the parameters are substituted
	into the expansion string with the first parameter being placed 
	where the characters '$1' are encountered, the second where '$2'
	are found and so forth. 

&space  If a variable name is immediatly followed by a punctuation character,
	the punctuation character is appended to the expansion string. 
	As this may not always be the desired placement, the tokenizer will
	search the expansion string for unescaped at signs (@) and place 
	the trailing punctuation character at each at sign location in 
	the expansion string.  When the at sign substution is performed 
	the punctuation is not appended to the end of the expansion string
	unless the last character in the expansion string is an at sign.


&subcat Concatination 
	If a variable name is enclosed within curly braces (e.g. ^&{color})
        then the token immediatly following the closing brace is appended 
	to the last token generated by the expansion process. 

&subcat Recursive Variables
	The expansion tokens of a variable may contain other variable names
	which are expanded when they are encountered by subsquent calls to 
	the AFIgettoken routine. 
	The tokenizer does not make any attempt to prevent "endless loop" 
	expansions and if encountered will probably cause the process to 
	crash.
	
&subcat Escaped Tokens And Symbols
        The tokenizer will recognize and remove the escape symbol (supplied
	by the user application during the AFIsettoken function call) from
	a token under two circumstances.
        character of a token and second character is the supplied variable 
        symbol.  No attempt is made to expand the resulting variable name 
	and the token is returned to the calling function.
	The tokenizer will also search for and remove the escape character
	from expansion buffers where the escape character immediatly 
	preceeds an at sign (@). The result is to leave the at sign in the 
	expansion buffer, and not to substitute a punctuation character for
	it. 
        
&parms  The AFIgettoken routine accepts two parameters:

&begterms
&term        file :  AFI handle of the open file.
&term        buf : Pointer to the buffer where token should be placed. 
&endterms

&return   The AFIgettoken routine returns the number of characters (bytes)
          of the token that were placed into the buffer. The length does
          not include the end of string ('\000') marker.  If the end of 
          file was encountered (no more tokens) then a length of AFI_ERROR
          is returned. If the user application has set the end of buffer
          signal flag (using the AFIset routine) then a length of 0 is 
          returned to the caller to indicate that the last token on the 
          logical record (prior to the newline) has already been returned.

&warn	The AFIgettoken routine does NOT check for recursion within variable
	expansions. Such a recursion will probably cause the process to 
	crash. 

&see	
AFIopen(), AFIset(), AFIsettoken()

&mods  
&begterms
&term	2 Apr 1997 (sd) : To support token seperator list.
&endterms


&bugs
This is the bugs section and it (hopefully) is indented.
&scd_end

***************************************************************************
*/

#include "afisetup.h"

int AFIgettoken( AFIHANDLE file, char *buf )
{
 int concat = 0;                /* concatination flag */
 int len = AFI_ERROR;           /* length of the token we are passing back */
 int pcount = 0;                /* # of times punctuation placed in var */
 char *vbuf;                    /* pointer at variable buffer */
 char *tok;                     /* start of token pointer */
 char *sptr;                    /* loop index - source pointer */
 char *dptr;                    /* loop index - destination pointer */
 struct file_block *fptr;       /* pointer at file management block */
 struct inputmgt_blk *imb;      /* pointer directly at imb */
 struct inputmgt_blk *new;      /* pointer at new imb to push */
 char tbuf[MAX_TBUF];           /* temp buffer for read */

 *buf = EOS;                             /* ensure user buffer is zapped */
 if( (fptr = files[file]) != NULL &&
     !(fptr->flags & F_EOF) )            /* good handle and not at end */
  {
   while( (!(fptr->flags & F_EOF)) &&              /* not eof but end of buf */
          fptr->tmb->ilist->idx >= fptr->tmb->ilist->end ) 
    {
     if( fptr->tmb->ilist->flags & IF_EOBSIG )   /* end of buffer signal? */
      {
       fptr->tmb->ilist->flags &= ~IF_EOBSIG;    /* clear so it happens once */
       return( 0 );                              /* return 0 length token */
      }
     else
      if( fptr->tmb->ilist->next == NULL )            /* last input buffer */
       {
        while( (len = AFIread( file, tbuf )) == 0 );
        fptr = files[file];               /* incase chain closed and new ptr */
        if( len > 0 )
         {                                    /* non blank record found */
          memcpy( fptr->tmb->ilist->buf, tbuf, len+1 );    /* save buffer */
          fptr->tmb->ilist->idx = fptr->tmb->ilist->buf;    /* reset index */
          fptr->tmb->ilist->end = fptr->tmb->ilist->buf + len;

          if( fptr->flags & F_EOBSIG)         /* need to mark real buffer? */
           fptr->tmb->ilist->flags |= IF_EOBSIG;
         }
       }                                     /* not last input buf on stack */
      else             
       {
        imb = fptr->tmb->ilist;
        fptr->tmb->ilist = fptr->tmb->ilist->next;  /* at next on stack */
        free( imb ); 
       }  
    }                                /* end while ! at end of buffer/file */

   if( ! (fptr->flags & F_EOF) )       /* if still not at eof */
    {
     if( !(fptr->flags & AFI_F_WHITE) ) /* skip lead white if not keep */
      for( imb = fptr->tmb->ilist;
           imb->idx < imb->end && isspace( *imb->idx );
           imb->idx++ );                /* find next nonblank - token start */
      else 
       imb = fptr->tmb->ilist;          /* dont skip, just point to ilist */


     if( imb->idx >= imb->end )        /* no more tokens left in this buffer */
      len = AFIgettoken( file, buf );   /* recurse to check or read more */
     else
      {                                        /* pull out the token */
       tok = imb->idx;                         /* point at token start */

       if( *imb->idx == fptr->tmb->varsym &&  /* variable ? */
           fptr->tmb->findvar != NULL )        /* and var search rtn find */
        {
         if( *(imb->idx+1) == '{' )            /* bracketed var name */
          {
           concat = 1;
           imb->idx++;                         /* skip it */
           tok += 2;
          }

         for( imb->idx++;  
              imb->idx < imb->end && (isalnum(*imb->idx) || *imb->idx == '_'); 
              imb->idx++ );                     /* find end of name */

         len = imb->idx - tok;                 /* length of the name */

         vbuf = (char *) (*fptr->tmb->findvar)( tok, len );    /* search */
         if( vbuf != NULL )                                  /* var found? */
          {
           new = (struct inputmgt_blk *) AFInew( INPUTMGT_BLK );
           if( imb->idx < imb->end && *imb->idx == '(' )  /* parameters? */
            {
             imb->idx++;                                  /* point past ( */
             for( tok = imb->idx; imb->idx < imb->end && *imb->idx != ')';
                  imb->idx++ )                       /* find end of plist */
              if( *imb->idx == fptr->tmb->escsym )   /* allow ^) */
               imb->idx++;                           /* skip escaped char */
             *imb->idx = EOS;              /* terminate for UTformat call */
             UTformat( tok, NULL, new->buf, vbuf, fptr->tmb->psep );          
             imb->idx++;
            }
           else          /* no parameters - just add expansion buffer */
            {
             if( concat )
              *imb->idx = ' ';

             for( pcount = 0, dptr=new->buf, sptr = vbuf; *sptr != EOS; 
                  dptr++, sptr++ )
              {
               if( *sptr == '@' )
                {
                 *dptr = *imb->idx;          /* replace @ with punctuation */
                 pcount++;                   /* that followed the var name */
                }
               else
                if( *sptr == fptr->tmb->escsym && *(sptr+1) == '@' )
                 {
                  sptr++;                      /* point past escape symbol */
                  *dptr = *sptr;               /* copy the @ unmolested */
                 }
                else
                 *dptr = *sptr;                /* just copy char */
              }                       /* end for - copy expansion buffer */

             if( ! pcount && *imb->idx != ' ' ) /* no @s in the expansion */
              {                                 /* and punctuation */
               *dptr = *imb->idx;               /* then add punct at end */
               ++dptr;
              }
             *dptr = EOS;                       /* ensure its nicely terms */
             ++imb->idx;                        /* skip punctuation char */
            }

           
           if( concat )
            {
             len = AFIgettoken( file, tbuf );    /* get next token */
             if( len > 0 )
              strncat( new->buf, tbuf, len );    /* add token to variable */
            }

           new->idx = new->buf;
           new->end = new->buf + strlen( new->buf );
           new->next = imb;                /* push new block onto the list */
           fptr->tmb->ilist = new;         /* point at new head */

           len = AFIgettoken( file, buf ); /* recurse to pull 1st tok of new */
          }                               /* end found expansion of var */
         else
          {
           if( ispunct( *imb->idx ) )      /* keep punctuation if there */
            {
             imb->idx++;                   /* point past it */
             len++;                        /* up the length */
             buf[len] = EOS;               /* nicely terminate the buffer */
            }
           strncpy( buf, tok, len );       /* no expansion - just copy name */
          }
        }                                 /* end if var and user cb routine */
       else                               /* token not variable */ 
        {                                 /* find end of token and move */
         if( *imb->idx == fptr->tmb->escsym &&
             *(imb->idx+1) == fptr->tmb->varsym )    /* trash escape char */
           {
            imb->idx++;                               /* only if ^& */
            tok++;
           }
 
         if( strchr( fptr->tmb->tsep, *imb->idx ) != NULL )  /* a token sep? */
          imb->idx++;                                    /* send back alone */
         else
          if( (sptr = strpbrk( imb->idx, fptr->tmb->tsep)) != NULL )
           imb->idx = sptr;
          else 
           imb->idx = imb->end;   /* no tok chars left, just @ end */

         len = imb->idx - tok; 
         strncpy( buf, tok, len );                      /* copy to user buf */
         buf[len] = EOS;                              /* proper termination */ 
        }
      }                                  /* end else - tokens left in buffer */
    }                                    /* end not at eof */
  }                                      /* end if valid file */

 return( len );   
}                      /* AFIgettoken */



/*
***************************************************************************
&scd_start
&title  AFIdummy
&name   AFIdummy - A dummy routine to test the second set of doc in one 
	file

&synop
        #include "afidefs.h"    
&space
        int AFIdummy( )

&desc   The AFIdummy routine is provided to allow for us to see if the 
	second set of documentation is handled correctly witin the 
	source file in parsing out the code inbetween documentation 
	sets. 

&parms  The routine expects no parameters.

&return The routine is declared void and thus does not return anything.
&scd_end
***************************************************************************
*/
void AFIdummy( )
{
 return( 0 );
}
