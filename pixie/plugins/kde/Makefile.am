
INCLUDES = -I$(srcdir)/../../libifplugin $(all_includes) 

lib_LTLIBRARIES = libkif_kdeeffects.la

libkif_kdeeffects_la_SOURCES = dialogs.cpp effects.cpp
METASOURCES = AUTO

lnkdir = $(kde_datadir)/pixie/plugins
lnk_DATA = kdeeffects.desktop         	

EXTRA_DIST = $(lnk_DATA)

libkif_kdeeffects_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libkif_kdeeffects_la_LIBADD = ../../libifplugin/libkifplugin.la $(LIB_KDEUI)

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kif_kdeeffects kif_kdeeffects *.h -lqt -lkdecore -lkdeui -lkfile 

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
