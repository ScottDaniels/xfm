

# this builds the posscript init file from init.ps
add_custom_target( build_ps_init 
	DEPENDS "init.ps"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND "./gen_init_ps.ksh"
)

# files that go into the mix
set ( pfm_src fmaddtok.c 
	fmasis.c 
	fmbd.c 
	fmbeglst.c 
	fmbox.c 
	fmbxend.c 
	fmbxst.c
	fmccol.c 
	fmcd.c 
	fmceject.c 
	fmcmd.c 
	fmcpage.c 
	fmditem.c 
	fmendlis.c 
	fmep.c 
	fmfigure.c 
	fmflush.c 
	fmfmt.c
	fmformat.c 
	fmheader.c 
	fmimbed.c 
	fmindent.c 
	fminit.c 
	fmjustif.c
	fmline.c 
	fmnofmt.c 
	fmoneject.c 
	fmpflush.c 
	fmrfoot.c 
	fmmain.c
	fmright.c 
	fmrun.c 
	fmrunout.c 
	fmsetcolour.c 
	fmsetfon.c
	fmsetstr.c 
	fmsetx.c 
	fmsety.c 
	fmspace.c 
	fmstate.c 
	fmtable.c 
	fmtc.c
	fmtmpfon.c 
	fmtmpy.c 
	fmtoc.c 
	fmtoksiz.c 
	fmtopmar.c 
	fmvartok.c
	fmsection.c 
	fmcolnotes.c 
	fmfloat_mar.c
)

add_executable( pfm ${pfm_src} )
include_directories( ../base )
add_dependencies( pfm xfm; build_ps_init )
target_link_libraries( pfm xfm;afi )

install( 
	TARGETS pfm
	DESTINATION ${install_bin}
)
