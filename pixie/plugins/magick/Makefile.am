
INCLUDES =  -I$(srcdir)/../../../libminimagick/ -I$(srcdir)/../../libifplugin $(all_includes) 

lib_LTLIBRARIES = libkif_magick.la 

libkif_magick_la_SOURCES = dialog.cpp magick.cpp
METASOURCES = AUTO

lnkdir = $(kde_datadir)/pixie/plugins
lnk_DATA = magick.desktop         	

EXTRA_DIST = $(lnk_DATA)

libkif_magick_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module 
libkif_magick_la_LIBADD= ../../../libminimagick/libminimagick.la ../../libifplugin/libkifplugin.la $(LIB_KDEUI)

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kif_magick kif_magick *.h -lqt -lkdecore -lkdeui -lkfile 

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
