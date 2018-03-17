/*
	Mnemonic:	pfmproto.h
	Abatract:	Protocol statements for the functions which are either unique to pfm or override
				a function in the base library. This can be gnerated with the command following the
				flower box provided all external functions are written with 'external' starting in 
				column 1 followed by the type an function name on the same line.

	Author:		E. Scott Daniels
*/
//				for x in *.c; do egrep "^extern" ${x:-foo}; done |  sort -k 2,2 | sed 's/{//; s/).*/);/'

extern void FMchop( char *buf, int len, int remain );
extern void FMaddtok( char *buf, int len );
extern void FMasis( );
extern void FMbd( void );
extern void FMbeglst( void );
extern void FMbox( void );
extern void FMbxend( void );
extern void FMbxstart( int option, char *colour, int border, int width, char *align );
extern int FMcol_enough( int lines );
extern void FMccol( int skip );
extern void FMcd(  void );
extern void PFMceject( void );
extern int FMcmd( char *buf );
extern int FMcolnotes_show( int end );
extern void FMcolnotes( );
extern void FMcpage( void );
extern void FMdefheader( void );
extern void FMditem( void );
extern void FMendlist( int option );
extern void FMep(  void );
extern void FMfigure( void );
extern void FMfloat_mar( void );
extern int FMflush( void );
extern void FMfmt_dump( void );
extern void FMfmt_force_colour( char* colour );
extern int FMfmt_save( void );
extern int FMfmt_restore( void );
extern int FMfmt_largest( void )				/* find the largest font in the list */;
extern void FMfmt_end( void );
extern void FMfmt_add( void );
extern void FMfmt_yadd( int v );
extern int FMfmt_pop( int *size, char **font, char **colour, int *start, int *end, int *ydisp );
extern void FMformat( void );
extern void FMheader( struct header_blk *hptr );
extern void FMimbed( void );
extern void FMindent( int* mar );
extern int FMinit( int argc, char **argv );
extern void FMjustify( void );
extern void FMline( void );
extern void FMcline( void );
extern void FMll( void );
extern int main( int argc, char **argv ) ;
extern void FMnofmt( void );
extern void FMateject( int page );
extern void FMoneject( void );
extern void FMpflush( void );
extern void FMrfoot( void );
extern void FMrhead( void );
extern void FMright( void );
extern void FMrun( void );
extern void FMrunout( int page, int shift );
extern void FMsection ( void );
extern void FMpopcolour( );
extern void FMset_last_colour( );
extern void FMsetcolour( char *t );
extern void FMpushcolour(  char *new_colour );
extern void FMsetfont( char* fname, int fsize );
extern void FMsetstr( char **string, int slen );
extern void FMsetx( void );
extern void FMsety( void );
extern void FMspace( void  );
extern void FMpush_state( void );
extern void FMpop_state( void );
extern void FMtable(  void );
extern void FMth( void );
extern void FMcell( int parms );
extern void FMtr( int last );
extern void FMpause_table( void ) ;
extern void FMrestart_table( void ) ;
extern void FMendtable( void );
extern void FMtc( void );
extern void FMtmpfont( void );
extern void FMtmpy( int cmd );
extern void FMtoc( int level );
extern int FMtoksize( char* tok, int len );
extern void FMtopmar( void );
extern int FMvartok( char **buf );
