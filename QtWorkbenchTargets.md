<sub>Back to</sub> PluginUsage

# Adding new targets #

By now you should have a project with one target that you can fully use with QtWorkbench. If not then you should start with QtWorkbenchProjects and BuildingQtProjects to get there. The example introduced in QtWorkbenchProjects is used through this page as well.
  1. Go to File->New->Build Target... and select "Console" -> Go. Just add the new target name and press "Finish". For our example create the "configdialog" target.
  1. Under Build Options... change the "Make Commands" the new target to match the ones of your existing target.
  1. Create or add new files to your new target. For our example "Add Files recursively..." and select the "configdialog" folder. You will have to select the file "configdialog.qrc" to be included in the project as it is not selected by default. Set the added files to belong to the "configdialog" target when asked. Check under Project->Properties->Build Targets that the correct files are in the new target (Code::Blocks kept adding the files of the existing target in my new target, if you see something like that you have to uncheck the "Using custom Makefiles" check box, correct the files belonging to the new target and then check the check box again, oh well...).
  1. Now you just have to follow the same steps as in BuildingQtProjects for the new target.

To add even more targets you just have to follow the steps above for every target you have to add.

<sub>Back to</sub> PluginUsage