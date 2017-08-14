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

/* prototypes 
	easily generated with: grep -h "^extern" *.c |sed 's/{//; s/$/;/'
*/


extern void get_mdy( int *m, int *d, int *y );
extern void get_times( int *h, int *m, int *s );
extern void di_register( char *name, int insert );
extern void di_synonym( char *exist, char *new )	;
extern void di_group( char *name, char *word );
extern void di_add( char *name, int pg );
extern void di_list( FILE *f, char *ufmt, char *uspace, char *gfmtb, char *gfmte );
extern void FMaddtok( char* buf, int len );
extern void FMasis( void  );
extern void FMbd( void  );
extern void FMbeglst( void  );
extern void FMbox(  void );
extern void FMbxend( void );
extern void FMbxstart( int option, char *colour, int border, int width, char *align );
extern void FMcapture( void );
extern void FMccol( int skip );
extern void FMcd( void  );
extern void FMceject( int force );
extern void FMcenter(  void );
extern void FMclose(  void );
extern int FMcmd( char* buf );
extern char *FMcollect(  void );
extern void FMcomma(  void );
extern void FMcpage(  void );
extern void FMdefheader(  void );
extern void FMditem( void );
extern void FMdo(  void );
extern void fmdump( char *ptr );
extern void FMset_var( char *vname, char *buf );
extern void FMdv( void );
extern void FMelse(  void );
extern void FMendlist( int option );
extern void FMendtable(  void );
extern void FMep( void  );
extern int FMevalex( char* rbuf );
extern void FMfigure(  void );
extern int FMflush( void );
extern void FMformat(  void );
extern int FMgetparm( char **buf );
extern int FMgetpts( char* tok, int len ) ;
extern int FMgettok( char **buf );
extern void FMgetval( void );
extern char* FMget_header_num( ) ;
extern char* FMget_header_txt( ) ;
extern char* FMmk_header_snum( int level ) ;
extern void FMmk_header_stxt( char* txt ) ;
extern void FMhn(  void );
extern char* FMi2roman( int x  );
extern void FMif(  void );
extern void FMignore(  void );
extern void FMimbed(  void );
extern void FMindent( int* mar );
extern void fmindex(  void );
extern int FMinit( int argc, char **argv );
extern void FMjump(  void );
extern void FMjustify(  void );
extern void FMline(  void );
extern void FMlisti(  void );
extern void FMll(  void );
extern int main( int argc, char** argv );
extern void FMmsg( int msg, char *buf );
extern void FMnofmt(  void );
extern int FMopen( char *name );
extern void FMpara( int fi, int opt );
extern void FMpflush(  void );
extern void FMpgnum(  void );
extern int FMread( char* buf );
extern void FMrfoot( void );
extern void FMrhead( void );
extern void FMright(  void );
extern void FMrow( int option );
extern void FMrun( void );
extern void FMrunout( int page, int shift );
extern void FMrunset( void );
extern void FMsection(  void );
extern void FMsetcol( int gutter );
extern void FMset_colour(  void );
extern void FMsetfont( char* fname, int fsize );
extern void FMsetjust( void );
extern void FMsetmar( void );
extern void FMsetstr( char **string, int slen );
extern void FMsetx(  void );
extern void FMsety( void );
extern void FMshowvars(  void );
extern void FMskip(  void );
extern void FMspace(  void );
extern void FMtable(  void );
extern void FMtc( void );
extern void FMtmpy( int cmd );
extern void FMtoc( int level );
extern int FMtoksize( char* tok, int len );
extern void FMtopmar( void );
extern int FMvartok( char ** buf );
extern void sym_clear( Sym_tab *table );
extern void sym_dump( Sym_tab *table );
extern Sym_tab *sym_alloc( int size );
extern void sym_del( Sym_tab *table, unsigned char *name, unsigned int class );
extern void *sym_get( Sym_tab *table, unsigned char *name, unsigned int class );
extern int sym_put( Sym_tab *table, unsigned char *name, unsigned int class, void *val );
extern void sym_stats( Sym_tab *table, int level );
extern void sym_foreach_class( Sym_tab *st, unsigned int class, void (* user_fun)(), void *user_data );
extern void UTformat( char* inbuf, char** inlist, char* outbuf, char* control, char* delim );
