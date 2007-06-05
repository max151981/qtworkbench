/***************************************************************
 * Name:      qtwprojectoptions.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include <wx/tokenzr.h>
#include <sdk.h>

#include "qtwprojectoptions.h"
#include "qtwprojecthandler.h"

BEGIN_EVENT_TABLE(qtwProjectOptions, wxDialog)
EVT_BUTTON(XRCID("ID_LOC_MOC_BUTTON"),qtwProjectOptions::OnBrowseMocButtonClick)
EVT_BUTTON(XRCID("ID_LOC_UIC_BUTTON"),qtwProjectOptions::OnBrowseUicButtonClick)
EVT_BUTTON(XRCID("ID_LOC_RCC_BUTTON"),qtwProjectOptions::OnBrowseRccButtonClick)
EVT_LISTBOX(XRCID("ID_TARGET_LISTBOX"),qtwProjectOptions::OnTargetListClick)
END_EVENT_TABLE()

qtwProjectOptions::qtwProjectOptions(wxWindow* parent)
{
    m_Handler = new QtWProjectHandler;

    wxXmlResource::Get()->LoadDialog(this, parent, wxT("dlgQtWProjectOptions"));

    PopulateTargetsListBox();
    UpdateTarget();
    PopulateWorld();
}

qtwProjectOptions::~qtwProjectOptions()
{
    delete m_Handler;
}

cbProject* qtwProjectOptions::CurrentActiveProject()
{
    // The active project is always valid as it is checked
    // before this dialog appears.
    return Manager::Get()->GetProjectManager()->GetActiveProject();
}

void qtwProjectOptions::PopulateTargetsListBox()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);
    int TargetsCount = CurrentActiveProject()->GetBuildTargetsCount();
    wxArrayString TargetsArray;
    for (int i=0; i<TargetsCount; i++)
    {
        TargetsArray.Add(CurrentActiveProject()->GetBuildTarget(i)->GetTitle());
    }
    list->Set(TargetsArray);
    list->SetSelection(0);
}

void qtwProjectOptions::UpdateTarget()
{
    wxListBox *list = XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox);

    int currentSelection = list->GetSelection();
    if (currentSelection == wxNOT_FOUND)
        return;

    wxString CurrentTarget = CurrentActiveProject()->GetBasePath();
    CurrentTarget << list->GetString(currentSelection) + wxT(".pro");

    m_Handler->SetFilename(CurrentTarget);
    m_Handler->Read();
}

void qtwProjectOptions::PopulateWorld()
{
    PopulateBuildMode();
    PopulateRequirements();
    PopulateModules();
    PopulateFileLocations();
}

void qtwProjectOptions::PopulateBuildMode()
{
    wxCheckBox *choiceRelease = XRCCTRL(*this, "ID_BUILDMOD_RELEASE_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceDebug = XRCCTRL(*this, "ID_BUILDMOD_DEBUG_CHECKBOX", wxCheckBox);

    choiceRelease->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("release")));
    choiceDebug->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("debug")));
}

void qtwProjectOptions::PopulateRequirements()
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

    choiceQt->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("qt")));
    choiceThread->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("thread")));
    choiceExceptions->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("exceptions")));
    choiceOpenGL->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("opengl")));
    choiceX11->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("x11")));
    choiceConsole->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("console")));
    choiceSTL->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("stl")));
    choiceRTTI->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("rtti")));
    choicePCH->SetValue(m_Handler->Contains(wxT("CONFIG"),wxT("precompile_header")));

    wxTextCtrl *configChoices = XRCCTRL(*this, "ID_CONFIG_VARIABLES", wxTextCtrl);
    wxArrayString values = m_Handler->GetValuesFor(wxT("CONFIG"));
    wxString value;
    m_ExtraConfigurations.Clear();
    for (size_t i=0; i<values.GetCount(); i++)
    {
        if (values[i] != wxT("release") &&
                values[i] != wxT("debug") &&
                values[i] != wxT("qt") &&
                values[i] != wxT("thread") &&
                values[i] != wxT("exceptions") &&
                values[i] != wxT("opengl") &&
                values[i] != wxT("x11") &&
                values[i] != wxT("console") &&
                values[i] != wxT("stl") &&
                values[i] != wxT("rtti") &&
                values[i] != wxT("precompile_header") &&
                values[i] != wxT("staticlib") &&
                values[i] != wxT("windows") &&
                values[i] != wxT("console") &&
                values[i] != wxT("staticlib") &&
                values[i] != wxT("dll"))

        {
            value += wxT(" ");
            value += values[i];
            m_ExtraConfigurations.Add(values[i]);
        }
    }
    configChoices->SetValue(value);
}

void qtwProjectOptions::PopulateModules()
{
    wxCheckBox *choiceCore = XRCCTRL(*this, "ID_MODS_CORE_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceGui = XRCCTRL(*this, "ID_MODS_GUI_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceNetwork = XRCCTRL(*this, "ID_MODS_NET_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceOpenGL = XRCCTRL(*this, "ID_MODS_OPENGL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceSql = XRCCTRL(*this, "ID_MODS_SQL_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceXml = XRCCTRL(*this, "ID_MODS_XML_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceSvg = XRCCTRL(*this, "ID_MODS_SVG_CHECKBOX", wxCheckBox);
    wxCheckBox *choiceQt3 = XRCCTRL(*this, "ID_MODS_QT3_CHECKBOX", wxCheckBox);

    choiceCore->SetValue(m_Handler->Contains(wxT("QT"),wxT("core")));
    choiceGui->SetValue(m_Handler->Contains(wxT("QT"),wxT("gui")));
    choiceNetwork->SetValue(m_Handler->Contains(wxT("QT"),wxT("network")));
    choiceOpenGL->SetValue(m_Handler->Contains(wxT("QT"),wxT("opengl")));
    choiceSql->SetValue(m_Handler->Contains(wxT("QT"),wxT("sql")));
    choiceXml->SetValue(m_Handler->Contains(wxT("QT"),wxT("xml")));
    choiceSvg->SetValue(m_Handler->Contains(wxT("QT"),wxT("svg")));
    choiceQt3->SetValue(m_Handler->Contains(wxT("QT"),wxT("qt3support")));
}

void qtwProjectOptions::PopulateFileLocations()
{
    wxTextCtrl *choiceMocDir = XRCCTRL(*this, "ID_LOC_MOC_TEXTCTRL", wxTextCtrl);
    wxTextCtrl *choiceUicDir = XRCCTRL(*this, "ID_LOC_UIC_TEXTCTRL", wxTextCtrl);
    wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);

    wxArrayString MocValues = m_Handler->GetValuesFor(wxT("MOC_DIR"));
    choiceMocDir->Clear();
    for (size_t i=0 ; i<MocValues.GetCount(); i++)
    {
        if (!MocValues[i].IsEmpty())
        {
            choiceMocDir->SetValue(MocValues[i]);
            break;
        }
    }

    wxArrayString UiValues = m_Handler->GetValuesFor(wxT("UI_DIR"));
    choiceUicDir->Clear();
    for (size_t i=0 ; i<UiValues.GetCount(); i++)
    {
        if (!UiValues[i].IsEmpty())
        {
            choiceUicDir->SetValue(UiValues[i]);
            break;
        }
    }

    wxArrayString RccValues = m_Handler->GetValuesFor(wxT("RCC_DIR"));
    choiceRccDir->Clear();
    for (size_t i=0 ; i<RccValues.GetCount(); i++)
    {
        if (!RccValues[i].IsEmpty())
        {
            choiceRccDir->SetValue(RccValues[i]);
            break;
        }
    }
}

void qtwProjectOptions::Update()
{

    wxCheckBox *choiceRelease = XRCCTRL(*this, "ID_BUILDMOD_RELEASE_CHECKBOX", wxCheckBox);
    if (choiceRelease->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("release"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("release"));

    wxCheckBox *choiceDebug = XRCCTRL(*this, "ID_BUILDMOD_DEBUG_CHECKBOX", wxCheckBox);
    if (choiceDebug->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("debug"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("debug"));

    wxCheckBox *choiceQt = XRCCTRL(*this, "ID_REQS_QT_CHECKBOX", wxCheckBox);
    if (choiceQt->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("qt"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("qt"));

    wxCheckBox *choiceThread = XRCCTRL(*this, "ID_REQS_THREAD_CHECKBOX", wxCheckBox);
    if (choiceThread->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("thread"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("thread"));

    wxCheckBox *choiceExceptions = XRCCTRL(*this, "ID_REQS_EXCEPTIONS_CHECKBOX", wxCheckBox);
    if (choiceExceptions->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("exceptions"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("exceptions"));

    wxCheckBox *choiceOpenGL = XRCCTRL(*this, "ID_REQS_OPENGL_CHECKBOX", wxCheckBox);
    if (choiceOpenGL->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("opengl"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("opengl"));

    wxCheckBox *choiceX11 = XRCCTRL(*this, "ID_REQS_X11_CHECKBOX", wxCheckBox);
    if (choiceX11->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("x11"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("x11"));

    wxCheckBox *choiceConsole = XRCCTRL(*this, "ID_REQS_CONSOLE_CHECKBOX", wxCheckBox);
    if (choiceConsole->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("console"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("console"));

    wxCheckBox *choiceSTL = XRCCTRL(*this, "ID_REQS_STL_CHECKBOX", wxCheckBox);
    if (choiceSTL->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("stl"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("stl"));

    wxCheckBox *choiceRTTI = XRCCTRL(*this, "ID_REQS_RTTI_CHECKBOX", wxCheckBox);
    if (choiceRTTI->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("rtti"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("rtti"));

    wxCheckBox *choicePCH = XRCCTRL(*this, "ID_REQS_PREC_CHECKBOX", wxCheckBox);
    if (choicePCH->GetValue())
        m_Handler->Add(wxT("CONFIG"),wxT("precompile_header"));
    else
        m_Handler->Remove(wxT("CONFIG"),wxT("precompile_header"));

    wxCheckBox *choiceCore = XRCCTRL(*this, "ID_MODS_CORE_CHECKBOX", wxCheckBox);
    if (choiceCore->GetValue())
        m_Handler->Add(wxT("QT"),wxT("core"));
    else
        m_Handler->Remove(wxT("QT"),wxT("core"));

    wxCheckBox *choiceGui = XRCCTRL(*this, "ID_MODS_GUI_CHECKBOX", wxCheckBox);
    if (choiceGui->GetValue())
        m_Handler->Add(wxT("QT"),wxT("gui"));
    else
        m_Handler->Remove(wxT("QT"),wxT("gui"));

    wxCheckBox *choiceNetwork = XRCCTRL(*this, "ID_MODS_NET_CHECKBOX", wxCheckBox);
    if (choiceNetwork->GetValue())
        m_Handler->Add(wxT("QT"),wxT("network"));
    else
        m_Handler->Remove(wxT("QT"),wxT("network"));

    wxCheckBox *choiceModOpenGL = XRCCTRL(*this, "ID_MODS_OPENGL_CHECKBOX", wxCheckBox);
    if (choiceModOpenGL->GetValue())
        m_Handler->Add(wxT("QT"),wxT("opengl"));
    else
        m_Handler->Remove(wxT("QT"),wxT("opengl"));

    wxCheckBox *choiceSql = XRCCTRL(*this, "ID_MODS_SQL_CHECKBOX", wxCheckBox);
    if (choiceSql->GetValue())
        m_Handler->Add(wxT("QT"),wxT("sql"));
    else
        m_Handler->Remove(wxT("QT"),wxT("sql"));

    wxCheckBox *choiceXml = XRCCTRL(*this, "ID_MODS_XML_CHECKBOX", wxCheckBox);
    if (choiceXml->GetValue())
        m_Handler->Add(wxT("QT"),wxT("xml"));
    else
        m_Handler->Remove(wxT("QT"),wxT("xml"));

    wxCheckBox *choiceSvg = XRCCTRL(*this, "ID_MODS_SVG_CHECKBOX", wxCheckBox);
    if (choiceSvg->GetValue())
        m_Handler->Add(wxT("QT"),wxT("svg"));
    else
        m_Handler->Remove(wxT("QT"),wxT("svg"));

    wxCheckBox *choiceQt3 = XRCCTRL(*this, "ID_MODS_QT3_CHECKBOX", wxCheckBox);
    if (choiceQt3->GetValue())
        m_Handler->Add(wxT("QT"),wxT("qt3support"));
    else
        m_Handler->Remove(wxT("QT"),wxT("qt3support"));

    wxTextCtrl *choiceMocDir = XRCCTRL(*this, "ID_LOC_MOC_TEXTCTRL", wxTextCtrl);
    wxString selectedPath = choiceMocDir->GetValue();
    selectedPath.Trim();
    selectedPath.Trim(false);
    wxArrayString values;
    values.Add(selectedPath);
    m_Handler->SetValuesFor(wxT("MOC_DIR"),values);

    wxTextCtrl *choiceUicDir = XRCCTRL(*this, "ID_LOC_UIC_TEXTCTRL", wxTextCtrl);
    selectedPath = choiceUicDir->GetValue();
    selectedPath.Trim();
    selectedPath.Trim(false);
    values.Clear();
    values.Add(selectedPath);
    m_Handler->SetValuesFor(wxT("UI_DIR"),values);

    wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);
    selectedPath = choiceRccDir->GetValue();
    selectedPath.Trim();
    selectedPath.Trim(false);
    values.Clear();
    values.Add(selectedPath);
    m_Handler->SetValuesFor(wxT("RCC_DIR"),values);

    wxTextCtrl *configChoices = XRCCTRL(*this, "ID_CONFIG_VARIABLES", wxTextCtrl);
    values.Clear();
    values = wxStringTokenize(configChoices->GetValue(), wxT(" "));
    wxArrayString configsToRemove = m_ExtraConfigurations;
    for (size_t i=0; i < values.GetCount(); i++)
    {
        m_Handler->Add(wxT("CONFIG"),values[i]);
        if (configsToRemove.Index(values[i]))
        {
            configsToRemove.Remove(values[i]);
        }
    }
    for (size_t i=0; i < configsToRemove.GetCount(); i++)
    {
        m_Handler->Remove(wxT("CONFIG"),configsToRemove[i]);
    }
}

void qtwProjectOptions::OnBrowseMocButtonClick(wxCommandEvent& event)
{
    wxString targetDir = CurrentActiveProject()->GetBasePath() +
                         XRCCTRL(*this, "ID_TARGET_LISTBOX", wxListBox)->GetStringSelection();
    wxString mocDir = ChooseDirectory(this,
                                      _("Select moc output directory (relative to target directory)"),
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
                                      _("Select uic output directory (relative to target directory)"),
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
                                      wxT("Select rcc output directory (relative to target directory)"),
                                      targetDir,
                                      targetDir,
                                      true,
                                      true);
    wxTextCtrl *choiceRccDir = XRCCTRL(*this, "ID_LOC_RCC_TEXTCTRL", wxTextCtrl);
    choiceRccDir->SetValue(rccDir);
}

void qtwProjectOptions::OnTargetListClick(wxCommandEvent& event)
{
    Update();
    m_Handler->Write();

    UpdateTarget();
    PopulateWorld();
}

void qtwProjectOptions::EndModal(int retCode)
{
    Update();
    m_Handler->Write();
    wxDialog::EndModal(retCode);
}

