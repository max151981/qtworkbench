/***************************************************************
 * Name:      qtwprogenerator.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#ifndef __WXMSW__
#include <sys/utsname.h>
#endif
#include <wx/tokenzr.h>
#include <sdk.h>

#include "qtwprojecthandler.h"
#include "qtwprogenerator.h"

qtwProGenerator::qtwProGenerator(cbProject* project)
        :m_Project(project)
{
    m_Handler = new QtWProjectHandler;
    m_CompilerSet=CompilerFactory::GetCompiler(project->GetCompilerID());
}

qtwProGenerator::~qtwProGenerator()
{
    delete m_Handler;
}

void qtwProGenerator::DoPrepareFiles()
{
    m_Files.Clear();
    int filesCount = m_Project->GetFilesCount();
    for (int i = 0; i < filesCount; ++i)
    {
        ProjectFile* pf = m_Project->GetFile(i);
        m_Files.Add(pf);
    }
}

void qtwProGenerator::DoPrepareValidTargets()
{
    m_LinkableTargets.Clear();
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            continue;

        // create link target only if it has at least one linkable file
        bool hasFiles = false;
        int filesCount = m_Files.GetCount();
        for (int i = 0; i < filesCount; ++i)
        {
            ProjectFile* pf = m_Files[i];
            if (pf->link && pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                hasFiles = true;
                break;
            }
        }
        if (hasFiles)
            m_LinkableTargets.Add(target);
    }
}

void qtwProGenerator::UpdateCompiler(ProjectBuildTarget* target)
{
    wxString compID = target
                      ? target->GetCompilerID()
                      : (m_Project ? m_Project->GetCompilerID() : _T(""));
    if (!compID.IsEmpty())
        m_CompilerSet = CompilerFactory::GetCompiler(compID);
    else
        m_CompilerSet = CompilerFactory::GetDefaultCompiler();
}

bool qtwProGenerator::IsTargetValid(ProjectBuildTarget* target)
{
    UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return false;
    bool hasBin = target->GetTargetType() != ttCommandsOnly; // is not "commands-only" target
    bool hasCmds = !target->GetCommandsAfterBuild().IsEmpty() ||
                   !target->GetCommandsBeforeBuild().IsEmpty();
    return hasBin && (hasCmds || m_LinkableTargets.Index(target) != -1);
}

void qtwProGenerator::QuoteStringIfNeeded(wxString& str, bool force)
{
    if (!force)
        return;
    if (m_CompilerSet->GetSwitches().forceCompilerUseQuotes ||
            m_CompilerSet->GetSwitches().forceLinkerUseQuotes ||
            (str.Find(' ') != -1 && str.GetChar(0) != '"'))
    {
        str = _T('"') + str + _T('"');
    }
}

void qtwProGenerator::DoAddCommonVariables(ProjectBuildTarget* target)
{
    if (!IsTargetValid(target))
        return;
    if (!m_CompilerSet)
        return;

    wxArrayString values;
    if (m_Handler->Contains(wxT("CONFIG"),wxT("plugin")))
    {
        values.Add(wxT("lib"));
        m_Handler->SetValuesFor(wxT("TEMPLATE"),values);

        m_Configuration.Add(wxT("staticlib"));
        m_Configuration.Add(wxT("dll"));
    }

    switch (target->GetTargetType())
    {
    case ttExecutable :
#ifdef __WXMSW__
        m_Configuration.Add(wxT("windows"));
#endif
        values.Add(wxT("app"));
        m_Handler->SetValuesFor(wxT("TEMPLATE"),values);
        break;
    case ttConsoleOnly :
        m_Configuration.Add(wxT("console"));
        values.Add(wxT("app"));
        m_Handler->SetValuesFor(wxT("TEMPLATE"),values);
        break;
    case ttStaticLib :
        m_Configuration.Add(wxT("staticlib"));
        values.Add(wxT("lib"));
        m_Handler->SetValuesFor(wxT("TEMPLATE"),values);
        break;
    case ttDynamicLib :
        m_Configuration.Add(wxT("dll"));
        values.Add(wxT("lib"));
        m_Handler->SetValuesFor(wxT("TEMPLATE"),values);
        break;
    case ttCommandsOnly :
    case ttNative :
        break;
    }

    wxString thePath = m_Project->GetBasePath();
    thePath << target->GetObjectOutput();
    QuoteStringIfNeeded(thePath);
    values.Clear();
    values.Add(thePath);
    m_Handler->SetValuesFor(wxT("OBJECTS_DIR"),values);

    wxFileName out(target->GetOutputFilename());
    thePath = m_Project->GetBasePath();
    thePath << out.GetPath(wxPATH_GET_VOLUME);
    QuoteStringIfNeeded(thePath);

    values.Clear();
    values.Add(thePath);
    m_Handler->SetValuesFor(wxT("DESTDIR"),values);

    values.Clear();
    values.Add(out.GetName());
    m_Handler->SetValuesFor(wxT("TARGET"),values);
}

/*wxString qtwProGenerator::MkspecToUse()
{
    wxString MkspecString;

    wxGetEnv(_T("QMAKESPEC"),&MkspecString); // Not present in every installation

    if (!m_Project)
        return wxString(_T(""));

    Compiler* compilerSet = CompilerFactory::GetCompiler(m_Project->GetCompilerID());

    //qmkspecs are generally labeled using the compiler executable filenames
    wxFileName CPPCompilerFilename(compilerSet->GetPrograms().CPP);
    wxFileName CCompilerFilename(compilerSet->GetPrograms().C);

    //QMAKESPEC envinroment variable holds the default mkspec variable
    wxString CPPCompilerFileString = CPPCompilerFilename.GetName().Lower();
    wxString CCompilerFileString = CCompilerFilename.GetName().Lower();

#ifdef __WXMSW__

    MkspecString.Replace(_T("win32-"),_T(""));
#else
#ifdef __APPLE__

    MkspecString.Replace(_T("macx-"),_T(""));
#else

    struct utsname unixname;
    if (uname (&unixname) < 0)
    {
        return wxString(_T(""));
    }
    wxString OSString = wxString(unixname.sysname, wxConvUTF8).Lower();
    OSString << _T("-");
    MkspecString.Replace(OSString,_T(""));//TODO(yop#5#):check on other than Linux, Solaris
#endif
#endif

    if (CPPCompilerFileString.Contains(MkspecString.Lower()))
        return wxString(_T("")); //no need to use other than QMAKESPEC
    if (CCompilerFileString.Contains(MkspecString.Lower()))
        return wxString(_T("")); //no need to use other than QMAKESPEC

    // Oh well if we're here the envinroment's QMAKESPEC isn't good for this
    // compiler, so we'll try to find the correct one for the given compiler
    // get the names of all the available mkspecs
    wxArrayString mkspecsArray;
    MkSpecBrowser browser(mkspecsArray);

    wxDir QtMkspecDir(QtBaseDirString + wxFileName::GetPathSeparator() + wxString(_T("mkspecs")));
    QtMkspecDir.Traverse(browser);

    // Available mkspecs are in mkspecsArray
    for (unsigned int index = 0; index < mkspecsArray.GetCount(); index++)
    {
        MkspecString = mkspecsArray[index];
        MkspecString.Remove(0,QtMkspecDir.GetName().Length()+1);
        mkspecsArray[index] = MkspecString; // A little hack :)

#ifdef __WXMSW__

        MkspecString.Replace(_T("win32-"),_T(""));
#else
#ifdef __APPLE__

        MkspecString.Replace(_T("macx-"),_T(""));
#else

        MkspecString.Replace(OSString,_T(""));//TODO(yop#5#):check on other than Linux, Solaris
#endif
#endif

        if (CPPCompilerFileString.Contains(MkspecString.Lower()))
        {
            //Bingo!
            return mkspecsArray[index];
        }
        else if (CCompilerFileString.Contains(MkspecString.Lower()))
        {
            //Bingo!
            return mkspecsArray[index];
        }
    }
    // If all of the above fail the user should have the correct QMAKESPEC
    // set as an enval, else it will compile compile using the default compiler
    // or won't compile at all.
    return wxString(_T(""));
}*/

