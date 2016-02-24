# Installation instructions #

## Windows users ##
If you are updating from a release < 0.5 then remove all the plugin related files from your Code::Blocks installation. This can be done manually or using the uninstall plugin option from the Code::Blocks plugin manager.

Use the [supplied binary plugin](http://code.google.com/p/qtworkbench/downloads/list) with the stable [Code::Blocks release](http://www.codeblocks.org/downloads/5#windows) or a recent [Code::Blocks nightly build](http://forums.codeblocks.org/index.php/board,20.0.html) (0.6.0 has been successfully installed in Feb 13 2008 night build):
Go to Plugins->Manage Plugins->Install New and select the file you downloaded. The plugin should be immediately available. For now binary releases are only provided for Windows, do not use the binary distribution of the plugin in a linux installation of Code::Blocks.

## Linux Users ##

### Setting up you build environment: ###
Before continuing you must make sure that you have setup your build environment in a way that you can actually build Code::Blocks from sources. This is well documented in the [Code::Blocks wiki](http://wiki.codeblocks.org/index.php?title=Installing_Code::Blocks#Linux). Please take the time to check if the requirements to build Code::Blocks are met. Don't follow the whole process of building Code::Blocks, just make sure that you have installed the prerequisites (wxWidgets - wxGTK, preferably version 2.8.4) on your linux distribution.

### Long story short: ###
```
svn checkout svn://svn.berlios.de/codeblocks/trunk codeblocks
cd codeblocks/
wget http://qtworkbench.googlecode.com/files/QtWorkbench-src-0.5.1.tar.gz
tar zxf QtWorkbench-src-0.5.1.tar.gz
patch --unified --strip=0 --forward --input=qtworkbench.patch
./bootstrap
./configure --prefix=/opt/codeblocks-svn --with-contrib-plugins=qtworkbench
make
sudo make install
/opt/codeblocks-svn/bin/codeblocks
```

### Step by step explanation: ###
  * **svn checkout svn://svn.berlios.de/codeblocks/trunk codeblocks** will get you a fresh copy of the Code::Blocks sources from the project's source repository.
  * **cd codeblocks/** we change our working directory to the one we downloaded the Code::Blocks sources in.
  * **wget http://qtworkbench.googlecode.com/files/QtWorkbench-src-0.5.1.tar.gz** we download (you will need to have [wget](http://www.gnu.org/software/wget/) installed something that is generally true for a typical linux installation) the QtWorkbench sources  for release 0.5.1 along with a patch to the Code::Blocks build process that enables building QtWorkbench
  * **tar zxf QtWorkbench-src-0.5.1.tar.gz** we unarchive the sources
  * **patch --unified --strip=0 --forward --input=qtworkbench.patch** we apply the patch to the Code::Blocks build process
  * **./bootstrap** we start the typical Code::Blocks build process
  * **./configure --prefix=/opt/codeblocks-svn --with-contrib-plugins=qtworkbench** we configure the Code::Blocks build process to: a) install Code::Blocks under /opt/codeblocks-svn and b) build QtWorkbench (suggested to replace --with-contrib-plugins=qtworkbench with --with-contrib-plugins=all in order to enjoy all the contributed Code::Blocks plugins)
  * **make** builds everything (will take a while so be patient)
  * **sudo make install** installs to wherever --prefix points to when we invoked ./configure
  * **/opt/codeblocks-svn/bin/codeblocks** runs our shiny new Code::Blocks installation, with QtWorkbench included.