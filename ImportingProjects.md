<sub>Back to</sub> PluginUsage

# The victim #

Go get the sources of [texmaker](http://www.xm1math.net/texmaker/index.html) and follow along: http://www.xm1math.net/texmaker/download.html
Unarchive the sources in a folder (for this tutorial this folder will be: C:\Workbench\texmaker-1.6\texmaker\) and let's go...

# Prepare #

Unfortunately it is still required that you have a good understanding on how to use qmake if you want to migrate your projects. Hopefully this will change in the future and this page will become obsolete.
First let's open the .pro file that texmaker provides:
```
TEMPLATE	= app
LANGUAGE	= C++
TARGET	 = texmaker
CONFIG	+= qt warn_off release
###############################
HEADERS	+= texmaker.h \
	symbollistwidget.h \
	icondelegate.h \
	latexeditor.h \
	latexhighlighter.h \
	latexeditorview.h \
	linenumberwidget.h \
	logeditor.h \
	loghighlighter.h \
	metapostlistwidget.h \
	finddialog.h \
	gotolinedialog.h \
	replacedialog.h \
	structdialog.h \
	filechooser.h \
	tabbingdialog.h \
	arraydialog.h \
	tabdialog.h \
	letterdialog.h \
	addoptiondialog.h \
	quickdocumentdialog.h \
	usermenudialog.h \
	usertooldialog.h \
	refdialog.h \
	configdialog.h \
	aboutdialog.h \
	webpublishdialog.h \
	spellerdialog.h \
	pstrickslistwidget.h
SOURCES	+= main.cpp \
	texmaker.cpp \
	symbollistwidget.cpp \
	icondelegate.cpp \
	latexeditor.cpp \
	latexhighlighter.cpp \
	latexeditorview.cpp \
	linenumberwidget.cpp \
	logeditor.cpp \
	loghighlighter.cpp \
	metapostlistwidget.cpp \
	finddialog.cpp \
	gotolinedialog.cpp \
	replacedialog.cpp \
	structdialog.cpp \
	filechooser.cpp \
	tabbingdialog.cpp \
	arraydialog.cpp \
	tabdialog.cpp \
	letterdialog.cpp \
	addoptiondialog.cpp \
	quickdocumentdialog.cpp \
	usermenudialog.cpp \
	usertooldialog.cpp \
	refdialog.cpp \
	configdialog.cpp \
	aboutdialog.cpp \
	webpublishdialog.cpp \
	spellerdialog.cpp \
	pstrickslistwidget.cpp
RESOURCES += texmaker.qrc
FORMS   += finddialog.ui\
	gotolinedialog.ui \
	replacedialog.ui \
	structdialog.ui \
	filechooser.ui \
	tabbingdialog.ui \
	arraydialog.ui \
	tabdialog.ui \
	letterdialog.ui \
	addoptiondialog.ui \
	quickdocumentdialog.ui \
	usermenudialog.ui \
	usertooldialog.ui \
	refdialog.ui \
	aboutdialog.ui \
	webpublishdialog.ui \
	configdialog.ui \
	spellerdialog.ui
TRANSLATIONS += texmaker_fr.ts \
	texmaker_zh.ts \
	texmaker_ru.ts \
	texmaker_de.ts
################################
unix:!macx {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
isEmpty( PREFIX ) {
    PREFIX=/usr
}
DEFINES += PREFIX=\\\"$${PREFIX}\\\"
target.path = $${PREFIX}/bin
INSTALLS = target
HEADERS	+= x11fontdialog.h
SOURCES	+= x11fontdialog.cpp
FORMS += x11fontdialog.ui
utilities.path = $${PREFIX}/share/texmaker
utilities.files = utilities/blank.png \
	utilities/doc1.png \
	utilities/doc10.png \
	utilities/doc11.png \
	utilities/doc12.png \
	utilities/doc13.png \
	utilities/doc14.png \
	utilities/doc15.png \
	utilities/doc16.png \
	utilities/doc17.png \
	utilities/doc18.png \
	utilities/doc19.png \
	utilities/doc2.png \
	utilities/doc20.png \
	utilities/doc3.png \
	utilities/doc4.png \
	utilities/doc5.png \
	utilities/doc6.png \
	utilities/doc7.png \
	utilities/doc8.png \
	utilities/doc9.png \
	utilities/latexhelp.html \
	utilities/next.gif \
	utilities/next_d.gif \
	utilities/prev.gif \
	utilities/prev_d.gif \
	utilities/psheader.txt \
	utilities/style.css \
	utilities/texmaker_fr.qm \
	utilities/texmaker_ru.qm \
	utilities/texmaker_zh.qm \
	utilities/texmaker_de.qm \
	utilities/up.gif \
	utilities/up_d.gif \
	utilities/usermanual_en.html \
	utilities/usermanual_fr.html \
	utilities/AUTHORS \
	utilities/COPYING \
	utilities/CHANGELOG.txt \
	utilities/texmaker16x16.png \
	utilities/texmaker22x22.png \
	utilities/texmaker32x32.png \
	utilities/texmaker48x48.png \
	utilities/texmaker64x64.png \
	utilities/texmaker128x128.png \
	utilities/texmaker.desktop \
	utilities/texmaker.svg \
	utilities/xpdf_texmaker_sh.txt
INSTALLS += utilities
}
################################
win32 {
INCLUDEPATH += C:\MinGW\include
RC_FILE = win.rc
}
###############################
macx {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
CONFIG += link_prl x86 ppc
QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
target.path = /Applications
INSTALLS = target
utilities.path = Contents/Resources
utilities.files = utilities/blank.png \
	utilities/doc1.png \
	utilities/doc10.png \
	utilities/doc11.png \
	utilities/doc12.png \
	utilities/doc13.png \
	utilities/doc14.png \
	utilities/doc15.png \
	utilities/doc16.png \
	utilities/doc17.png \
	utilities/doc18.png \
	utilities/doc19.png \
	utilities/doc2.png \
	utilities/doc20.png \
	utilities/doc3.png \
	utilities/doc4.png \
	utilities/doc5.png \
	utilities/doc6.png \
	utilities/doc7.png \
	utilities/doc8.png \
	utilities/doc9.png \
	utilities/latexhelp.html \
	utilities/next.gif \
	utilities/next_d.gif \
	utilities/prev.gif \
	utilities/prev_d.gif \
	utilities/psheader.txt \
	utilities/style.css \
	utilities/texmaker_fr.qm \
	utilities/texmaker_ru.qm \
	utilities/texmaker_zh.qm \
	utilities/texmaker_de.qm \
	utilities/up.gif \
	utilities/up_d.gif \
	utilities/usermanual_en.html \
	utilities/usermanual_fr.html \
	utilities/AUTHORS \
	utilities/COPYING \
	utilities/CHANGELOG.txt
QMAKE_BUNDLE_DATA += utilities
INSTALLS += utilities
ICON = texmaker.icns
QMAKE_INFO_PLIST =Info.plist
}
```

It seems like a typical .pro file. Nice...
We 'll use the win32 setup.

### Migrating ###

First we 'll make the typical steps:
  * Fire up Code::Blocks with QtWorkbench (version 0.6.x and up) installed
  * File->New...->Project->Empty Project->Go
  * Fill up what suits your needs. The project folder should be the one you have extracted the sources in. For me it's something like:
![http://y.pagles.googlepages.com/tex-newproject.png](http://y.pagles.googlepages.com/tex-newproject.png)
![http://y.pagles.googlepages.com/tex-newproject2.png](http://y.pagles.googlepages.com/tex-newproject2.png)
  * Press Finish and now you have an empty project. Lets do the typical configuration of the new project before we move on:
  * Project->Properties
  * Mark "This is a custom Makefile"
  * C/C++ parser options tab->Include your Qt headers paths (if you have declared a [Qt global variable](GlobalVariables.md) this will be easy)
![http://y.pagles.googlepages.com/tex-properties-parser.png](http://y.pagles.googlepages.com/tex-properties-parser.png)
  * We 're done with the project properties, press OK to close the dialog
  * Project->Build Options...->Make commands tab->Change everything as in the screenshot:
![http://y.pagles.googlepages.com/tex-buildoptions-make.png](http://y.pagles.googlepages.com/tex-buildoptions-make.png)
  * Press OK and we 're done with the typical settings (In fact this is the way to prepare any new project you need).
  * Let's now check the provided .pro by the project we are trying to import. We see that the files included in the project are (besides sources) .ui files (user interface files from designer), .qrc files (resource files with images etc.) and .ts files (translation files).
  * Project->Add files recursively...
  * Select the folder you have created the project in (in this tutorial C:\Workbench\texmaker-1.6\texmaker\) and press OK.
  * In the next dialog select "Wildcard Select" and add ".qrc", ".ts" and ".ui" files
![http://y.pagles.googlepages.com/tex-fileadd.png](http://y.pagles.googlepages.com/tex-fileadd.png)
  * Press "No" when prompted if you want to clear your previous selection.
  * You should now have all .cpp, .h, .qrc, .ts and .ui files checked in the file addition dialog. Press OK.
  * Project->Properties->Qt Workbench:
![http://y.pagles.googlepages.com/tex-qtproject.png](http://y.pagles.googlepages.com/tex-qtproject.png)
  * Press OK and save your project (File->Save project)
  * We are done! Press Build and voila...

**Note:** I had to comment out the following line (parentmatcher.cpp: 227) to make it build
```
LatexEditor::clearMarkerFormat(block, ParenthesisMatcherPropertyId);
```
this was a static member function of LatexEditor that was commented out. I don't know if this is correct but you can't build this project otherwise...

![http://y.pagles.googlepages.com/tex-success.png](http://y.pagles.googlepages.com/tex-success.png)
  * Project is setup and builds without problems, happy coding!

<sub>Back to</sub> PluginUsage