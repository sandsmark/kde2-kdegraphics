#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

APPSDIR = $(kde_appsdir)/Graphics
INCLUDES = $(all_includes)
SUBDIRS = pics

bin_PROGRAMS = kiconedit

kiconedit_SOURCES = utils.cpp main.cpp kiconedit.cpp kicongrid.cpp \
    kiconcolors.cpp kcolorgrid.cpp \
    kicontools.cpp kicon.cpp kresize.cpp knew.cpp properties.cpp \
    kiconeditslots.cpp kiconconfig.cpp \
    kwizard.cpp knotebook.cpp ktabbar.cpp

kiconedit_METASOURCES = USE_AUTOMOC

# the library search path. 
kiconedit_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kiconedit_LDADD = $(LIB_KFILE) -lkdeprint

DISTCLEANFILES = *~ .deps/* .libs/*

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kiconedit.desktop $(APPSDIR)

uninstall-local:
	-rm -f $(APPSDIR)/kiconedit.desktop

messages:
	$(XGETTEXT) $(kiconedit_SOURCES) -o $(podir)/kiconedit.pot

