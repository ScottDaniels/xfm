/*
=================================================================================================
	(c) Copyright 1995-2011 By E. Scott Daniels. All rights reserved.

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
/*
*****************************************************************************
*
*  Mnemonic: AFIreadp
*  Abstract: This routine is responsible for reading the next "buffer" 
*            from the pipe represented by the file pointer passed in.
*            As the AFI RTL is designed to proivde an application routine 
*            with an interface to a generic "file" the stream IO from a 
*            pipe is broken into "records" and EOS terminated as if the 
*            data had been read from a file.  The user may set the binary
*            flag on for the file which will cause the pipe contents to be
*            unparsed by this routine and the user will be able to receive
*            streamed input. 
*
*            NOTE:    This routine is desined to be called by the AFIread
*                     routine and is NOT intended for public use.
*
*            WARNING: While the routine is not directly recursive, there 
*                     is a recursion potential as this routine calls AFIread
*                     which may call this routine if the chained file is
*                     also a pipe. 

*  Parms:    file - File handle of the file to process. 
*            buf  - Pointer to storage buffer for read to place bytes from 
*                   the stream.
*  Returns:  The number of bytes actually read or -1 if there was an error
*            (the errno system global is left unchanged for the caller to 
*            examine if desired upon receiving an error indication.)
*  Date:     11 March 1997
*  Author:   E. Scott Daniels
*
******************************************************************************
*/
#include "afisetup.h"                 /* get other headers etc. */

int AFIreadp( int file, char *buf )
{
 struct file_block *fptr;        /* pointer at file block for the open pipe */
 char* bptr;                     /* pointer into user's buffer */
 int len = -1;                  /* number of bytes moved to user's buffer */
 int rlen;                       /* # bytes delivered by read call */

 if( (fptr = afi_files[file]) == NULL )
  return( AFI_ERROR );                 /* bad # passed in - get out now */

 if( fptr->flags & F_BINARY )               /* user wants real stream info */
  len =  read( fptr->pptr->pipefds[READ_PIPE], buf, fptr->max );
 else
  {                 /* read from pipe until buf full, or end of record */
   bptr = buf;
   *bptr = ' ';     /* allow entry into the loop */
 
   do
    {
     if( fptr->pptr->cptr >= fptr->pptr->cend )  /* time to read from pipe? */
      {
       rlen = read( fptr->pptr->pipefds[READ_PIPE], 
                                 fptr->pptr->cache, fptr->pptr->csize );
       
       if( rlen < 0 )              /* end of file on the pipe */
        {
         buf[++len] = EOS;         /* terminate the buffer */
        
         if( len == 0 )            /* nothing moved from cache before read */
          {
           if( fptr->chain != NULL)        /* another file chained on */
            {
             AFIclose1( file );            /* close the pipe up and unchain */
             len = AFIread( file, buf );   /* read from next file in chain */
            }
           else
            len = AFI_ERROR;         /* end of file - no more to read from */
          }
        }                            /* end if read failed */
       else                        
        {                            /* cache has new stuff! */
         fptr->pptr->cend = fptr->pptr->cache + rlen;  /* mark end of buffer */
         fptr->pptr->cptr = fptr->pptr->cache;        /* start at beginning */
        }                  /* end successful read into cache */  
      }                    /* end if we needed to read from pipe */

                    
      do                      /* copy from cache to user's buffer */
       {
        ++len;                           /* count of bytes placed in buf */
        *bptr =  *fptr->pptr->cptr == '\n' ? EOS : *fptr->pptr->cptr;
        ++bptr;                          /* @ next insertion point */
        ++fptr->pptr->cptr;              /* move to next in cache */
       }
      while( len < fptr->max &&                        /* buffer not full */
             fptr->pptr->cptr < fptr->pptr->cend &&    /* still cache */
             buf[len] != EOS );                        /* buf not terminated */
    }                                             
   while( buf[len] != EOS && len < fptr->max - 1 );    /* end while */

  }                                                 /* end not binary */

 return( len ); 
}                     /* AFIreadp */