void qtwProGenerator::DoAppendCompilerOptions(ProjectBuildTarget* target, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
    if (useGlobalOptions)
        opts = m_CompilerSet->GetCompilerOptions();
    else
    {
        if (target)
            opts = target->GetCompilerOptions();
        else
            opts = m_Project ? m_Project->GetCompilerOptions() : m_CompilerSet->GetCompilerOptions();
    }


    wxArrayString currentValues = m_Handler->GetValuesFor(wxT("QMAKE_CXXFLAGS"));
    for (size_t i=0; i<opts.GetCount(); i++)
    {
        wxString temp = opts[i];
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(temp);
        currentValues.Add(temp);
    }
    m_Handler->SetValuesFor(wxT("QMAKE_CXXFLAGS"),currentValues);
}

void qtwProGenerator::DoAppendLinkerOptions(ProjectBuildTarget* target, bool useGlobalOptions)
{
    CompileOptionsBase* obj;
    if (!m_CompilerSet)
        return;
    if (useGlobalOptions)
        obj = m_CompilerSet;
    else
        obj = target ? (CompileOptionsBase*)target : (m_Project ? (CompileOptionsBase*)m_Project : m_CompilerSet);

    wxArrayString opts = obj->GetLinkerOptions();
    wxArrayString currentValues = m_Handler->GetValuesFor(wxT("QMAKE_LFLAGS"));
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        wxString temp = opts[x];
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(temp);
        currentValues.Add(temp);
    }
    m_Handler->SetValuesFor(wxT("QMAKE_LFLAGS"),currentValues);
}

