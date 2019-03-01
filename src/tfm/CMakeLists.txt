
# build tfm
file( GLOB tfm_files  *.c )
add_executable( tfm ${tfm_files} )
include_directories( ../base )
target_link_libraries( tfm xfm;afi )

