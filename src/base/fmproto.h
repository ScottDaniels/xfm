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

/* prototypes */


void FMaddtok( char *, int );
void FMasis( );
void FMbd( );
void FMbeglst( );
void FMbox( );
void FMbxend( );
void FMbxstart( int option, char *colour, int border, int width, char *align );
char *FMcollect();
void FMccol( int );
void FMcd( );
void FMceject( int );
void FMcenter( );
void FMclose( );
int FMcmd( char *);
void FMcpage( );
void FMcpage( );
void FMdefheader( );
void FMditem( );
void FMdv( );
void FMelse( );
void FMendlist( int );
void FMep( );
int FMevalex( char *  );
void FMfigure( );
int FMflush( );
void FMformat( );
int FMgetparm( char **);
int FMgetpts( char *, int );
int FMgettok( char ** );
void FMgetval( );
/*struct var_blk *FMgetvar( char *); */
char *FMgetvar( char *, int ); 
void FMheader( struct header_blk *);
void FMhn( );
void FMif( );
void FMimbed( );
void FMindent( int * );
int FMinit( int, char ** );
void FMjustify( );
void FMline( );
void FMlisti( );
void FMll( );
void FMmsg( int, char *);
void FMnofmt( );
int FMopen( char *);
void FMpara( int, int );
void FMpflush( );
void FMpgnum( );
int FMread( char *);
void FMright( );
void FMrunout( int, int);
void FMrunset( );
void FMsection( );
void FMsetcol( int );
void FMsetfont( char *, int );
void FMsetjust( );
void FMsetmar( );
void FMsetstr( char **, int );
void FMsetx( );
void FMsety( );
void FMspace( );
void FMtc( );
void FMtmpy( int );
void FMtoc( int );
int FMtoksize( char *, int );
void FMtopmar( );
int FMvartok( char ** );
