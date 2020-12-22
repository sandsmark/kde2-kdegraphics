SUBDIRS = pics

bin_PROGRAMS = kview
check_PROGRAMS = test

INCLUDES = $(all_includes)

METASOURCES = AUTO

lib_LTLIBRARIES = libkviewpart.la kview.la

libkviewpart_la_SOURCES = canvas.cpp kview_view.cc kview_asyncio.cc
libkviewpart_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
libkviewpart_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint

kview_SOURCES = dummy.C
kview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kview_LDADD = kview.la

kview_la_SOURCES = main.cpp viewer.cpp kview.cpp colour.cpp \
	filter.cpp filtlist.cpp \
	kcproc.cpp kviewconfdialog_base.ui kviewconfdialog.cpp \
        imagelistdialog_base.ui imagelistdialog.cpp \
        knumdialog_base.ui knumdialog.cpp \
        kfilteraction.cpp infowin_base.ui infowin.cpp

kview_la_LDFLAGS = $(all_libraries) -module -avoid-version
kview_la_LIBADD = libkviewpart.la

test_SOURCES = libkview.la test.cpp
test_LDFLAGS  = $(all_libraries) -module -avoid-version
test_LDADD = $(libkviewpart_la_LIBADD)
test.cpp:
	echo "int main() {return 0;};" > test.cpp


partdir = $(kde_datadir)/kview
part_DATA = kview_part.rc kview_popup.rc

lnkdir = $(kde_appsdir)/Graphics
lnk_DATA = kview.desktop

EXTRA_DIST = $(lnk_DATA)

KDE_ICON = kview

rcdir = $(kde_datadir)/kview
rc_DATA = kviewui.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cpp *.cc *.h -o $(podir)/kview.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kview kview *.h -lqt -lkdecore -lkdeui -lkfile

dummy.C:
	echo > dummy.C

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
