# $Id: Makefile.am 72926 2000-11-30 23:20:54Z waba $

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       ./libtiffax/libtif.la  -lm $(LIB_KFILE) 
INCLUDES=	-I$(srcdir)/libtiffax $(all_includes)

####### Files
noinst_HEADERS = kfax.h faxexpand.h print.h options.h

kfax_SOURCES   = kfax.cpp faxexpand.cpp faxinit.cpp faxinput.cpp\
		 fax2tiff.c fax2ps.c print.cpp options.cpp viewfax.cpp

METASOURCES    = kfax.moc print.moc options.moc

bin_PROGRAMS   = kfax

SUBDIRS = libtiffax

appsdir = $(kde_appsdir)/Graphics
apps_DATA = kfax.desktop

picsdir = $(kde_datadir)/kfax/pics
pics_DATA = kfax.tif kfaxlogo.xpm

KDE_ICON = kfax

EXTRA_DIST = $(apps_DATA) $(pics_DATA) 

messages:
	$(XGETTEXT) $(kfax_SOURCES) print.h ../kfaxview/*.cpp -o $(podir)/kfax.pot

####### Explicit dependencies


