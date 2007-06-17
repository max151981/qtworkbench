/***************************************************************
 * Name:      qtworkbench.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#include <sdk.h>
#include <annoyingdialog.h>

#include "qtworkbench.h"
#include "qtwprogenerator.h"
#include "qtwprojectoptions.h"
#include "qtwconfiguration.h"

namespace
{
    PluginRegistrant<QtWorkbench> reg(_T("QtWorkbench"));
}

int idQtWbMenuOptions = wxNewId();
int idQtWbMenuOptionsEdit = wxNewId();
int idQtWbMenuRunQMake = wxNewId();
int idQtWProcess = wxNewId();

BEGIN_EVENT_TABLE(QtWorkbench, cbPlugin)
EVT_MENU(idQtWbMenuOptions, QtWorkbench::OnProjectOptions)
EVT_MENU(idQtWbMenuOptionsEdit, QtWorkbench::OnProjectOptionsEdit)
EVT_MENU(idQtWbMenuRunQMake, QtWorkbench::OnRunQMake)
EVT_PIPEDPROCESS_TERMINATED(idQtWProcess, QtWorkbench::OnProcessTerminated)
END_EVENT_TABLE()

QtWorkbench::QtWorkbench():
        m_Process(0L),
        m_Pid(0L)
{
    if (!Manager::LoadResource(_T("QtWorkbench.zip")))
    {
        NotifyMissingFile(_T("QtWorkbench.zip"));
    }
}

QtWorkbench::~QtWorkbench()
{
    //dtor
}

void QtWorkbench::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void QtWorkbench::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

int QtWorkbench::Configure()
{
    //create and display the configuration dialog for your plugin
    return 0;
}

cbConfigurationPanel* QtWorkbench::GetConfigurationPanel(wxWindow* parent)
{
    qtwConfiguration* dlg = new qtwConfiguration(parent);
    // deleted by the caller

    return dlg;
}

void QtWorkbench::BuildMenu(wxMenuBar* menuBar)
{
    if (!m_IsAttached)
        return;

    int projectMenuPos = menuBar->FindMenu(_("&Project"));
    wxMenu* projectMenu = menuBar->GetMenu(projectMenuPos);
    projectMenu->AppendSeparator();
    projectMenu->Append(idQtWbMenuOptions, wxT("Qt project options..."));
    projectMenu->Append(idQtWbMenuRunQMake, wxT("Run qmake"));

    // TODO Later enable this. The parser must be of proven quality first
    // projectMenu->Append(idQtWbMenuOptionsEdit, wxT("Edit Qt project options"));
}

void QtWorkbench::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    NotImplemented(_T("QtWorkbench::BuildModuleMenu()"));
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

    wxString filename = project->GetBasePath();
    filename << wxFileName::GetPathSeparator();
    filename << project->GetActiveBuildTarget();
    filename << wxT(".pro");

    if(wxFile::Exists(filename)){
        Manager::Get()->GetEditorManager()->Open(filename);
    }
}

void QtWorkbench::OnProcessTerminated(CodeBlocksEvent& event)
{
    delete m_Process;
    m_Process = NULL;
    m_Pid = 0L;

    if(!m_TargetNames.GetCount()){
        return;
    }

    wxString theMakefile = CurrentActiveProject()->GetMakefile();
    wxRemoveFile(CurrentActiveProject()->GetBasePath() + wxFileName::GetPathSeparator() + theMakefile);
    wxString cmd = QMakeCommand();
    cmd += theMakefile;
    cmd += wxT(".");
    cmd += m_TargetNames[0];
    cmd += wxT(" ");
    cmd += m_TargetNames[0];
    cmd += wxT(".pro");

    m_TargetNames.RemoveAt(0);

    m_Process = new PipedProcess((void**)&m_Process,
                                 this,
                                 idQtWProcess,
                                 true,
                                 CurrentActiveProject()->GetBasePath());
    m_Pid = wxExecute(cmd, wxEXEC_ASYNC, m_Process);
}

void QtWorkbench::OnRunQMake(wxCommandEvent& event)
{
    if (m_Pid)
        return;

    delete m_Process;
    m_Process = NULL;

    cbProject *theCurrentActiveProject = CurrentActiveProject();

    if (!theCurrentActiveProject)
    {
        cbMessageBox(_T("Please open/create a project first."), _("Error"), wxICON_ERROR);
        return;
    }

    qtwProGenerator Generator(theCurrentActiveProject);
    if (!Generator.CreatePro())
    {
        cbMessageBox(_T("Could not create .pro file for at least one of the project's targets."), _("Error"), wxICON_ERROR);
        return;
    }

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("qtwb"));

    wxString qdir = cfg->Read(_T("/QtDir"));
    if (qdir.IsEmpty())
    {
        if (!wxGetEnv(_T("QTDIR"),&qdir))
        {
            AnnoyingDialog dlg( _("qmake location could not be establised"),
                                _("You have not specified a Qt installation directory"
                                   "and the QTDIR environmental variable is not set."
                                   "If qmake is also not located in a directory listed in"
                                   "your PATH environmental variable then invoking qmake will fail."),
                                wxART_INFORMATION,AnnoyingDialog::OK,wxID_OK);
            dlg.ShowModal();
            // We will try to run qmake anyway...
        }
    }

    m_TargetNames.Clear();
    int targetsCount = theCurrentActiveProject->GetBuildTargetsCount();
    for (int i = 0; i < targetsCount; ++i)
    {
        ProjectBuildTarget* target = theCurrentActiveProject->GetBuildTarget(i);
        m_TargetNames.Add(target->GetTitle());
    }

    CodeBlocksEvent mockEvent;
    OnProcessTerminated(mockEvent);
}

cbProject* QtWorkbench::CurrentActiveProject()
{
    return Manager::Get()->GetProjectManager()->GetActiveProject();
}

wxString QtWorkbench::QMakeCommand(){
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("qtwb"));
    wxString cmd = cfg->Read(_T("/QtDir"));
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(cmd);
    if (!cmd.IsEmpty())
    {
        cmd << wxFileName::GetPathSeparator() << _T("bin") << wxFileName::GetPathSeparator();
    }

    cmd << _T("qmake -makefile");

    wxString QMakeSpec = cfg->Read(_T("/QMkSpec"));

    if (!QMakeSpec.IsEmpty())
    {
        cmd << _T(" -spec ") << QMakeSpec;
        // else the QMAKESPEC env var will be used which is ok.
    }
    cmd << _T(" -o ");
    return cmd;
}
