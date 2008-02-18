/***************************************************************
 * Name:      QtWConfiguration.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include <sdk.h>
#include "qtwutilities.h"
#include "qtwconfiguration.h"

BEGIN_EVENT_TABLE(QtWConfiguration, wxPanel)
EVT_BUTTON(XRCID("ID_BUTTON_QTDIR"),QtWConfiguration::OnBrowseQtDir)
EVT_BUTTON(XRCID("ID_BUTTON_QMAKESPEC"),QtWConfiguration::OnBrowseQtMakeSpec)
EVT_TEXT(XRCID("ID_TEXTCTRL_QMAKESPEC"), QtWConfiguration::OnReReadQMakeSpecs)
END_EVENT_TABLE()

QtWConfiguration::QtWConfiguration(wxWindow* parent)
{
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgQtWConfig"));
    LoadSettings();
}

QtWConfiguration::~QtWConfiguration()
{}

void QtWConfiguration::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("qtwb"));
    cfg->Write(wxT("/QtDir"), XRCCTRL(*this, "ID_TEXTCTRL_QTDIR", wxTextCtrl)->GetValue());
    cfg->Write(wxT("/QMkSpecDir"), XRCCTRL(*this, "ID_TEXTCTRL_QMAKESPEC", wxTextCtrl)->GetValue());
    cfg->Write(wxT("/QMkSpec"), XRCCTRL(*this, "ID_CHOICE_MKSPEC", wxChoice)->GetStringSelection());
}

void QtWConfiguration::LoadSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("qtwb"));
    XRCCTRL(*this, "ID_TEXTCTRL_QTDIR", wxTextCtrl)->SetValue(cfg->Read(_T("/QtDir")));
    wxString qmakespecsDir = cfg->Read(_T("/QMkSpecDir"));
    XRCCTRL(*this, "ID_TEXTCTRL_QMAKESPEC", wxTextCtrl)->SetValue(qmakespecsDir);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(qmakespecsDir);
    PopulateMkSpecs(qmakespecsDir);
    XRCCTRL(*this, "ID_CHOICE_MKSPEC", wxChoice)->SetStringSelection(cfg->Read(_T("/QMkSpec")));
}

void QtWConfiguration::OnBrowseQtDir(wxCommandEvent& event)
{
    wxTextCtrl *choiceQtDir = XRCCTRL(*this, "ID_TEXTCTRL_QTDIR", wxTextCtrl);
    wxString targetDir = choiceQtDir->GetValue();
    wxString QtDir = ChooseDirectory(this,
                                     _("Select your Qt installation directory"),
                                     targetDir,
                                     targetDir,
                                     false,
                                     true);
    if (!QtDir.IsEmpty())
    {
        choiceQtDir->SetValue(QtDir);
    }
}

void QtWConfiguration::OnBrowseQtMakeSpec(wxCommandEvent& event)
{
    wxTextCtrl *choiceQMakeSpecDir = XRCCTRL(*this, "ID_TEXTCTRL_QMAKESPEC", wxTextCtrl);
    wxString targetDir = choiceQMakeSpecDir->GetValue();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(targetDir);
    wxString QMakeSpecDir = ChooseDirectory(this,
                                            _("Select your Qt mkspec directory (generally the mkspecs directory under your Qt installation directory)"),
                                            targetDir,
                                            targetDir,
                                            false,
                                            true);
    if (!QMakeSpecDir.IsEmpty())
    {
        choiceQMakeSpecDir->SetValue(QMakeSpecDir);
        PopulateMkSpecs(QMakeSpecDir);
    }
}

void QtWConfiguration::OnReReadQMakeSpecs(wxCommandEvent& event){
    wxString targetDir = XRCCTRL(*this, "ID_TEXTCTRL_QMAKESPEC", wxTextCtrl)->GetValue();
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(targetDir);
    PopulateMkSpecs(targetDir);
}

void QtWConfiguration::PopulateMkSpecs(const wxString &QMakeSpecDir)
{
    wxChoice *availableMkSpecs = XRCCTRL(*this, "ID_CHOICE_MKSPEC", wxChoice);
    if (!wxDir::Exists(QMakeSpecDir))
    {
        availableMkSpecs->Clear();
        return;
    }

    wxArrayString mkspecsArray;
    QtWMkSpecBrowser browser(mkspecsArray);

    wxDir QtMkspecDir(QMakeSpecDir);
    QtMkspecDir.Traverse(browser);

    wxString keptSelection = availableMkSpecs->GetStringSelection();
    availableMkSpecs->Clear();
    for (unsigned int index = 0; index < mkspecsArray.GetCount(); index++)
    {
        if (wxFile::Exists(mkspecsArray[index] + wxFileName::GetPathSeparator() + wxT("qmake.conf")))
        {
            wxString currentSelection = mkspecsArray[index].Remove(0,QMakeSpecDir.Length()+1);
            availableMkSpecs->Append(currentSelection);
        }
    }
    if (keptSelection.IsEmpty())
    {
        availableMkSpecs->SetStringSelection(wxT("default"));
    }
    else if (!availableMkSpecs->SetStringSelection(keptSelection))
    {
        availableMkSpecs->SetStringSelection(wxT("default"));
    }
}
