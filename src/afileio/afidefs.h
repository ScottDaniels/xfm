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
******************************************************************************
*  Public include file for the AFIleio package.
******************************************************************************
*/

#define AFIHANDLE int       /* prep to move to a totally dynamic environ */

                             /* various return codes */
#define AFI_OK        0      /* all processing went ok in routine */
#define AFI_ERROR    (-1)    /* problems during the afi routine */

                             /* various file flags user can set/reset */
#define AFI_F_AUTOCR  0x01   /* auto generate a newline on writes */
#define AFI_F_BINARY  0x02   /* binary data - dont stop read on newline */
#define AFI_F_STDOUT  0x04   /* file is standard output */
#define AFI_F_NOBUF   0x08   /* dont buffer writes - force flushes */
#define AFI_F_WHITE   0x80   /* return white space as token */
#define AFI_F_EOBSIG  0x100  /* signal end of read buffer when tokenizing */
#define AFI_F_EOFSIG  0x200	/* signal an eof even on chained files */

                             /* options for afistat */
#define AFI_CHAINED   0      /* return true if file is chained */
#define AFI_CHAINNUM  1      /* return the number of files in chain */
#define AFI_NAME      2      /* return the file name of the file */
#define AFI_OPENED    3      /* return number of opened files */
#define AFI_FLAGS     4      /* return the file's flags */
#define AFI_OPS       5      /* return the # operations done on the file */

#define AFI_SET    1         /* set flags when calling afisetflag */
#define AFI_RESET  0         /* reset flag(s) with afisetflag */


#define AFIopenpath AFIopenp           /* just incase user messed up */
                                       /* various macros for users */
#define AFIrewind(f) AFIseek((f),0l,0)    /* rewind the file */


/* function prototypes for the afi routines */
int AFIchain( int file, char *fname, char *opts, char *path );
int AFIclose1( int file );
int AFIclose( int file );
int AFIgettoken( AFIHANDLE file, char *buf );
int AFIopen( char *name, char *opts );
int AFIpushtoken( AFIHANDLE file, char *buf );
int AFIread( int file, char *rec );
int AFIreadp( int file, char *buf );		/* pipe read */
int AFIseek( int file, long offset, int loc );
void AFIsetflag( int file, int flag, int opt );
int AFIsetsize( int file, int num );
int AFIsettoken( AFIHANDLE file, Sym_tab *st, char *tsep, char varsym, char escsym, char *psep );
long AFIstat( int file, int opt, void **data );
int AFIwrite( int file, char *rec );
