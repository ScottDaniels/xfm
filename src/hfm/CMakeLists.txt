
# create a library then build tfm
file( GLOB hfm_files  *.c )
add_executable( hfm ${hfm_files} )
include_directories( ../base )
add_dependencies( hfm xfm )
target_link_libraries( hfm xfm;afi )

