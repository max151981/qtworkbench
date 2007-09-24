/***************************************************************
 * Name:      qtwprojectoptions.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/
#include "configurationpanel.h"
#include "qtwutilities.h"

class cbProject;
class QtWProjectHandler;
class QtWProjectOptions : public cbConfigurationPanel
{
public:
    QtWProjectOptions(wxWindow* parent, cbProject* project, QMakeEnabledProjectsMap &enabledProjects);
    ~QtWProjectOptions();

    virtual wxString GetTitle() const
    {
        return _("Qt Workbench");
    }
    virtual wxString GetBitmapBaseName() const
    {
        return _T("generic-plugin");
    }
    virtual void OnApply()
    {
        SaveSettings();
    }
    virtual void OnCancel()
    {}
private:

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

    void SaveSettings();
    void Update();
    void ReadTargets();
    void UpdateTarget();

    cbProject* m_Project;
    QtWProjectHandler *m_Handler;
    QMakeEnabledProjectsMap &m_EnabledProjects;

    WX_DECLARE_STRING_HASH_MAP(QtWProjectHandler *, QtWProjectHandlersMap);
    QtWProjectHandlersMap m_TargetHandlers;

    DECLARE_EVENT_TABLE()
};
