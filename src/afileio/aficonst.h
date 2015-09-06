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
***************************************************************************
*
*  Mnemonic: aficonst.h
*  Abstract: This file contains constants that are for internal use of the 
*            AFI routines only. This file should NOT be included by any
*            user based subroutines.
*  Author:   E. Scott Daniels
*
***************************************************************************
*/

#define EOS     '\000'     /* end of string marker */
#define TRUE    1
#define FALSE   0

#define AFIHANDLE int   /* file handle "type" */

                        /* indexes into file descriptor array for a pipe */
#define READ_PIPE  0    /* fd for the read end of the pipe */
#define WRITE_PIPE 1    /* fd for the write end of the pipe */

#define FIRST   00
#define NEXT    01

#define F_AUTOCR  0x01   /* flags - auto generate a newline on write */
#define F_BINARY  0x02   /* binary data - dont stop read on newline */
#define F_STDOUT  0x04   /* output to standard output */
#define F_NOBUF   0x08   /* dont buffer data on write - immed flush */
#define F_STDIN   0x10   /* read from stdinput */
#define F_EOF     0x20   /* end reached on input - really need for stdin only*/
#define F_PIPE    0x40   /* "file" is a pipe */
#define F_WHITE   0x80   /* return white space as a token */
#define F_EOBSIG  0x100  /* set end of buffer signals on real reads */

                         /* input buffer management block flags */
#define IF_EOBSIG 0x01   /* signal end of buffer to user, dont auto stream */

#define DOT     '.'      /* dot character returned by directory look */

#define MAX_FILES 20     /* maximum number of file chains that can be open */
#define MAX_NAME  1024     /* number of characters in the file name */
#define MAX_CACHE 2048   /* size of the cache for pipes */
#define MAX_TBUF  2048   /* max size of a token management input buffer */


#define TOKENMGT_BLK 1   /* block types to allocate in new */
#define INPUTMGT_BLK 2
