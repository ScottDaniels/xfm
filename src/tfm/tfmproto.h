/*
	Mnemonic:	tfmproto.h
	Abatract:	Protocol statements for the functions which are either unique to tfm or override
				a function in the base library. This can be gnerated with the command following the
				flower box provided all external functions are written with 'external' starting in 
				column 1 followed by the type an function name on the same line.

	Author:		E. Scott Daniels
*/
//				for x in *.c; do egrep "^extern" ${x:-foo}; done |  sort -k 2,2 | sed 's/{//; s/).*/);/'

extern int FMcolnotes_show( int end );
extern int FMflush( void );
extern int FMinit( int argc, char **argv );
extern void FMaddtok( char* buf, int len );
extern void FMbd( void );
extern void FMbeglst( void );
extern void FMbxend( void );
extern void FMbxstart( int option, char *colour, int border, int width, char *align );
extern void FMcell( int parms );
extern void FMcenter( void );
extern void FMcolnotes( void );
extern void FMditem(  void );
extern void FMendlist( int option );
extern void FMheader( struct header_blk* hptr );
extern void FMignore( void );
extern void FMjustify(  void );
extern void FMline( void );
extern void FMlisti( void );
extern void FMnofmt( void );
extern void FMpara( int i, int j );
extern void FMspace( void );
extern void FMtr( void );