void qtwProGenerator::DoAppendLinkerLibs(ProjectBuildTarget* target, bool useGlobalOptions)
{
    if (!m_CompilerSet)
        return;

    wxArrayString libs;
    if (useGlobalOptions)
        libs = m_CompilerSet->GetLinkLibs();
    else
    {
        if (target)
            libs = target->GetLinkLibs();
        else
        {
            if (m_Project)
                libs = m_Project->GetLinkLibs();
            else
                m_CompilerSet->GetLinkLibs();
        }
        wxString compID = target ? target->GetCompilerID() : (m_Project ? m_Project->GetCompilerID() : CompilerFactory::GetDefaultCompilerID());
        m_CompilerSet = CompilerFactory::GetCompiler(compID);
    }

    wxArrayString currentValues = m_Handler->GetValuesFor(wxT("LIBS"));
    for (unsigned int x = 0; x < libs.GetCount(); ++x)
    {
        if (libs[x].IsEmpty())
            continue;

        // construct linker option for each lib, based on compiler's settings
        wxString libPrefix = m_CompilerSet->GetSwitches().libPrefix;
        wxString libExt = m_CompilerSet->GetSwitches().libExtension;
        wxString lib = libs[x];
        QuoteStringIfNeeded(lib);
        // run replacements on libs only if no slashes in name (which means it's a relative or absolute path)
        if (lib.Find('/') == -1 && lib.Find('\\') == -1)
        {
            // 'lib' prefix
            bool hadLibPrefix = false;
            if (!m_CompilerSet->GetSwitches().linkerNeedsLibPrefix &&
                    !libPrefix.IsEmpty() &&
                    lib.StartsWith(libPrefix))
            {
                lib.Remove(0, libPrefix.Length());
                hadLibPrefix = true;
            }
            // extension
            if (!m_CompilerSet->GetSwitches().linkerNeedsLibExtension &&
                    lib.Length() > libExt.Length() &&
                    lib.Right(libExt.Length() + 1) == _T(".") + libExt)
            {
                // remove the extension only if we had a lib prefix
                if (hadLibPrefix)
                    lib.RemoveLast(libExt.Length() + 1);
            }
            else if (m_CompilerSet->GetSwitches().linkerNeedsLibExtension &&
                     !libExt.IsEmpty())
            {
                if (lib.Length() <= libExt.Length() ||
                        lib.Right(libExt.Length() + 1) != _T(".") + libExt)
                {
                    lib << _T(".") << libExt;
                }
            }
            lib = m_CompilerSet->GetSwitches().linkLibs + lib;
        }
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(lib);
        currentValues.Add(lib);
    }
    m_Handler->SetValuesFor(wxT("LIBS"),currentValues);
}

