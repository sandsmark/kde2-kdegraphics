appsdir = $(kde_appsdir)/Graphics
INCLUDES= -Ikpathsea -I$(top_srcdir)/kviewshell \
	-I$(top_srcdir)/kfax/libtiffax \
	 $(all_includes)

lib_LTLIBRARIES= libkfax.la

METASOURCES = AUTO

libkfax_la_SOURCES = kfax_multipage.cpp tiffsplit.c

libkfax_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkfax_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint -ltiff \
		$(top_builddir)/kfax/libtiffax/libtif.la \
		$(top_builddir)/kviewshell/libkmultipage.la

messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kfax.pot

#The real kfax program (in ../kfax) can also display raw g3 files,
#which the kfax-by-kviewshell can't
#apps_DATA = kfax.desktop
#bin_SCRIPTS = kfax

partdir = $(kde_datadir)/kfax
part_DATA = kfax_part.rc


