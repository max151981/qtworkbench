<sub>Back to</sub> PluginUsage

# Creating a project #
In the following we will use some of the examples shipped with any Qt installation so it is easy to follow along. Also I have assumed that you have already set up a Code::Blocks global variable for your Qt installation (see GlobalVariables). If you haven't then in the following just replace $(#qt) with your Qt installation directory (i.e. if you see $(#qt.include) you should input <your path to Qt>\include I hope it's clear).

  1. File->New->Project->Empty Project->Go
  1. Fill your project name (in the following assume that it is named "dialogs") and the path to create it in.
  1. In the next wizard screen select only the Release target and your compiler (make sure it matches the one you configured in mkspecs, see PluginConfiguration). Fill in your target's name (assume complexwizard in our example).
  1. This step is only needed to follow our example: Under your Qt installation directory there is a folder named "examples". In there you will be able to find another folder named "dialogs". Copy all of the folders underneath the last one (as a hint one of these should be named "complexwizard") to your newly created project's folder. Go through all the directories and remove the files with the extension ".pro" (you don't actually need to do this but we want the plugin to generate these files don't we?)
  1. Add any files you need to your project. The plugin will automatically identify user interface files, translation files etc. In our example: Project->Add files recursively...-> Select the "complexwizard" under the project's directory -> OK -> OK
  1. Because our example is a gui application go to Project->Properties->Build Targets... and change the "Type:" of the target to GUI application.
  1. Set the project to use custom Makefiles: Project->Properties->Select the "This is a custom Makefile" check box and press OK.
  1. Add the Qt header folders to the parser paths (you need code completion don't you?): Project->Properties->C/C++ parser options and add the following: $(#qt.include) $(#qt.include)/QtCore $(#qt.include)/QtGui You will have to close and reopen the project  for the changes to take effect. When you reopen the project try out the code completion of Qt classes.
  1. Under Project->Build Options... remove all the compiler flags for the target you created (that would be "complexwizard" in our example)
  1. This is a bit tricky: Project->Build Options... Now for both the project and the target select "Make" Commands and make them match the following screen shot (at the end of the page they are provided as plain text for easy copy-pasting):

![http://y.pagles.googlepages.com/make_commands.png](http://y.pagles.googlepages.com/make_commands.png)

As promised the following are the values from the screen shot:

$make -f $makefile.$target

$make -f $makefile $file

$make -f $makefile.$target clean

$make -f $makefile.$target distclean

Your project is ready to use the plugin for building Qt projects.

<sub>Back to</sub> PluginUsage