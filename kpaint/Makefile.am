# $Id: Makefile.am 107884 2001-07-26 22:50:53Z rich $
##

INCLUDES=	$(all_includes)
LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD	=       $(LIB_KSYCOCA) $(LIB_KFILE) 

appsdir = $(kde_appsdir)/Graphics
apps_DATA = kpaint.desktop

KDE_ICON = kpaint

kpaint_SOURCES = main.cpp canvas.cpp kpaint.cpp manager.cpp \
	infodialog.cpp palette.cpp palettedialog.cpp \
	canvassize.cpp properties.cpp \
	tool.cpp ellipse.cpp circle.cpp line.cpp brush.cpp \
	spraycan.cpp rectangle.cpp pen.cpp areaselect.cpp depthdialog.cpp \
	roundangle.cpp navwidget.cpp \
	mainview.cpp text.cpp

noinst_HEADERS = areaselect.h brush.h canvas.h canvassize.h circle.h \
	depthdialog.h ellipse.h infodialog.h \
	kpaint.h kundo.h line.h main.h manager.h navwidget.h palette.h \
	palettedialog.h pen.h properties.h rectangle.h roundangle.h \
	spraycan.h tool.h version.h mainview.h text.h

METASOURCES =	AUTO

rc_DATA = kpaint.rc
rcdir = $(kde_datadir)/kpaint

bin_PROGRAMS = kpaint

SUBDIRS = toolpics images

EXTRA_DIST = $(apps_DATA) 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpaint.pot

