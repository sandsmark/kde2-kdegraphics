SUBDIRS=pics libifplugin plugins thumbnails color
appsdir = $(kde_appsdir)/Graphics
apps_DATA = pixie.desktop

INCLUDES= $(all_includes)

bin_PROGRAMS = pixie

pixie_SOURCES = main.cpp imagelist.cpp imagewindow.cpp \
	pluginmenu.cpp moviewindow.cpp slidedialog.cpp \
	imagelistitem.cpp sizedialog.cpp \
	ifapp.cpp bigwindow.cpp \
	htmlexport.cpp dndlistbox.cpp screengrab.cpp miscbindings.cpp


pixie_METASOURCES = AUTO
pixie_LDFLAGS = $(all_libraries) $(KDE_RPATH) -export-dynamic

pixie_LDADD   = $(LIB_KSYCOCA) $(LIB_KFILE)

messages:
	$(XGETTEXT) `find . -name "*.cpp"` -o $(podir)/pixie.pot

KDE_ICON = pixie


