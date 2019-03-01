
# create the alternate file I/O library which {t|p|h}fm all use
file( GLOB afi_files  *.c *.h )
add_library( afi STATIC ${afi_files} )
