extern void FMaddtok( char* buf, int len );
extern void FMbd( void );
extern void FMbeglst( void );
extern void FMbox( void );
extern void FMbxend( void );
extern void FMbxstart( int option, char *colour, int border, int width, char *align );
extern void FMcd( void );
extern void FMceject( int i );
extern void FMcell( int parms );
extern void FMcenter( void );
extern int FMcmd( char* buf );
extern void FMcss( void );
extern void FMditem( void );
extern void FMelse( void );
extern void FMendlist( int option );
extern void FMele_stack( int action, int type );
extern int FMflush( void );
extern void FMheader( struct header_blk *hptr );
extern void FMindent( int* mar );
extern int FMinit( int argc, char **argv );
extern void FMjustify( void );
extern void FMline(  void );
extern void FMlisti( void );
extern void FMll( void );
extern void FMnofmt( void );
extern void FMpara( int i, int j );
extern void FMpflush( void );
extern void FMright( void );
extern void FMrunset( void );
extern void FMsection( void );
extern void FMsetcol( int gutter );
extern void FMsetfont( char* fname, int fsize );
extern void FMsetmar( void );
extern void FMsetx( void );
extern void FMsety( void );
extern void FMspace( void );
extern void FMtable( void );
extern void FMth( void );
extern void FMendtable( void );
extern void FMtc( void );
extern void FMtoc( int level );
extern void FMtr( void );
extern int copybuf( char *in, char *out, char escsym );