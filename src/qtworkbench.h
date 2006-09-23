/***************************************************************
 * Name:      qtworkbench.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#ifndef QTWORKBENCH_H
#define QTWORKBENCH_H

#if defined(__GNUG__) && !defined(__APPLE__)
 #pragma interface "qtworkbench.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
 #pragma hdrstop
#endif

#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include <sdk_events.h>

class SimpleTextLog;

class QtWorkbench : public cbPlugin
{
public:
    QtWorkbench();
    ~QtWorkbench();
    virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L);
    void BuildMenu(wxMenuBar* menuBar);
    void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data);
    bool BuildToolBar(wxToolBar* toolBar);
    int Clean();
    int DistClean();
    int Compile();
    int Rebuild();
    int CleanWorkspace();
    int CompileWorkspace();
    int RebuildWorkspace();

    void OnAttach();
    void OnRelease(bool appShutDown);
    void OnCompile(wxCommandEvent& event);
    void OnClean(wxCommandEvent& event);
    void OnDistClean(wxCommandEvent& event);
    void OnRebuild(wxCommandEvent& event);
    void OnCompileWorkspace(wxCommandEvent& event);
    void OnCleanWorkspace(wxCommandEvent& event);
    void OnRebuildWorkspace(wxCommandEvent& event);
    void OnProjectOptions(wxCommandEvent& event);
    void OnProjectOptionsEdit(wxCommandEvent& event);
    void OnProcessTerminated(CodeBlocksEvent& event);
    void OnQtwTimer(wxTimerEvent& event);

    int RunQMakeAndMake();
    cbProject* CurrentActiveProject();
    ProjectBuildTarget* CurrentBuildTarget();
    cbCompilerPlugin* TheCompilerPlugin();
protected:
private:
    enum QtWActions{NONE=0,BUILD,REBUILD,CLEAN,DISTCLEAN,BUILD_ALL,REBUILD_ALL,CLEAN_ALL};
    wxMenu* m_Menu;
    wxProcess* m_Process;
    wxTimer m_Timer;
    std::vector<bool> m_prevMakefileState;
    std::vector<bool> m_prevModifiedState;
    long m_Pid;
    int m_CurrentAction;
    size_t m_NextOne;
    DECLARE_EVENT_TABLE()
};

#endif // QTWORKBENCH_H
