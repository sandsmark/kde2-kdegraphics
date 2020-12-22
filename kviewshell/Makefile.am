appsdir = $(kde_appsdir)/Graphics
INCLUDES= $(all_includes)

####### Files

bin_PROGRAMS = 	kviewshell
lib_LTLIBRARIES = libkviewerpart.la libkmultipage.la libkpagetest.la

libkviewerpart_la_SOURCES = kviewpart.cpp marklist.cpp scrollbox.cpp gotodialog.cpp
libkviewerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkviewerpart_la_LIBADD = $(LIB_KFILE) -lkparts

libkmultipage_la_SOURCES = kmultipage.cpp centeringScrollview.cpp
libkmultipage_la_LDFLAGS = $(all_libraries)
libkmultipage_la_LIBADD = $(LIB_KFILE) -lkparts

libkpagetest_la_SOURCES = kpagetest.cpp 
libkpagetest_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkpagetest_la_LIBADD = $(LIB_KFILE) -lkparts libkmultipage.la

kviewshell_SOURCES = main.cpp kviewshell.cpp 
kviewshell_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kviewshell_LDADD = libkviewerpart.la

METASOURCES = AUTO
#EXTRA_DIST = kviewshell.desktop 

#KDE_ICON = kghostview

#apps_DATA = kghostview.desktop 

#partdir = $(kde_datadir)/kghostview
#part_DATA = kgv_part.rc kghostviewui.rc

rc_DATA = kviewshell.rc
rcdir = $(kde_datadir)/kviewshell
partrc_DATA = kviewerpart.rc
partrcdir = $(kde_datadir)/kviewerpart


data_DATA = paper-formats
datadir = $(kde_datadir)/kviewshell

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewshell.pot

