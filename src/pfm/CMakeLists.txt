

add_custom_target( build_ps_init 
	DEPENDS "init.ps"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND "./gen_init_ps.ksh"
)

# files that go into the mix
file( GLOB pfm_files  *.c )

add_executable( pfm ${pfm_files} )
include_directories( ../base )
add_dependencies( pfm xfm; build_ps_init )
target_link_libraries( pfm xfm;afi )

