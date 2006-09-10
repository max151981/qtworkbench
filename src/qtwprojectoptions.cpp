/***************************************************************
 * Name:      qtwprojectoptions.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include <sdk.h>

#include "qtwprojectoptions.h"
#include "qtwprogenerator.h"

BEGIN_EVENT_TABLE(qtwProjectOptions, wxDialog)
EVT_BUTTON(XRCID("ID_LOC_MOC_BUTTON"),qtwProjectOptions::OnBrowseMocButtonClick)
EVT_BUTTON(XRCID("ID_LOC_UIC_BUTTON"),qtwProjectOptions::OnBrowseUicButtonClick)
EVT_BUTTON(XRCID("ID_LOC_RCC_BUTTON"),qtwProjectOptions::OnBrowseRccButtonClick)
EVT_LISTBOX(XRCID("ID_TARGET_LISTBOX"),qtwProjectOptions::OnTargetListClick)
EVT_CHECKBOX(XRCID("ID_BUILDMOD_RELEASE_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_BUILDMOD_DEBUG_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_QT_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_THREAD_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_EXCEPTIONS_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_OPENGL_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_X11_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_CONSOLE_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_STL_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_RTTI_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_REQS_PREC_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_CORE_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_GUI_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_NET_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_OPENGL_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_SQL_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_XML_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_SVG_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_MODS_QT3_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_CHECKBOX(XRCID("ID_QTPLUGIN_CHECKBOX"),qtwProjectOptions::OnNeedsUpdate)
EVT_TEXT(XRCID("ID_LOC_MOC_TEXTCTRL"),qtwProjectOptions::OnNeedsUpdate)
EVT_TEXT(XRCID("ID_LOC_UIC_TEXTCTRL"),qtwProjectOptions::OnNeedsUpdate)
EVT_TEXT(XRCID("ID_LOC_RCC_TEXTCTRL"),qtwProjectOptions::OnNeedsUpdate)
END_EVENT_TABLE()

qtwProjectOptions::qtwProjectOptions(wxWindow* parent) : m_AllProjectOptions(0L)
{
    m_AllProjectOptions = new wxArrayString;
    wxXmlResource::Get()->LoadDialog(this, parent, _T("qtwprojectoptions"));
    PopulateTargetsListBox();
    PopulateTargetsArrayString();
    PopulateWorld();
}

qtwProjectOptions::~qtwProjectOptions()
{}

cbProject* qtwProjectOptions::CurrentActiveProject()
{
    // The active project is always be valid as it is checked
    // before this dialog appears.
    return Manager::Get()->GetProjectManager()->GetActiveProject();
}

wxString qtwProjectOptions::GetListedProjectFileContents(int index)
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    cbProject *CurrentProject = CurrentActiveProject();
    wxString TargetBuffer;
    if(CurrentProject)
    {
        wxString CurrentTarget = CurrentProject->GetBasePath();
        CurrentTarget << wxFileName::GetPathSeparator() << list->GetString(index);
        if(wxDir::Exists(CurrentTarget))
        {
            CurrentTarget << wxFileName::GetPathSeparator();
            CurrentTarget << list->GetString(index) + _T(".pro");
            if(wxFile::Exists(CurrentTarget))
            {
                wxFile file(CurrentTarget, wxFile::read);
                cbRead(file,TargetBuffer);
            }
        }
        else if (index==0)
        {
            // probably one target only
            CurrentTarget = CurrentProject->GetBasePath();
            CurrentTarget << CurrentActiveProject()->GetTitle() + _T(".pro");
            if(wxFile::Exists(CurrentTarget))
            {
                wxFile file(CurrentTarget, wxFile::read);
                cbRead(file,TargetBuffer);
            }
        }
        else
        {
            wxString message = _T("Folder ");
            message << CurrentTarget;
            message << _T(" does not exist,");
            message << _T('\n');
            message << _T("in the project's folder");
            cbMessageBox(message, _("Error"), wxICON_ERROR);
        }
    }
    return TargetBuffer;
}

void qtwProjectOptions::PopulateTargetsListBox()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    cbProject *CurrentProject = CurrentActiveProject();

    if(CurrentProject)
    {
        int TargetsCount = CurrentProject->GetBuildTargetsCount();
        wxArrayString TargetsArray;
        for(int i=0; i<TargetsCount; i++)
        {
            TargetsArray.Add(CurrentProject->GetBuildTarget(i)->GetTitle());
        }
        list->Set(TargetsArray);
        list->SetSelection(0);
    }

}

void qtwProjectOptions::PopulateTargetsArrayString()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    int TargetsCount = list->GetCount();
    for(int i=0; i<TargetsCount; i++)
    {
        m_AllProjectOptions->Add(GetListedProjectFileContents(i));
    }
}

void qtwProjectOptions::PopulateWorld()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);

    int currentSelection = list->GetSelection();
    if(currentSelection == wxNOT_FOUND)
        return;

    wxString TargetBuffer = m_AllProjectOptions->Item(currentSelection);

    PopulateBuildMode(TargetBuffer);
    PopulateRequirements(TargetBuffer);
    PopulateModules(TargetBuffer);
    PopulatePluginMode(TargetBuffer);
    PopulateFileLocations(TargetBuffer);
}

void qtwProjectOptions::PopulateBuildMode(const wxString &buffer)
{
    wxCheckBox *choiceRelease = XRCCTRL(*this, "ID_BUILDMOD_RELEASE_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceDebug = XRCCTRL(*this, "ID_BUILDMOD_DEBUG_CHECKBOX", wxCheckBox);

    choiceRelease->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("release")));
    choiceDebug->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("debug")));
}

void qtwProjectOptions::PopulateRequirements(const wxString &buffer)
{
    wxCheckBox *choiceQt = XRCCTRL(*this, "ID_REQS_QT_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceThread = XRCCTRL(*this, "ID_REQS_THREAD_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceExceptions = XRCCTRL(*this, "ID_REQS_EXCEPTIONS_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceOpenGL = XRCCTRL(*this, "ID_REQS_OPENGL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceX11 = XRCCTRL(*this, "ID_REQS_X11_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceConsole = XRCCTRL(*this, "ID_REQS_CONSOLE_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceSTL = XRCCTRL(*this, "ID_REQS_STL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceRTTI = XRCCTRL(*this, "ID_REQS_RTTI_CHECKBOX", wxCheckBox);
    wxCheckBox *choicePCH = XRCCTRL(*this, "ID_REQS_PREC_CHECKBOX", wxCheckBox);

    choiceQt->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("qt")));
    choiceThread->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("thread")));
    choiceExceptions->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("exceptions")));
    choiceOpenGL->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("opengl")));
    choiceX11->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("x11")));
    choiceConsole->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("console")));
    choiceSTL->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("stl")));
    choiceRTTI->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("rtti")));
    choicePCH->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("precompile_header")));
}

void qtwProjectOptions::PopulateModules(const wxString &buffer)
{
    wxCheckBox *choiceCore = XRCCTRL(*this, "ID_MODS_CORE_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceGui = XRCCTRL(*this, "ID_MODS_GUI_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceNetwork = XRCCTRL(*this, "ID_MODS_NET_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceOpenGL = XRCCTRL(*this, "ID_MODS_OPENGL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceSql = XRCCTRL(*this, "ID_MODS_SQL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceXml = XRCCTRL(*this, "ID_MODS_XML_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceSvg = XRCCTRL(*this, "ID_MODS_SVG_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceQt3 = XRCCTRL(*this, "ID_MODS_QT3_CHECKBOX", wxCheckBox);

    choiceCore->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("core")));
    choiceGui->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("gui")));
    choiceNetwork->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("network")));
    choiceOpenGL->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("opengl")));
    choiceSql->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("sql")));
    choiceXml->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("xml")));
    choiceSvg->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("svg")));
    choiceQt3->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("QT")).Contains(_T("qt3support")));
}

void qtwProjectOptions::PopulatePluginMode(const wxString &buffer)
{
    wxCheckBox *choicePlugin = XRCCTRL(*this, "ID_QTPLUGIN_CHECKBOX", wxCheckBox);
    choicePlugin->SetValue(qtwProGenerator::GetArgsFor(buffer,_T("CONFIG")).Contains(_T("plugin")));
}

void qtwProjectOptions::PopulateFileLocations(const wxString &buffer)
{
    wxTextCtrl *choiceMocDir = XRCCTRL(*this, "ID_LOC_MOC_TEXTCTRL", wxTextCtrl);
    wxTextCtrl *choiceUicDir = XRCCTRL(*this, "ID_LOC_UIC_TEXTCTRL", wxTextCtrl);
    wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);

    wxString value = qtwProGenerator::GetArgsFor(buffer,_T("MOC_DIR"));
    value.Replace(_T("MOC_DIR=\t"),_T(""));
    value.Pad(0);
    choiceMocDir->SetValue(value);
    value = qtwProGenerator::GetArgsFor(buffer,_T("UI_DIR"));
    value.Replace(_T("UI_DIR=\t"),_T(""));
    value.Pad(0);
    choiceUicDir->SetValue(value);
    value = qtwProGenerator::GetArgsFor(buffer,_T("RCC_DIR"));
    value.Replace(_T("RCC_DIR=\t"),_T(""));
    value.Pad(0);
    choiceRccDir->SetValue(value);
}

void qtwProjectOptions::OnNeedsUpdate(wxCommandEvent& event)
{
    UpdateQtTargetSettings();
}

void qtwProjectOptions::OnBrowseMocButtonClick(wxCommandEvent& event)
{
    wxString targetDir = CurrentActiveProject()->GetBasePath() +
                         XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox)->GetStringSelection();
    wxString mocDir = ChooseDirectory(this,
                                      _T("Select moc output directory (relative to target directory)"),
                                      targetDir,
                                      targetDir,
                                      true,
                                      true);
    wxTextCtrl *choiceMocDir = XRCCTRL(*this, "ID_LOC_MOC_TEXTCTRL", wxTextCtrl);
    choiceMocDir->SetValue(mocDir);
}

void qtwProjectOptions::OnBrowseUicButtonClick(wxCommandEvent& event)
{
    wxString targetDir = CurrentActiveProject()->GetBasePath() +
                         XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox)->GetStringSelection();
    wxString uicDir = ChooseDirectory(this,
                                      _T("Select uic output directory (relative to target directory)"),
                                      targetDir,
                                      targetDir,
                                      true,
                                      true);
    wxTextCtrl *choiceUicDir = XRCCTRL(*this, "ID_LOC_UIC_TEXTCTRL", wxTextCtrl);
    choiceUicDir->SetValue(uicDir);
}

void qtwProjectOptions::OnBrowseRccButtonClick(wxCommandEvent& event)
{
    wxString targetDir = CurrentActiveProject()->GetBasePath() +
                         XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox)->GetStringSelection();
    wxString rccDir = ChooseDirectory(this,
                                      _T("Select rcc output directory (relative to target directory)"),
                                      targetDir,
                                      targetDir,
                                      true,
                                      true);
    wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);
    choiceRccDir->SetValue(rccDir);
}

void qtwProjectOptions::OnTargetListClick(wxCommandEvent& event)
{
    PopulateWorld();
}

void qtwProjectOptions::SaveQtProjectSettings()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    cbProject *CurrentProject = CurrentActiveProject();
    if(CurrentProject)
    {
        for (int index=0; index<list->GetCount(); index++)
        {
            wxString CurrentTarget = CurrentProject->GetBasePath();
            CurrentTarget << wxFileName::GetPathSeparator() << list->GetString(index);
            if(wxDir::Exists(CurrentTarget))
            {
                CurrentTarget << wxFileName::GetPathSeparator();
                CurrentTarget << list->GetString(index) + _T(".pro");
                wxFile file(CurrentTarget, wxFile::write);
                cbWrite(file,m_AllProjectOptions->Item(index));
            }
            else if(list->GetCount()==1)
            {
                // One target only this is an easy one
                CurrentTarget = CurrentProject->GetBasePath();
                CurrentTarget << CurrentActiveProject()->GetTitle() + _T(".pro");
                wxFile file(CurrentTarget, wxFile::write);
                cbWrite(file,m_AllProjectOptions->Item(index));
            }
            else
            {
                wxString message = _T("Folder ");
                message << CurrentTarget;
                message << _T(" does not exist,");
                message << _T('\n');
                message << _T("in the project's folder");
                cbMessageBox(message, _("Error"), wxICON_ERROR);
            }
        }
    }
}

void qtwProjectOptions::UpdateQtTargetSettings()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    int currentIndex = list->GetSelection();
    if(currentIndex == wxNOT_FOUND)
        return;
    cbProject *CurrentProject = CurrentActiveProject();
    if(CurrentProject)
    {
        wxString TargetBuffer;
        wxString CurrentTarget = CurrentProject->GetBasePath();
        CurrentTarget << wxFileName::GetPathSeparator() << list->GetStringSelection();
        if(wxDir::Exists(CurrentTarget) || list->GetCount()==1)
        {
            TargetBuffer = m_AllProjectOptions->Item(currentIndex);
            if((TargetBuffer.IsEmpty()))
            {
                qtwProGenerator::PrependHeader(TargetBuffer);
            }

            wxString argumentsToAdd;
            wxString argumentsToRemove;
            wxCheckBox *choiceRelease = XRCCTRL(*this, "ID_BUILDMOD_RELEASE_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceDebug = XRCCTRL(*this, "ID_BUILDMOD_DEBUG_CHECKBOX", wxCheckBox);
            if(choiceRelease->GetValue())
                argumentsToAdd.Append(_T(" release"));
            else
                argumentsToRemove.Append(_T(" release"));
            if(choiceDebug->GetValue())
                argumentsToAdd.Append(_T(" debug"));
            else
                argumentsToRemove.Append(_T(" debug"));
            argumentsToAdd.Remove(0,1);
            qtwProGenerator::Remove(TargetBuffer,_T("CONFIG"),argumentsToRemove);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("CONFIG"),argumentsToAdd,_T("+="),false);
            argumentsToAdd = _T("");
            argumentsToRemove = _T("");

            wxCheckBox *choiceQt = XRCCTRL(*this, "ID_REQS_QT_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceThread = XRCCTRL(*this, "ID_REQS_THREAD_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceExceptions = XRCCTRL(*this, "ID_REQS_EXCEPTIONS_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceOpenGL = XRCCTRL(*this, "ID_REQS_OPENGL_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceX11 = XRCCTRL(*this, "ID_REQS_X11_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceConsole = XRCCTRL(*this, "ID_REQS_CONSOLE_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceSTL = XRCCTRL(*this, "ID_REQS_STL_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceRTTI = XRCCTRL(*this, "ID_REQS_RTTI_CHECKBOX", wxCheckBox);
            wxCheckBox *choicePCH = XRCCTRL(*this, "ID_REQS_PREC_CHECKBOX", wxCheckBox);
            if(choiceQt->GetValue())
                argumentsToAdd.Append(_T(" qt"));
            else
                argumentsToRemove.Append(_T(" qt"));
            if(choiceThread->GetValue())
                argumentsToAdd.Append(_T(" thread"));
            else
                argumentsToRemove.Append(_T(" thread"));
            if(choiceExceptions->GetValue())
                argumentsToAdd.Append(_T(" exceptions"));
            else
                argumentsToRemove.Append(_T(" exceptions"));
            if(choiceOpenGL->GetValue())
                argumentsToAdd.Append(_T(" opengl"));
            else
                argumentsToRemove.Append(_T(" opengl"));
            if(choiceX11->GetValue())
                argumentsToAdd.Append(_T(" x11"));
            else
                argumentsToRemove.Append(_T(" x11"));
            if(choiceConsole->GetValue())
                argumentsToAdd.Append(_T(" console"));
            else
                argumentsToRemove.Append(_T(" console"));
            if(choiceSTL->GetValue())
                argumentsToAdd.Append(_T(" stl"));
            else
                argumentsToRemove.Append(_T(" stl"));
            if(choiceRTTI->GetValue())
                argumentsToAdd.Append(_T(" rtti"));
            else
                argumentsToRemove.Append(_T(" rtti"));
            if(choicePCH->GetValue())
                argumentsToAdd.Append(_T(" precompile_header"));
            else
                argumentsToRemove.Append(_T(" precompile_header"));
            argumentsToAdd.Remove(0,1);
            qtwProGenerator::Remove(TargetBuffer,_T("CONFIG"),argumentsToRemove);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("CONFIG"),argumentsToAdd,_T("+="),false);
            argumentsToAdd = _T("");
            argumentsToRemove = _T("");

            wxCheckBox *choiceCore = XRCCTRL(*this, "ID_MODS_CORE_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceGui = XRCCTRL(*this, "ID_MODS_GUI_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceNetwork = XRCCTRL(*this, "ID_MODS_NET_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceModOpenGL = XRCCTRL(*this, "ID_MODS_OPENGL_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceSql = XRCCTRL(*this, "ID_MODS_SQL_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceXml = XRCCTRL(*this, "ID_MODS_XML_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceSvg = XRCCTRL(*this, "ID_MODS_SVG_CHECKBOX", wxCheckBox);
            wxCheckBox *choiceQt3 = XRCCTRL(*this, "ID_MODS_QT3_CHECKBOX", wxCheckBox);
            if(choiceCore->GetValue())
                argumentsToAdd.Append(_T(" core"));
            else
                argumentsToRemove.Append(_T(" core"));
            if(choiceGui->GetValue())
                argumentsToAdd.Append(_T(" gui"));
            else
                argumentsToRemove.Append(_T(" gui"));
            if(choiceNetwork->GetValue())
                argumentsToAdd.Append(_T(" network"));
            else
                argumentsToRemove.Append(_T(" network"));
            if(choiceModOpenGL->GetValue())
                argumentsToAdd.Append(_T(" opengl"));
            else
                argumentsToRemove.Append(_T(" opengl"));
            if(choiceSql->GetValue())
                argumentsToAdd.Append(_T(" sql"));
            else
                argumentsToRemove.Append(_T(" sql"));
            if(choiceXml->GetValue())
                argumentsToAdd.Append(_T(" xml"));
            else
                argumentsToRemove.Append(_T(" xml"));
            if(choiceSvg->GetValue())
                argumentsToAdd.Append(_T(" svg"));
            else
                argumentsToRemove.Append(_T(" svg"));
            if(choiceQt3->GetValue())
                argumentsToAdd.Append(_T(" qt3support"));
            else
                argumentsToRemove.Append(_T(" qt3support"));
            argumentsToAdd.Remove(0,1);
            qtwProGenerator::Remove(TargetBuffer,_T("QT"),argumentsToRemove);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("QT"),argumentsToAdd,_T("+="),false);
            argumentsToAdd = _T("");
            argumentsToRemove = _T("");

            wxCheckBox *choicePlugin = XRCCTRL(*this, "ID_QTPLUGIN_CHECKBOX", wxCheckBox);
            if(choicePlugin->GetValue())
                argumentsToAdd.Append(_T("plugin"));
            else
                argumentsToRemove.Append(_T("plugin"));
            qtwProGenerator::Remove(TargetBuffer,_T("CONFIG"),argumentsToRemove);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("CONFIG"),argumentsToAdd,_T("+="),false);
            argumentsToAdd = _T("");
            argumentsToRemove = _T("");

            wxTextCtrl *choiceMocDir = XRCCTRL(*this, "ID_LOC_MOC_TEXTCTRL", wxTextCtrl);
            wxTextCtrl *choiceUicDir = XRCCTRL(*this, "ID_LOC_UIC_TEXTCTRL", wxTextCtrl);
            wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);

            wxString selectedPath = choiceMocDir->GetValue();
            // I kept getting some trash (extra whitespace) that messed up the .pro file
            selectedPath.Replace(_T("\n"),_T(""));
            selectedPath.Replace(_T("\r"),_T(""));
            selectedPath.Trim();
            selectedPath.Trim(false);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("MOC_DIR"),selectedPath,_T("="),true);

            selectedPath = choiceUicDir->GetValue();
            selectedPath.Replace(_T("\n"),_T(""));
            selectedPath.Replace(_T("\r"),_T(""));
            selectedPath.Trim();
            selectedPath.Trim(false);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("UI_DIR"),selectedPath,_T("="),true);

            selectedPath = choiceRccDir->GetValue();
            selectedPath.Replace(_T("\n"),_T(""));
            selectedPath.Replace(_T("\r"),_T(""));
            selectedPath.Trim();
            selectedPath.Trim(false);
            qtwProGenerator::AddOrReplace(TargetBuffer,_T("RCC_DIR"),selectedPath,_T("="),true);

            if(!TargetBuffer.IsEmpty())
            {
                m_AllProjectOptions->RemoveAt(currentIndex);
                m_AllProjectOptions->Insert(TargetBuffer,currentIndex);
            }
        }
    }
}

void qtwProjectOptions::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        SaveQtProjectSettings();
    }

    wxDialog::EndModal(retCode);
}

