/***************************************************************
 * Name:      qtwconfiguration.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#ifndef QTWCONFIGURATION_H
#define QTWCONFIGURATION_H

#include "configurationpanel.h"

class qtwConfiguration : public cbConfigurationPanel
{
	public:
		qtwConfiguration(wxWindow* parent);
		virtual ~qtwConfiguration();

	protected:
        virtual wxString GetTitle() const { return _("Qt Workbench"); }
        virtual wxString GetBitmapBaseName() const { return _T("generic-plugin"); }
        virtual void OnApply(){SaveSettings();}
        virtual void OnCancel(){}

        void OnBrowseQtMakeSpec(wxCommandEvent& event);
        void OnBrowseQtDir(wxCommandEvent& event);

        void LoadSettings();
        void SaveSettings();

        void PopulateMkSpecs(const wxString &QMakeSpecDir);
	private:
        DECLARE_EVENT_TABLE()
};

#endif
