SUBDIRS = data

appsdir = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

bin_PROGRAMS = 	kghostview
lib_LTLIBRARIES = libkghostview.la

libkghostview_la_SOURCES = kgv_miniwidget.cpp marklist.cpp messages.cpp \
infodialog.cpp gotodialog.cpp kgvshell.cpp kpswidget.cpp kgvpageview.cpp ps.c \
kgv_view.cpp scrollbox.cpp kdsc.cpp kgvpagedecorator.cpp kgvconfigdialog.cpp \
kgvmainwidget.cpp

noinst_HEADERS =  marklist.h messages.h infodialog.h gotodialog.h kgvshell.h \
kpswidget.h kgvpageview.h ps.h kgv_miniwidget.h kgv_view.h scrollbox.h kdsc.h \
kgvpagedecorator.h kmaybe.h kgv.h kgvconfigdialog.h kgvmainwidget.h

kghostview_SOURCES = main.cpp
kghostview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kghostview_LDADD = libkghostview.la

libkghostview_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libkghostview_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint

METASOURCES = AUTO
EXTRA_DIST = kghostview.desktop 

KDE_ICON = kghostview

apps_DATA = kghostview.desktop 

partdir = $(kde_datadir)/kghostview
part_DATA = kgv_part.rc kghostviewui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kghostview.pot

# check_PROGRAMS = testdsc

# testdsc_SOURCES = testdsc.cpp kdsc.cpp ps.c
# testdsc_LDADD = -lqt



