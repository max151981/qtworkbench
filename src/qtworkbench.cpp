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
int idQtWProcess = wxNewId();

BEGIN_EVENT_TABLE(QtWorkbench, cbPlugin)
EVT_MENU(idQtWbMenuOptions, QtWorkbench::OnProjectOptions)
EVT_MENU(idQtWbMenuOptionsEdit, QtWorkbench::OnProjectOptionsEdit)
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
    // Register to get notified with the cbEVT_COMPILER_STARTED event.
    // We will use this event to automatically invoke qmake before the
    // build process.
    Manager::Get()->RegisterEventSink(cbEVT_COMPILER_STARTED, new cbEventFunctor<QtWorkbench, CodeBlocksEvent>(this, &QtWorkbench::OnBuildStarted));
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
    QtWConfiguration* dlg = new QtWConfiguration(parent);
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
    QtWProjectOptions qtwpo(Manager::Get()->GetAppWindow());
    PlaceWindow(&qtwpo);
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

    wxString theMakefile = CurrentActiveProject()->GetMakefile() + wxT(".") + m_TargetNames[0];
    wxRemoveFile(CurrentActiveProject()->GetBasePath() + wxFileName::GetPathSeparator() + theMakefile);
    wxString cmd = QMakeCommand();
    cmd += theMakefile;
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

void QtWorkbench::RunQMake(){

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

    QtWProGenerator Generator(theCurrentActiveProject);
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
                                _("You have not specified a Qt installation directory\n"
                                   "and the QTDIR environmental variable is not set.\n"
                                   "If qmake is also not located in a directory listed in\n"
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

void QtWorkbench::OnBuildStarted(CodeBlocksEvent& event){

    RunQMake();

    while(m_Pid){
        wxMilliSleep(10);
        Manager::Yield();
    }
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
