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
#include "qtworkbench.h"

WX_DEFINE_ARRAY(ProjectBuildTarget*, Targets_Array);
WX_DEFINE_ARRAY(ProjectFile*, Files_Array); // keep our own copy, to sort it by file weight (priority)
WX_DEFINE_ARRAY(ProjectFile*, ObjectFiles_Array); // holds object files already included in the Makefile

class QtWProjectHandler;

class qtwProGenerator {
public:
    qtwProGenerator(cbProject*);
    ~qtwProGenerator();
    bool CreatePro();

private:
    void DoPrepareFiles();
    void DoPrepareValidTargets();

    void UpdateCompiler(ProjectBuildTarget*);
    bool IsTargetValid(ProjectBuildTarget*);

    void QuoteStringIfNeeded(wxString&, bool force=true);

    wxString MkspecToUse();
    void DoAddCommonVariables(ProjectBuildTarget*);
    void DoAppendCompilerOptions(ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendLinkerOptions(ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendLinkerLibs(ProjectBuildTarget* target = 0L, bool useGlobalOptions = false);
    void DoAppendIncludeDirs(ProjectBuildTarget* target = 0L, const wxString& prefix = _T("-I"), bool useGlobalOptions = false);
    void DoAppendLibDirs(ProjectBuildTarget* target = 0L, const wxString& prefix = _T("-L"), bool useGlobalOptions = false);
    void DoAddProOptions(ProjectBuildTarget* target);
    void DoAddTargetFiles(ProjectBuildTarget*);

    Compiler* m_CompilerSet;
    cbProject* m_Project;
    wxArrayString m_Configuration;
    Targets_Array m_LinkableTargets;
    Files_Array m_Files;
    ObjectFiles_Array m_ObjectFiles;
    QtWProjectHandler *m_Handler;
};
#endif // PROGENERATOR_H
