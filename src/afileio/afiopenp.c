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
*  Mnemonic: AFIopenpath
*  Abstract: This routine will attempt to open the file name as it is passed
*            in after it has been appended to the directory names that are
*            contained in the path string. Directories are tried one at a
*            time until the end of the path string is reached, or the file
*            is opened. If the path string is NULL then only the name is
*            used on the open attempt.
*  Parms:    name  - Pointer to the file name.
*            mode  - Mode string to pass to AFIopen
*            path  - Pointer to a list of directory path names seperated by
*                    semicolons.
*  Returns:  Status as returned by AFIopen.
*  Date:     7 January 1995
*  Author:   E. Scott Daniels
*
*  Modified: 11 Mar 1997 - To make multi-thread strtok_r call and for pipes
*			23 Jun 2015 - Modernisation.
*****************************************************************************
*/
#include "afisetup.h"    /* get necessary include files */

int AFIopenp( char *name, char *mode, char *path )
{
	int file;				/* file number from AFIopen */
	char *tok;       	  /* pointer at token in path and file name */
	char *tpath = NULL;   /* temp path buffer */
	char *tname = NULL;   /* temp name buffer */
	char *lasts;      	 /* string place holder for strtok_r */
	int len;			// working buffer len

	if( path == NULL ||                   /* allow no path, or if its an */
   		*name == '/'                  ||  /* absolute path name  or */
   		strncmp( name, "./", 2 ) == 0 ||  /* specific path name  or */
   		strcmp( name, "pipe" ) == 0   ||  /* pipe or */
   		strcmp( name, "stdin" ) == 0  ||  /* standardin or standard out then */
   		strcmp( name, "stdout" ) == 0     /* just ignore path and call open */
 	)
	{
		return( AFIopen( name, mode ) ); 
	}

	tpath = strdup( path );				// buffer we can trash with tok()
	len = strlen( name ) + strlen( path ) + 1;
	tname = (char *) malloc( len + 1 ); 
	if( tpath && tname ) {
		tok = strtok_r( tpath, ":;", &lasts );  /* point at first token in path */

		do {
 			snprintf( tname, len, "%s/%s", tok, name );  	// next path/name to try
 			file = AFIopen( tname, mode );
 			tok = strtok_r( NULL, ":;", &lasts );          /* point at next in path */
		}
		while( file < AFI_OK && tok != NULL );   /* until end of path or opened */

		free( tname );           /* release temp storage */
		free( tpath );
	}

	return( file );          /* send back the file handle */
}
