project( xfm LANGUAGES C )
cmake_minimum_required( VERSION 3.0 )


# version is for all of {X}fm; {t|p|h}fm have other internal versions.
set( major_version "1" )
set( minor_version "0" )
set( patch_level "1" )

set( install_root "/usr/local" )	# hummm, is /opt better for this?
set( install_bin "bin" )
set( install_lib "share/xfm" )		# into share because it is NOT hardware dependent

include( GNUInstallDirs )

# get the current git commit id which will be passed to each compile for version strings
execute_process( COMMAND
	git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE commit_id
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)

set( CMAKE_CXX_FLAGS "-g -Wall" )

# build libraries (base and afilio), then build the formatters
add_subdirectory( src/base )
add_subdirectory( src/afileio )
add_subdirectory( src/tfm )
add_subdirectory( src/pfm )
add_subdirectory( src/hfm )

target_compile_definitions( tfm PUBLIC COMMIT=${commit_id} ) 
target_compile_definitions( pfm PUBLIC COMMIT=${commit_id} ) 
target_compile_definitions( hfm PUBLIC COMMIT=${commit_id} ) 

install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/tfm/tfm 
    ${CMAKE_CURRENT_BINARY_DIR}/src/pfm/pfm
    ${CMAKE_CURRENT_BINARY_DIR}/src/hfm/hfm

    DESTINATION ${install_bin}
)

# find the template files and set them up for install
file( GLOB templates ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.im )
install( FILES
	${templates}
	DESTINATION ${install_lib}
)

IF( EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake" )
        include( InstallRequiredSystemLibraries )

        set( CPACK_set_DESTDIR "on" )
        set( CPACK_PACKAGING_INSTALL_PREFIX "${install_root}" )
        set( CPACK_GENERATOR "DEB" )

        set( CPACK_PACKAGE_DESCRIPTION "A document formatter which generates postscript, ASCII and HTML with extensions to generate troff, rts and markdown." )
        set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "{X}fm Text Formatting Utilities" )
        set( CPACK_PACKAGE_VENDOR "esdaniels" )
        set( CPACK_PACKAGE_CONTACT "klimbrox@gmail.com" )
        set( CPACK_PACKAGE_VERSION_MAJOR "${major_version}" )
        set( CPACK_PACKAGE_VERSION_MINOR "${minor_version}" )
        set( CPACK_PACKAGE_VERSION_PATCH "${patch_level}" )
        set( CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${major_version}.${minor_version}.${CPACK_PACKAGE_VERSION_PATCH}" )
        set( CPACK_SOURCE_PACKAGE_FILE_NAME "vric${CMAKE_PROJECT_NAME}_${major_version}.${minor_version}.${CPACK_PACKAGE_VERSION_PATCH}" )

        #we have no dependcies
        #set( CPACK_DEBIAN_PACKAGE_DEPENDS "nothing ( >= 1.1.1 ), nobody ( >= 1.1.1 )" )

        set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
        set( CPACK_DEBIAN_PACKAGE_SECTION "tools" )
        set( CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )

        # this seems ingnored if included
        #set( CPACK_COMPONENTS_ALL Libraries ApplicationData )

        INCLUDE( CPack )
ENDIF()
