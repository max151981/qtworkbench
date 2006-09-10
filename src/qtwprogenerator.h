/***************************************************************
 * Name:      qtwprogenerator.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#ifndef PROGENERATOR_H
#define PROGENERATOR_H

#include <settings.h>
#include <cbproject.h>
#include <compiler.h>
#include <wx/dir.h>
#include "qtworkbench.h"

WX_DEFINE_ARRAY(ProjectBuildTarget*, Targets_Array);
WX_DEFINE_ARRAY(ProjectFile*, Files_Array); // keep our own copy, to sort it by file weight (priority)
WX_DEFINE_ARRAY(ProjectFile*, ObjectFiles_Array); // holds object files already included in the Makefile

class MkSpecBrowser : public wxDirTraverser {
public:
    MkSpecBrowser(wxArrayString& dirs) : mkspec_dirs(dirs) {}

    virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename)) {
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& dirname) {
        mkspec_dirs.Add(dirname);
        return wxDIR_CONTINUE;
    }

private:
    wxArrayString& mkspec_dirs;
};

class qtwProGenerator {
public:
    qtwProGenerator(cbProject*, wxString, wxString*);
    ~qtwProGenerator();
    bool CreatePro();
    static void PrependHeader(wxString&);
    static void AddOrReplace(wxString&, wxString, wxString, wxString, bool force=true);
    static void Remove(wxString&, wxString, wxString);
    static wxString GetArgsFor(const wxString &, wxString);
private:
    void DoPrepareFiles();
    void DoPrepareValidTargets();

    void UpdateCompiler(ProjectBuildTarget*);
    bool IsTargetValid(ProjectBuildTarget*);

    void QuoteStringIfNeeded(wxString&, bool force=true);

    wxString MkspecToUse();
    void DoAddCommonVariables(wxString&, ProjectBuildTarget*);
    void DoAppendCompilerOptions(wxString& cmd, ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendLinkerOptions(wxString& cmd, ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendLinkerLibs(wxString& cmd, ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendIncludeDirs(wxString& cmd, ProjectBuildTarget* target = 0L, const wxString& prefix = _T("-I"), bool useGlobalOptions = false);
    void DoAppendResourceIncludeDirs(wxString& cmd, ProjectBuildTarget* target = 0L, const wxString& prefix = _T("-I"), bool useGlobalOptions = false);
    void DoAppendLibDirs(wxString& cmd, ProjectBuildTarget* target = 0L, const wxString& prefix = _T("-L"), bool useGlobalOptions = false);
    void DoAddProOptions(wxString&,ProjectBuildTarget* target);
    void DoAddTargetFiles(wxString&, ProjectBuildTarget*);
    void DoAddConfiguration(wxString&, ProjectBuildTarget*);

    void DoAddSubdirToProjectPro(wxString &,wxString &);
    void DoPrependSubdirsConfiguration(wxString &);

    Compiler* m_CompilerSet;
    cbProject* m_Project;
    wxString *m_Arguments;
    wxString m_Configuration;
    Targets_Array m_LinkableTargets;
    Files_Array m_Files;
    ObjectFiles_Array m_ObjectFiles;
    bool m_GeneratingPro;
};
#endif // PROGENERATOR_H
