<sub>Back to</sub> PluginUsage

# Generating Makefiles #

We assume that you have a project that fulfills the requirements of the plugin (see QtWorkbenchProjects). For illustration we 'll continue with our example introduced in QtWorkbenchProjects.
The only things that have been added by the plugin to the usual Code::Blocks user interface is a new tab in the project properties dialog (accessible through Project->Properties or by right clicking the respective project in the management tab) named Qt Workbench. For a newlly created project you can set all the typical project settings using the respective Code::Blocks controls and options (like compiler flags, source files etc.). QtWorkbench will pick up these settings and apply them to the generated .pro file for each target of your project. The Qt specific options must be inserted through the dialog provided by the QtWorkbench plugin. Select Project->Properties and navigate to the Qt Workbench tab to access it. For a project that will use QtWorkbench you have to check the "This project uses Qt Workbench" checkbox in this dialog. For the example we introduced in QtWorkbenchProjects the Qt options could look like the following:

![http://y.pagles.googlepages.com/qt_configuration.png](http://y.pagles.googlepages.com/qt_configuration.png)

After pressing OK, you are ready to build your project. Also note that although we created a "Release" target when we created the project we can set our target to be a "Release" or "Debug" target from the Qt project options dialog. This is because the kind of target is handled by qmake and not by Code::Blocks.
From this point on you can use Code::Blocks as you would usually do. To build your project use the Code::Blocks menu entry, or toolbar button, or keyboard shortcut, the same applies for debbuging or running your target, everything is available.

<sub>Back to</sub> PluginUsage