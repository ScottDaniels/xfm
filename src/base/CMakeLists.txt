

# create the base library which {t|p|h}fm all use
file( GLOB base_files  *.c *.h )
add_library( xfm STATIC ${base_files} )
