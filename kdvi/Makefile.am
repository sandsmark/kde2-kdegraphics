# just set the variable
appsdir = $(kde_appsdir)/Graphics
# set the include path for X, qt and KDE
INCLUDES= -I$(top_srcdir)/kviewshell $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = . pix

# you can add here more. This one gets installed
lib_LTLIBRARIES= libkdvi.la
noinst_PROGRAMS = squeeze

# just to make sure, automake makes them
METASOURCES = AUTO

libkdvi_la_SOURCES = kdvi_multipage.cpp \
	dviwin.cpp \
	optiondialog.cpp infodialog.cpp\
	psheader.c dviwin_draw.cpp dvi_init.cpp\
	font.cpp fontpool.cpp fontprogress.cpp pk.cpp psgs.cpp\
	special.cpp util.cpp vf.cpp glyph.cpp

libkdvi_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdvi_la_LIBADD = $(LIB_KPARTS) -lkdeprint \
			$(top_builddir)/kviewshell/libkmultipage.la

# Which sources should be compiled for squeeze.
squeeze_SOURCES	= squeeze.c


## this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = c-openmx.h dvi.h dviwin.h oconfig.h\
	optiondialog.h xdvi.h 

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kdvi.pot

apps_DATA = kdvi.desktop


bin_SCRIPTS = kdvi

partdir = $(kde_datadir)/kdvi
part_DATA = kdvi_part.rc

psheader.c: psheader.txt squeeze
	./squeeze $(srcdir)/psheader.txt $@

# serial num for Makefile.am to retrigger remake: 1
