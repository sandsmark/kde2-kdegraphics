## kdegraphics/Makefile.am $Id: Makefile.am 86592 2001-03-12 12:33:44Z dfaure $
## (C) 1997 Stephan Kulow

AUTOMAKE_OPTIONS = foreign 1.4
COMPILE_FIRST = kviewshell $(KSCANDIR)
SUBDIRS = $(COMPILE_FIRST) $(TOPSUBDIRS)
DISTCLEANFILES = inst-apps

install-data-local:
	@echo ""
	@echo ""
	@echo ""
	@echo "*************** Important *************************"
	@echo ""
	@echo " add "$(bindir)" to your PATH"
	@echo ""
	@echo " Please report bugs with our web form at"
	@echo " http://bugs.kde.org."
	@echo ""
	@echo " Stephan Kulow <coolo@kde.org> and all the KDE"
	@echo " developers wish you fun with the KDE."                     
	@echo "****************************************************"
	@echo ""

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

EXTRA_DIST = admin bsd-port debian kdebase.spec.in README.pam kde.pamd 

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

$(top_srcdir)/configure.in: $(top_srcdir)/configure.files $(shell test -f $(top_srcdir)/configure.files && sed -e "s%^%\$(top_srcdir)/%" $(top_srcdir)/configure.files)
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in
$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs
$(top_srcdir)/configure.files:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.files

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 SUBDIRS

