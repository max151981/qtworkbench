/***************************************************************
 * Name:      qtworkbench.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#include <sdk.h>
#include <annoyingdialog.h>
#include <projectloader_hooks.h>

#include "qtworkbench.h"
#include "qtwprogenerator.h"
#include "qtwprojectoptions.h"
#include "qtwconfiguration.h"

namespace
{
    PluginRegistrant<QtWorkbench> reg(_T("QtWorkbench"));
}

int idQtWProcess = wxNewId();

BEGIN_EVENT_TABLE(QtWorkbench, cbPlugin)
    EVT_PIPEDPROCESS_TERMINATED(idQtWProcess, QtWorkbench::OnProcessTerminated)
END_EVENT_TABLE()

QtWorkbench::QtWorkbench():
        m_Process(0L),
        m_HookId(0L),
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

    // Hook to project loading procedure. We will use this to verify
    // that QtWorkbench is actually used for the project so we start
    // qmake automaticaly whe the user invokes the build procedure
    ProjectLoaderHooks::HookFunctorBase* myhook = new ProjectLoaderHooks::HookFunctor<QtWorkbench>(this, &QtWorkbench::OnProjectLoadingHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(myhook);

}

void QtWorkbench::OnRelease(bool appShutDown)
{
    if (m_HookId)
    {
        ProjectLoaderHooks::UnregisterHook(m_HookId, true);
    }
}

int QtWorkbench::Configure()
{
    //create and display the configuration dialog for your plugin
    return 0;
}

cbConfigurationPanel* QtWorkbench::GetConfigurationPanel(wxWindow* parent)
{
    QtWConfiguration* dlg = new QtWConfiguration(parent);
    return dlg;
}

cbConfigurationPanel* QtWorkbench::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    QtWProjectOptions* dlg = new QtWProjectOptions(parent, project, m_EnabledProjects);
    return dlg;
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

void QtWorkbench::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading)
{
    TiXmlElement* node = elem->FirstChildElement("qtworkbench");
    if (loading)
    {
        bool enabled = false;
        if (m_EnabledProjects.find(project->GetFilename()) != m_EnabledProjects.end())
        {
            enabled = m_EnabledProjects[project->GetFilename()];
        }
        else if (node)
        {
            TiXmlElement* enabledElem = node->FirstChildElement("enabled");
            if (enabledElem->Attribute("value"))
            {
                enabled = (cbC2U("true") == cbC2U(enabledElem->Attribute("value")));
            }
        }

        // The C::B project filename is the only unique
        // way to identify the different projects.
        m_EnabledProjects[project->GetFilename()] = enabled;
    }
    else
    {
        if (!node)
        {
            if (m_EnabledProjects[project->GetFilename()])
            {
                // Create the node only if needed (project actually uses the plugin)
                node = elem->InsertEndChild(TiXmlElement("qtworkbench"))->ToElement();
            }
        }

        if (node)
        {
            // The node was already there or has just been created because the
            // project uses the plugin.

            node->Clear();

            TiXmlElement* enabledElem = node->InsertEndChild(TiXmlElement("enabled"))->ToElement();
            enabledElem->SetAttribute("value", m_EnabledProjects[project->GetFilename()] ? cbU2C(wxT("true")) : cbU2C(wxT("false")));
            m_EnabledProjects.erase(project->GetFilename());
        }
    }
}

void QtWorkbench::OnProcessTerminated(CodeBlocksEvent& event)
{
    delete m_Process;
    m_Process = NULL;
    m_Pid = 0L;

    if (!m_TargetNames.GetCount())
    {
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

void QtWorkbench::RunQMake()
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

    // Is this a good approach?
    CodeBlocksEvent mockEvent;
    OnProcessTerminated(mockEvent);
}

void QtWorkbench::OnBuildStarted(CodeBlocksEvent& event)
{
    // Do nothing is the project is not using QtWorkbench
    if (!m_EnabledProjects[CurrentActiveProject()->GetFilename()])
    {
        return;
    }
    RunQMake();

    while (m_Pid)
    {
        wxMilliSleep(10);
        Manager::Yield();
    }
}

cbProject* QtWorkbench::CurrentActiveProject()
{
    return Manager::Get()->GetProjectManager()->GetActiveProject();
}

wxString QtWorkbench::QMakeCommand()
{
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
