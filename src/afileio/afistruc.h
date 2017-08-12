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
**************************************************************************
*
*    This file contains the structures for the file i/o package.
*    17 November 1988
*    E. Scott Daniels
*
**************************************************************************
*/

struct inputmgt_blk            /* input buffer management block for tokens */
 {
  struct inputmgt_blk *next;   /* these are a fifo queue of input buffers */
  char *idx;                   /* index at next char to process in buffer */
  char *end;                   /* index of 1 past last valid char in buf */
  int flags;                   /* flags- the IF_xxx constants */
  char *buf;
 };
 
struct tokenmgt_blk            /* token management block */
 {
  struct inputmgt_blk *ilist;  /* list of input buffers to process */
  char varsym;                 /* variable symbol */
  char escsym;                 /* escape symbol */
  char *tsep;                  /* list of chars that act as token seps */
  char *psep;                  /* parameter seperator character */
 };

struct pipe_blk                 /* info needed to manage a pipe */
 {
  int pipefds[2];               /* file descriptors  */
  char *cache;                  /* cache buffer */
  char *cptr;                   /* index into the current cache */
  char *cend;                   /* last piece of valid data in cache */
  int csize;                    /* number of bytes allocated for cache */
 };

struct file_block  /* defines an open file - pointed to by files array */
 {
  struct file_block *chain;     /* pointer to next file block in chain */
  struct pipe_blk *pptr;        /* pointer to pipe info if file is a pipe */
  struct tokenmgt_blk *tmb;     /* token management block for tokenized strm */
  char* name;			        /* name of the file */
  FILE *file;                   /* real file handle (pointer) */
  int flags;                    /* read write flags */
  int max;                      /* max number of characters to read/write */
  long operations;              /* number of operations on the file */
  Sym_tab *symtab;		/* variable lookup symbol table (supplied by user on settok call */
 };


 extern struct file_block **afi_files;

char *AFIisvar( char *tok, struct tokenmgt_blk *tmb );
