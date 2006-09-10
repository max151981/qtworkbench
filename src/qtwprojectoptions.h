/***************************************************************
 * Name:      qtwprogenerator.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include <wx/dialog.h>

class cbProject;

class qtwProjectOptions : public wxDialog
{
public:
    qtwProjectOptions(wxWindow* parent);
    ~qtwProjectOptions();
private:
    cbProject* CurrentActiveProject();

    void PopulateTargetsListBox();
    void PopulateTargetsArrayString();
    void PopulateWorld();
    void PopulateBuildMode(const wxString&);
    void PopulateRequirements(const wxString&);
    void PopulateModules(const wxString&);
    void PopulatePluginMode(const wxString&);
    void PopulateFileLocations(const wxString&);

    void OnNeedsUpdate(wxCommandEvent&);
    void OnBrowseMocButtonClick(wxCommandEvent&);
    void OnBrowseUicButtonClick(wxCommandEvent&);
    void OnBrowseRccButtonClick(wxCommandEvent&);
    void OnTargetListClick(wxCommandEvent&);

    void EndModal(int retCode);
    wxString GetListedProjectFileContents(int);
    void UpdateQtTargetSettings();
    void SaveQtProjectSettings();

    wxArrayString* m_AllProjectOptions;
    DECLARE_EVENT_TABLE()
};
