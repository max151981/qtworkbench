/***************************************************************
 * Name:      qtwprojectoptions.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include <wx/dialog.h>

class cbProject;
class QtWProjectHandler;
class QtWProjectOptions : public wxDialog
{
public:
    QtWProjectOptions(wxWindow* parent);
    ~QtWProjectOptions();
private:
    cbProject* CurrentActiveProject();

    void PopulateTargetsListBox();
    void PopulateWorld();
    void PopulateBuildMode();
    void PopulateRequirements();
    void PopulateModules();
    void PopulateFileLocations();
    void PopulateVariablesList();
    void PopulateValuesList();

    void OnBrowseMocButtonClick(wxCommandEvent&);
    void OnBrowseUicButtonClick(wxCommandEvent&);
    void OnBrowseRccButtonClick(wxCommandEvent&);
    void OnAddValue(wxCommandEvent&);
    void OnAddVariable(wxCommandEvent&);
    void OnRemoveValue(wxCommandEvent&);
    void OnRemoveVariable(wxCommandEvent&);
    void OnTargetListClick(wxCommandEvent&);
    void OnNotebookPageChange(wxNotebookEvent&);
    void OnUpdateAdvancedView(wxCommandEvent&);
    void OnUsingQtWorkbench(wxCommandEvent&);

    void EndModal(int retCode);
    void Update();
    void UpdateTarget();

    QtWProjectHandler *m_Handler;
    wxArrayString m_ExtraConfigurations;
    DECLARE_EVENT_TABLE()
};