void qtwProGenerator::DoAppendIncludeDirs(ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
    if (useGlobalOptions)
        opts = m_CompilerSet->GetIncludeDirs();
    else
    {
        if (target)
            opts = target->GetIncludeDirs();
        else
            opts = m_Project ? m_Project->GetIncludeDirs() : m_CompilerSet->GetIncludeDirs();
    }

    wxArrayString currentValues = m_Handler->GetValuesFor(wxT("INCLUDEPATH"));
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        if (opts[x].IsEmpty())
            continue;
        wxString out = UnixFilename(opts[x]);
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        QuoteStringIfNeeded(out);
        currentValues.Add(opts[x]);
    }
    m_Handler->SetValuesFor(wxT("INCLUDEPATH"),currentValues);
}

void qtwProGenerator::DoAppendLibDirs(ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
    if (useGlobalOptions)
        opts = m_CompilerSet->GetLibDirs();
    else
    {
        if (target)
            opts = target->GetLibDirs();
        else
            opts = m_Project ? m_Project->GetLibDirs() : m_CompilerSet->GetLibDirs();
    }

    wxArrayString currentValues = m_Handler->GetValuesFor(wxT("LIBS"));
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        if (opts[x].Trim().IsEmpty())
            continue;
        wxString out = UnixFilename(opts[x]);
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        currentValues.Add(wxT("-L\"") + out + wxT("\""));
    }
    m_Handler->SetValuesFor(wxT("LIBS"),currentValues);
}

void qtwProGenerator::DoAddProOptions(ProjectBuildTarget* target)
{
    UpdateCompiler(target);
    if (!m_CompilerSet)
        return;

    OptionsRelation relation = target->GetOptionRelation(ortCompilerOptions);

    m_Handler->SetValuesFor(wxT("QMAKE_CXXFLAGS"),wxArrayString());
    DoAppendCompilerOptions(0L, true);
    if (relation==orUseTargetOptionsOnly)
    {
        DoAppendCompilerOptions(target);
    }
    else if (relation==orUseParentOptionsOnly)
    {
        DoAppendCompilerOptions(0L);
    }
    else if (relation==orPrependToParentOptions)
    {
        DoAppendCompilerOptions(target);
        DoAppendCompilerOptions(0L);
    }
    else if (relation==orAppendToParentOptions)
    {
        DoAppendCompilerOptions(0L);
        DoAppendCompilerOptions(target);
    }

    m_Handler->SetValuesFor(wxT("QMAKE_LFLAGS"),wxArrayString());
    DoAppendLinkerOptions(0L, true);
    if (relation==orUseTargetOptionsOnly)
    {
        DoAppendLinkerOptions(target);
    }
    else if (relation==orUseParentOptionsOnly)
    {
        DoAppendLinkerOptions(0L);
    }
    else if (relation==orPrependToParentOptions)
    {
        DoAppendLinkerOptions(target);
        DoAppendLinkerOptions(0L);
    }
    else if (relation==orAppendToParentOptions)
    {
        DoAppendLinkerOptions(0L);
        DoAppendLinkerOptions(target);
    }

    m_Handler->SetValuesFor(wxT("INCLUDEPATH"),wxArrayString());
    DoAppendIncludeDirs(0L, _T(""), true);
    if (relation==orUseTargetOptionsOnly)
    {
        DoAppendIncludeDirs(target, _T(""));
    }
    else if (relation==orUseParentOptionsOnly)
    {
        DoAppendIncludeDirs(0L, _T(""));
    }
    else if (relation==orPrependToParentOptions)
    {
        DoAppendIncludeDirs(target, _T(""));
        DoAppendIncludeDirs(0L, _T(""));
    }
    else if (relation==orAppendToParentOptions)
    {
        DoAppendIncludeDirs(0L, _T(""));
        DoAppendIncludeDirs(target, _T(""));
    }

    m_Handler->SetValuesFor(wxT("LIBS"),wxArrayString());
    DoAppendLibDirs(0L, _T(""), true);
    if (relation==orUseTargetOptionsOnly)
    {
        DoAppendLibDirs(target, _T(""));
    }
    else if (relation==orUseParentOptionsOnly)
    {
        DoAppendLibDirs(0L, _T(""));
    }
    else if (relation==orPrependToParentOptions)
    {
        DoAppendLibDirs(target, _T(""));
        DoAppendLibDirs(0L, _T(""));
    }
    else if (relation==orAppendToParentOptions)
    {
        DoAppendLibDirs(0L, _T(""));
        DoAppendLibDirs(target, _T(""));
    }

    // Both Lib dirs and Libs are in LIBS
    // m_Handler->SetValuesFor(wxT("LIBS"),wxArrayString());
    DoAppendLinkerLibs(0L, true);
    if (relation==orUseTargetOptionsOnly)
    {
        DoAppendLinkerLibs(target);
    }
    else if (relation==orUseParentOptionsOnly)
    {
        DoAppendLinkerLibs(0L);
    }
    else if (relation==orPrependToParentOptions)
    {
        DoAppendLinkerLibs(target);
        DoAppendLinkerLibs(0L);
    }
    else if (relation==orAppendToParentOptions)
    {
        DoAppendLinkerLibs(0L);
        DoAppendLinkerLibs(target);
    }
}

