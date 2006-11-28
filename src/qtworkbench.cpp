/***************************************************************
 * Name:      qtworkbench.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#include <sdk.h>

#include "qtworkbench.h"
#include "qtwprogenerator.h"
#include "qtwprojectoptions.h"

namespace
{
    PluginRegistrant<QtWorkbench> reg(_T("QtWorkbench"));
}

int idQtWbMenuCompile = XRCID("idQtWbMenuCompile");
int idQtWbMenuClean = XRCID("idQtWbMenuClean");
int idQtWbMenuDistClean = XRCID("idQtWbMenuDistClean");
int idQtWbMenuRebuild = XRCID("idQtWbMenuRebuild");
int idQtWbMenuOptions = XRCID("idQtWbMenuOptions");
int idQtWbMenuOptionsEdit = XRCID("idQtWbMenuOptionsEdit");
int idQtWbMenuCompileWorkspace = XRCID("idQtWbMenuCompileWorkspace");
int idQtWbMenuRebuildWorkspace = XRCID("idQtWbMenuRebuildWorkspace");
int idQtWbMenuCleanWorkspace = XRCID("idQtWbMenuCleanWorkspace");
int idQtWbTimer = wxNewId();
int idQtWProcess = wxNewId();

BEGIN_EVENT_TABLE(QtWorkbench, cbCompilerPlugin)
EVT_MENU(idQtWbMenuCompile, QtWorkbench::OnCompile)
EVT_MENU(idQtWbMenuClean, QtWorkbench::OnClean)
EVT_MENU(idQtWbMenuDistClean, QtWorkbench::OnDistClean)
EVT_MENU(idQtWbMenuRebuild, QtWorkbench::OnRebuild)
EVT_MENU(idQtWbMenuCompileWorkspace, QtWorkbench::OnCompileWorkspace)
EVT_MENU(idQtWbMenuCleanWorkspace, QtWorkbench::OnCleanWorkspace)
EVT_MENU(idQtWbMenuRebuildWorkspace, QtWorkbench::OnRebuildWorkspace)
EVT_MENU(idQtWbMenuOptions, QtWorkbench::OnProjectOptions)
EVT_MENU(idQtWbMenuOptionsEdit, QtWorkbench::OnProjectOptionsEdit)
EVT_TIMER(idQtWbTimer, QtWorkbench::OnQtwTimer)
EVT_PIPEDPROCESS_TERMINATED(idQtWProcess, QtWorkbench::OnProcessTerminated)
END_EVENT_TABLE()

QtWorkbench::QtWorkbench()
        : m_Menu(0L),
        m_Process(0L),
        m_Pid(0L),
        m_CurrentAction(NONE),
        m_NextOne(0)
{
    if (!Manager::LoadResource(_T("qtworkbench.zip")))
    {
        NotifyMissingFile(_T("qtworkbench.zip"));
    }
}

QtWorkbench::~QtWorkbench()
{
    //dtor
}

void QtWorkbench::OnAttach()
{}

void QtWorkbench::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

int QtWorkbench::Configure(cbProject* project, ProjectBuildTarget* target)
{
    //create and display the configuration dialog for your plugin
    NotImplemented(_T("QtWorkbench::Configure()"));
    return -1;
}

void QtWorkbench::BuildMenu(wxMenuBar* menuBar)
{
    if (!m_IsAttached)
        return;
    if (m_Menu)
        return;

    m_Menu=Manager::Get()->LoadMenu(_T("qtworkbench_menu"),true);


    int finalPos = 7;
    // wxSmith first - respect :)
    int projMenuPos = menuBar->FindMenu(_("wxSmith")); //if the & is added in the menu item change it
    if (projMenuPos != wxNOT_FOUND)
        finalPos = projMenuPos+1;
    else
    {
        // Place it after debug
        projMenuPos = menuBar->FindMenu(_("&Debug"));
        if (projMenuPos != wxNOT_FOUND)
            finalPos = projMenuPos + 1;
    }

    menuBar->Insert(finalPos, m_Menu, _("&QtWorkbench"));
}

void QtWorkbench::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter "type" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    NotImplemented(_T("QtWorkbench::OfferModuleMenuSpace()"));
}

bool QtWorkbench::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    NotImplemented(_T("QtWorkbench::BuildToolBar()"));
    // return true if you add toolbar items
    return false;
}

int QtWorkbench::Clean()
{
    m_CurrentAction=CLEAN;
    return RunQMakeAndMake();
}

int QtWorkbench::DistClean()
{
    m_CurrentAction=DISTCLEAN;
    return RunQMakeAndMake();
}

int QtWorkbench::Compile()
{
    m_CurrentAction=BUILD;
    return RunQMakeAndMake();
}

int QtWorkbench::Rebuild()
{
    m_CurrentAction=REBUILD;
    return RunQMakeAndMake();
}

int QtWorkbench::CleanWorkspace()
{
    m_CurrentAction=CLEAN_ALL;
    return RunQMakeAndMake();
}

int QtWorkbench::CompileWorkspace()
{
    m_CurrentAction=BUILD_ALL;
    return RunQMakeAndMake();
}

int QtWorkbench::RebuildWorkspace()
{
    m_CurrentAction=REBUILD_ALL;
    return RunQMakeAndMake();
}

void QtWorkbench::OnCompile(wxCommandEvent& event)
{
    // Let's not mess up if the compiler plugin is building something
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    Compile();
}

void QtWorkbench::OnClean(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    Clean();
}

void QtWorkbench::OnDistClean(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    DistClean();
}

void QtWorkbench::OnRebuild(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    Rebuild();
}

void QtWorkbench::OnCompileWorkspace(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    CompileWorkspace();
}

void QtWorkbench::OnCleanWorkspace(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    CleanWorkspace();
}

void QtWorkbench::OnRebuildWorkspace(wxCommandEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
            return;
    }
    RebuildWorkspace();
}

void QtWorkbench::OnQtwTimer(wxTimerEvent& event)
{
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        if (compiler->IsRunning())
        {
            m_Timer.Start(500,wxTIMER_ONE_SHOT);
            return;
        }
    }
    ProjectsArray* allProjects = Manager::Get()->GetProjectManager()->GetProjects();
    if (!allProjects)
    {
        cbMessageBox(_T("QtWorkbench plugin could not recover some of your settings. "
                        "Make sure that your settings are the intendend ones."), _("Error"), wxICON_ERROR);
    }
    else
    {
        for (size_t i = 0; i < allProjects->GetCount(); ++i)
        {
            if (i >= m_prevMakefileState.size() || i >= m_prevMakefileState.size())
            {
                cbMessageBox(_T("QtWorkbench plugin could not recover some of your settings. "
                                "Make sure that your settings are the intendend ones."), _("Error"), wxICON_ERROR);
                break; // The project options should be a mess if we get this.
            }
            cbProject* prj = allProjects->Item(i);
            prj->SetMakefileCustom(m_prevMakefileState[i]);
            prj->SetModified(m_prevModifiedState[i]);

            wxString activeTarget = prj->GetActiveBuildTarget();
            // If there is only one build target I want it to act as if "ALL" build is selected
            if (prj->GetBuildTargetsCount() == 1)
                continue;
            else
            {
                wxString buildCommand;
                // The Makefile produced by qmake names the targets as sub-[target name].
                // I will temporarily change the make command to meet those requirements.
                // This of cource only applies when building/cleaning/whatever a specific target.
                wxArrayString virtuals = prj->GetVirtualBuildTargetGroup(activeTarget);
                if (virtuals.GetCount())
                {
                    for (size_t i=0; i<virtuals.GetCount();i++)
                    {
                        buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcBuild);
                        buildCommand.Replace(_T("sub-$target"),_T("$target"));
                        prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcBuild,buildCommand);

                        buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcClean);
                        buildCommand.Replace(_T("sub-$target"),_T("$target"));
                        prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcClean,buildCommand);

                        buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcDistClean);
                        buildCommand.Replace(_T("sub-$target"),_T("$target"));
                        prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcDistClean,buildCommand);
                    }

                }
                else
                {

                    buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcBuild);
                    buildCommand.Replace(_T("sub-$target"),_T("$target"));
                    prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcBuild,buildCommand);

                    buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcClean);
                    buildCommand.Replace(_T("sub-$target"),_T("$target"));
                    prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcClean,buildCommand);

                    buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcDistClean);
                    buildCommand.Replace(_T("sub-$target"),_T("$target"));
                    prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcDistClean,buildCommand);
                }
            }
        }
    }

    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    if (mbar)
    {
        mbar->Enable(idQtWbMenuCompile,true);
        mbar->Enable(idQtWbMenuClean,true);
        mbar->Enable(idQtWbMenuRebuild,true);
        mbar->Enable(idQtWbMenuDistClean,true);
        mbar->Enable(idQtWbMenuCompileWorkspace,true);
        mbar->Enable(idQtWbMenuRebuildWorkspace,true);
        mbar->Enable(idQtWbMenuCleanWorkspace,true);
    }
}

void QtWorkbench::OnProjectOptions(wxCommandEvent& event)
{
    if (!CurrentActiveProject())
    {
        // There are events signaling that a project has been opened etc.
        // The menu item should start deactivated and activate
        // when an active project exists. For now a messagebox is enough.
        cbMessageBox(_T("Please open/create a project first."), _("Error"), wxICON_ERROR);
        return;
    }
    qtwProjectOptions qtwpo(Manager::Get()->GetAppWindow());
    qtwpo.ShowModal();
}

void QtWorkbench::OnProjectOptionsEdit(wxCommandEvent& event)
{
    cbProject *project = CurrentActiveProject();
    if (!project)
    {
        cbMessageBox(_T("Please open/create a project first."), _("Error"), wxICON_ERROR);
        return;
    }

    m_CurrentAction=NONE;
    if (!RunQMakeAndMake())
    {
        return;
    }

    wxString filename = project->GetTitle();
    filename << _T(".pro");

    ProjectBuildTarget* selectedTarget = CurrentBuildTarget();
    if (selectedTarget)
    {
        filename = selectedTarget->GetTitle();
        filename << wxFileName::GetPathSeparator();
        filename << selectedTarget->GetTitle();
        filename << _T(".pro");
    }

    filename.Prepend(project->GetBasePath());
    Manager::Get()->GetEditorManager()->Open(filename);
}

void QtWorkbench::OnProcessTerminated(CodeBlocksEvent& event)
{
    delete m_Process;
    m_Process = NULL;
    m_Pid = 0L;

    if (m_CurrentAction==NONE)
        return;

    if ((m_CurrentAction == BUILD_ALL || m_CurrentAction == CLEAN_ALL || m_CurrentAction == REBUILD_ALL) && (m_NextOne))
    {
        RunQMakeAndMake();
        return;
    }

    ProjectsArray* allProjects = Manager::Get()->GetProjectManager()->GetProjects();
    if (!allProjects)
    {
        m_CurrentAction=NONE;
        return;
    }

    cbProject* theCurrentActiveProject = CurrentActiveProject();
    if (!theCurrentActiveProject)
    {
        m_CurrentAction=NONE;
        return;
    }

    ProjectBuildTarget* theCurrentTarget = CurrentBuildTarget();

    m_prevMakefileState.clear();
    m_prevModifiedState.clear();

    for (size_t i = 0; i < allProjects->GetCount(); ++i)
    {
        cbProject* prj = allProjects->Item(i);
        m_prevMakefileState.push_back(prj->IsMakefileCustom());
        m_prevModifiedState.push_back(prj->GetModified());
        prj->SetMakefileCustom(true);

        wxString activeTarget = prj->GetActiveBuildTarget();
        // If there is only one build target I want it to act as if "ALL" build is selected
        if (prj->GetBuildTargetsCount() == 1)
            continue;
        else
        {
            wxString buildCommand;
            // The Makefile produced by qmake names the targets as sub-[target name].
            // I will temporarily change the make command to meet those requirements.
            // This of cource only applies when building/cleaning/whatever a specific target.
            wxArrayString virtuals = prj->GetVirtualBuildTargetGroup(activeTarget);
            if (virtuals.GetCount())
            {
                for (size_t i=0; i<virtuals.GetCount();i++)
                {
                    buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcBuild);
                    buildCommand.Replace(_T("$target"),_T("sub-$target"));
                    prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcBuild,buildCommand);

                    buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcClean);
                    buildCommand.Replace(_T("$target"),_T("sub-$target"));
                    prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcClean,buildCommand);

                    buildCommand = prj->GetBuildTarget(virtuals[i])->GetMakeCommandFor(mcDistClean);
                    buildCommand.Replace(_T("$target"),_T("sub-$target"));
                    prj->GetBuildTarget(virtuals[i])->SetMakeCommandFor(mcDistClean,buildCommand);
                }
            }
            else
            {
                buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcBuild);
                buildCommand.Replace(_T("$target"),_T("sub-$target"));
                prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcBuild,buildCommand);

                buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcClean);
                buildCommand.Replace(_T("$target"),_T("sub-$target"));
                prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcClean,buildCommand);

                buildCommand = prj->GetBuildTarget(activeTarget)->GetMakeCommandFor(mcDistClean);
                buildCommand.Replace(_T("$target"),_T("sub-$target"));
                prj->GetBuildTarget(activeTarget)->SetMakeCommandFor(mcDistClean,buildCommand);
            }
        }
    }

    // Yiannis Mandravellos suggestion (modified to meet the plugin's demands) follows
    // find compiler plugin
    cbCompilerPlugin* compiler = TheCompilerPlugin();
    if (compiler)
    {
        // we have our compiler!
        // start building
        switch (m_CurrentAction)
        {
        case BUILD:
            m_CurrentAction=NONE;
            if (theCurrentTarget)
                compiler->Build(theCurrentTarget->GetTitle());
            else
                compiler->Build();
            break;
        case REBUILD:
            m_CurrentAction=NONE;
            if (theCurrentTarget)
                compiler->Rebuild(theCurrentTarget->GetTitle());
            else
                compiler->Rebuild();
            break;
        case CLEAN:
            m_CurrentAction=NONE;
            if (theCurrentTarget)
                compiler->Clean(theCurrentTarget->GetTitle());
            else
                compiler->Clean();
            break;
        case DISTCLEAN:
            m_CurrentAction=NONE;
            if (theCurrentTarget)
                compiler->DistClean(theCurrentTarget->GetTitle());
            else
                compiler->DistClean();
            break;
        case BUILD_ALL:
            m_CurrentAction=NONE;
            compiler->BuildWorkspace();
            break;
        case CLEAN_ALL:
            m_CurrentAction=NONE;
            compiler->CleanWorkspace();
            break;
        case REBUILD_ALL:
            m_CurrentAction=NONE;
            compiler->RebuildWorkspace();
            break;
        default:
            m_CurrentAction=NONE;
            break;
        }
    }

    wxMenuBar* mbar = Manager::Get()->GetAppWindow()->GetMenuBar();
    if (mbar)
    {
        mbar->Enable(idQtWbMenuCompile,false);
        mbar->Enable(idQtWbMenuClean,false);
        mbar->Enable(idQtWbMenuRebuild,false);
        mbar->Enable(idQtWbMenuDistClean,false);
        mbar->Enable(idQtWbMenuCompileWorkspace,false);
        mbar->Enable(idQtWbMenuRebuildWorkspace,false);
        mbar->Enable(idQtWbMenuCleanWorkspace,false);
    }
    m_Timer.SetOwner(this,idQtWbTimer);
    m_Timer.Start(500,wxTIMER_ONE_SHOT);
}

int QtWorkbench::RunQMakeAndMake()
{
    if (m_Pid)
        return 0;

    delete m_Process;
    m_Process = NULL;

    cbProject *theCurrentActiveProject = CurrentActiveProject();

    if (!theCurrentActiveProject)
    {
        cbMessageBox(_T("Please open/create a project first."), _("Error"), wxICON_ERROR);
        m_CurrentAction=NONE;
        m_NextOne=0;
        return 0;
    }

    if (m_CurrentAction == BUILD_ALL || m_CurrentAction == CLEAN_ALL || m_CurrentAction == REBUILD_ALL)
    {
        ProjectsArray* allProjects = Manager::Get()->GetProjectManager()->GetProjects();
        if (!allProjects)
        {
            cbMessageBox(_T("Please open/create a project first."), _("Error"), wxICON_ERROR);
            m_CurrentAction=NONE;
            m_NextOne=0;
            return 0;
        }
        theCurrentActiveProject = allProjects->Item(m_NextOne);
        m_NextOne++;
        if (m_NextOne >= allProjects->GetCount())
            m_NextOne=0;
    }

    wxString QMakeArgument(_T(""));
    wxString theMakefile(_T(""));

    wxSetWorkingDirectory(theCurrentActiveProject->GetBasePath());
    theMakefile = theCurrentActiveProject->GetMakefile();
    qtwProGenerator Generator(theCurrentActiveProject, theCurrentActiveProject->GetCompilerID(), &QMakeArgument);
    if (!Generator.CreatePro())
    {
        cbMessageBox(_T("Could not create .pro file for at least one of the project's "
                        "targets.\n Make sure that you have the QTDIR variable set up correctly and that "
                        "for each target of your project a folder having the name of the target exists under "
                        "the project's folder. If you don't have multiple targets this is not necessery, the "
                        "project's folder will be used instead."), _("Error"), wxICON_ERROR);
        m_CurrentAction=NONE;
        m_NextOne=0;
        return 0;
    }

    wxString cmd;
    //TODO(yop##): Make it user configurable (changing QTDIR allows to swap versions)
    if (!wxGetEnv(_T("QTDIR"),&cmd))
    {
        cbMessageBox(_T("The QTDIR variable is not set."
                        "Please check your Qt installation."), _("Error"), wxICON_ERROR);
        m_CurrentAction=NONE;
        m_NextOne=0;
        return 0;
    }
    cmd << wxFileName::GetPathSeparator() << _T("bin");
    cmd << wxFileName::GetPathSeparator()<< _T("qmake");
    if (theCurrentActiveProject->GetVirtualBuildTargetGroup(theCurrentActiveProject->GetActiveBuildTarget()).GetCount())
    {
        cmd << _T(" CONFIG+=") << theCurrentActiveProject->GetActiveBuildTarget();
    }
    cmd << _T(" ") << QMakeArgument;
    cmd << _T(" -o");
    cmd << _T(" ") << theMakefile;
    if (theCurrentActiveProject->GetVirtualBuildTargetGroup(theCurrentActiveProject->GetActiveBuildTarget()).GetCount())
    {
        cmd << _T(" ") << theCurrentActiveProject->GetActiveBuildTarget();
    }

    // If the user switces compilers with an existing makefile then that makefile is kept and the
    // new compiler is not used. If the old makefile is deleted then the new makefile uses the new one.
    // Should I behind the curtains delete the old makefile?
    m_Process = new PipedProcess((void**)&m_Process,
                                 this,
                                 idQtWProcess,
                                 true,
                                 theCurrentActiveProject->GetBasePath());
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_Process);
    return 1;
}

cbProject* QtWorkbench::CurrentActiveProject()
{
    return Manager::Get()->GetProjectManager()->GetActiveProject();
}

ProjectBuildTarget* QtWorkbench::CurrentBuildTarget()
{
    cbProject* theCurrentActiveProject = CurrentActiveProject();
    if (theCurrentActiveProject)
    {
        wxString activeBuildTarget = theCurrentActiveProject->GetActiveBuildTarget();
        // If there is only one build target I want it to act as if "ALL" build is selected
        if (theCurrentActiveProject->GetBuildTargetsCount() == 1)
            return NULL;
        else
            return theCurrentActiveProject->GetBuildTarget(activeBuildTarget);
    }
    return NULL;
}

cbCompilerPlugin* QtWorkbench::TheCompilerPlugin()
{
    PluginsArray arr = Manager::Get()->GetPluginManager()->GetCompilerOffers();

    if (arr.GetCount() != 1)
    {
        cbMessageBox(_T("More than one compiler plugins!"), _("Error"), wxICON_ERROR);
        return NULL;
    }

    cbCompilerPlugin* compiler = static_cast<cbCompilerPlugin*>(arr[0]);
    return compiler;
}
