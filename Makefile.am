INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I./src
libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libQtWorkbench.la

libQtWorkbench_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libQtWorkbench_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libQtWorkbench_la_SOURCES = src/qtwconfiguration.cpp src/qtworkbench.cpp src/qtwprogenerator.cpp src/qtwprojecthandler.cpp src/qtwprojectoptions.cpp

noinst_HEADERS = src/qtwconfiguration.h src/qtwmkspecbrowser.h src/qtworkbench.h src/qtwprogenerator.h src/qtwprojecthandler.h src/qtwprojectoptions.h

EXTRA_DIST = qtworkbench.cbp qtworkbench-linux.cbp res/manifest.xml res/QtWorkbenchForms.xrc


pkgdata_DATA = QtWorkbench.zip
CLEANFILES = $(pkgdata_DATA)

QtWorkbench.zip: res/manifest.xml
	PWD=`pwd` cd $(srcdir)/res && zip $(PWD)/QtWorkbench.zip manifest.xml *.xrc > /dev/null