void qtwProGenerator::DoAddTargetFiles(ProjectBuildTarget* target)
{
    if (!IsTargetValid(target))
        return;
    if (!m_CompilerSet)
        return;

    wxArrayString target_headers;
    wxArrayString target_sources;
    wxArrayString target_forms;
    wxArrayString target_resources;
    wxArrayString target_translations;
    wxArrayString target_pch;

    int filesCount = m_Files.GetCount();
    for (int i = 0; i < filesCount; ++i)
    {
        ProjectFile* pf = m_Files[i];
        // if the file belongs in this target
        if (pf->buildTargets.Index(target->GetTitle()) >= 0)
        {
            wxString fname(pf->relativeFilename);

            if (FileTypeOf(fname) == ftSource)
            {
                target_sources.Add(fname);
            }
            else if (FileTypeOf(fname) == ftHeader)
            {
                target_headers.Add(fname);
                if (pf->compile)
                {
                    target_pch.Add(fname);
                }
            }
            else if (fname.AfterLast(_T('.')).Lower() == _T("ui"))
            {
                target_forms.Add(fname);
            }
            else if (fname.AfterLast(_T('.')).Lower() == _T("ts"))
            {
                target_translations.Add(fname);
            }
            else if (fname.AfterLast(_T('.')).Lower() == _T("qrc"))
            {
                target_resources.Add(fname);
            }
        }
    }

    m_Handler->SetValuesFor(wxT("HEADERS"),target_headers);
    m_Handler->SetValuesFor(wxT("SOURCES"),target_sources);
    m_Handler->SetValuesFor(wxT("FORMS"),target_forms);
    m_Handler->SetValuesFor(wxT("RESOURCES"),target_resources);
    m_Handler->SetValuesFor(wxT("TRANSLATIONS"),target_translations);
    m_Handler->SetValuesFor(wxT("PRECOMPILED_HEADER"),target_pch);
}

bool qtwProGenerator::CreatePro()
{
    DoPrepareFiles();
    DoPrepareValidTargets();

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int i = 0; i < targetsCount; ++i)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(i);
        m_Configuration.Clear();

        if (!IsTargetValid(target))
            continue;

        wxString TargetProFilenameString = m_Project->GetBasePath() + target->GetTitle();
        TargetProFilenameString << _T(".pro");

        m_Handler->SetFilename(TargetProFilenameString);
        m_Handler->Read();

        DoAddCommonVariables(target);
        DoAddTargetFiles(target);
        DoAddProOptions(target);

        for (size_t j=0; j<m_Configuration.GetCount(); j++)
        {
            m_Handler->Add(wxT("CONFIG"),m_Configuration[j]);
        }

        m_Handler->Write();
    }
    return true;
}
