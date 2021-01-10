
INCLUDES = $(all_includes) 

lib_LTLIBRARIES = libpixie_thumb.la

libpixie_thumb_la_SOURCES = thumbiconview.cpp thumbpropsdlg.cpp thumbtree.cpp\
	thumbpreview.cpp thumbtoplevel.cpp  hotlistbox.cpp

METASOURCES = AUTO

libpixie_thumb_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libpixie_thumb_la_LIBADD = $(LIB_KSYCOCA)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
