
# create a library then build tfm
set( hfm_files
	fmaddtok.c fmbd.c fmbeglst.c fmbox.c fmbxend.c fmbxst.c fmcd.c fmceject.c fmcell.c 
	fmcenter.c fmcss.c fmditem.c fmelse.c fmendlis.c 
	fmflush.c fmheader.c fmindent.c fminit.c fmjustif.c fmline.c fmlisti.c fmll.c 
	fmnofmt.c fmpara.c fmpflush.c fmright.c fmrunset.c fmsectio.c fmsetfon.c 
	fmsetmar.c fmsetx.c fmsety.c fmspace.c fmtable.c fmtc.c fmtoc.c fmtoksiz.c fmtr.c fmestack.c 
	fmcolnotes.c
)

add_executable( hfm ${hfm_files} )
include_directories( ../base )
add_dependencies( hfm xfm )
target_link_libraries( hfm xfm;afi )

