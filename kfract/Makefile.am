
INCLUDES = $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD =   $(LIB_KDEUI) $(LIB_KFILE)

kfract_SOURCES = kfract.C kfractdlgs.C drawview.C fract.C \
                mandel.C julia.C main.C
METASOURCES = kfract.moc kfractdlgs.moc 
bin_PROGRAMS   = kfract

noinst_HEADERS = kfract.h kfractdlgs.h drawview.h fract.h julia.h lambda.h \
				mandel.h version.h

lnk_DATA = kfract.desktop
lnkdir = $(kde_appsdir)/Graphics

rc_DATA = kfract.rc
rcdir = $(kde_datadir)/kfract

KDE_ICON = kfract

EXTRA_DIST = $(lnk_DATA) 

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.C -o $(podir)/kfract.pot

