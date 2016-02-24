<sub>Back to</sub> PluginUsage

# Adding global variables #

Since Code::Blocks allows the usage of custom variables around the project, let's use them. Although this is not necessary to run the plugin it makes our lives easier so it is recommended, since if you install a new Qt version you only have to change one thing and the changes will take effect all around Code::Blocks. Navigate to Settings->Global Variables... and insert a new variable naming it as you wish (mine is "qt"), also add a User-defined field named "mkspecs" (or whatever you wish as long as you remember this). The field "base" should point to your current top level Qt installation dir (or the folder that contains the bin directory containing qmake) while the mkspecs variable should point to the respective mkspecs directory for your Qt installation (generally the mkspecs folder under your Qt installation directory). To give you a hint this is how mine looks like:

![http://y.pagles.googlepages.com/global_variables.png](http://y.pagles.googlepages.com/global_variables.png)

<sub>Back to</sub> PluginUsage