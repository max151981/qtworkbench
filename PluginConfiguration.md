<sub>Back to</sub> PluginUsage

# Configuring the plugin #

The plugin provides a simple configuration dialog. This is accessible through Settings->Environment...->QtWorkbench. The only things that have to be configured are the Qt installation path and the path to the available mkspecs (Qt per compiler and OS "make" specifications). The later is generally a folder named "mkspecs" under the Qt installation directory. The most flexible way to configure this would be having already set up a Code::Blocks global variable (see GlobalVariables). If the global variable you have set up is named "qt" and you have the "mkspecs" user-defined variable defined then the configuration dialog would look like the following:

![http://y.pagles.googlepages.com/configuration.png](http://y.pagles.googlepages.com/configuration.png)

The "default" mkspec is generaly a good choice. If you need something different then select it from the drop down list. If you don't have a global variable defined then just provide manually the paths to your Qt installation and the respective mkspecs directory.

<sub>Back to</sub> PluginUsage