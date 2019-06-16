
# build tfm

set( tfm_files
	fmaddtok.c
	fmbeglst.c
	fmbxend.c
	fmbxst.c
	fmcell.c
	fmcmd.c
	fmditem.c
	fmendlis.c
	fmflush.c
	fmheader.c
	fmignore.c
	fminit.c
	fmjustif.c
	fmline.c
	fmlisti.c
	fmmain.c
	fmpara.c
	fmtable.c
	fmtr.c
	fmspace.c
	fmcenter.c
	fmcolnotes.c
)

add_executable( tfm ${tfm_files} )
include_directories( ../base )
target_link_libraries( tfm xfm;afi )

install(
	TARGETS tfm
	DESTINATION ${install_bin}
)
