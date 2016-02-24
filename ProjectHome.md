## Description ##
A plugin for the Code::Blocks IDE that builds Qt projects using qmake to generate Makefiles and Code::Blocks support for Makefiles to build the project.

The plugin will prepare Makefiles that are compatible with the CodeBlocks build system so all the typical Code::Blocks features are available:

  * Building using the Code::Blocks build commands
  * Debugging
  * Automatically moving to the offending line during build errors

Following the instructions on plugin usage in the wiki you will get an easy way of building and managing your Qt projects using the excellent Code::Blocks IDE.

**The plugin supports the now (Jul '09) latest official C::B release 8.02. The plugin will not be further maintained and the source code repository has been converted to Mercurial enabling the easiest available option for forking to anyone interested in picking it up. Thanks for all the fish.**

## Changes ##
### 0.6.0 ###
  * Advanced view of qmake properties
  * No menu entries. The plugin related dialog is under the project properties
  * Automatic invocation of qmake for projects that are using the plugin ("Run QMake" is gone)
### 0.5.1 ###
  * Handling of qmake operators.
  * Automake linux build.
### 0.5.0 ###
  * .pro parser replacement.
  * Removed the separate "QtWorkbench" menu entry.
  * Gui clean up.
  * Configuration dialog.