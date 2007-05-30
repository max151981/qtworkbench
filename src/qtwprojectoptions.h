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
class qtwProjectOptions : public wxDialog
{
public:
    qtwProjectOptions(wxWindow* parent);
    ~qtwProjectOptions();
private:
    cbProject* CurrentActiveProject();

    void PopulateTargetsListBox();
    void PopulateWorld();
    void PopulateBuildMode();
    void PopulateRequirements();
    void PopulateModules();
    void PopulateFileLocations();

    void OnNeedsUpdate(wxCommandEvent&);
    void OnBrowseMocButtonClick(wxCommandEvent&);
    void OnBrowseUicButtonClick(wxCommandEvent&);
    void OnBrowseRccButtonClick(wxCommandEvent&);
    void OnTargetListClick(wxCommandEvent&);

    void EndModal(int retCode);
    void UpdateTarget();

    QtWProjectHandler *m_Handler;
    DECLARE_EVENT_TABLE()
};
