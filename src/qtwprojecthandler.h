/***************************************************************
 * Name:      qtwprojecthandler.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#ifndef QTWPROJECTHANDLER_H
#define QTWPROJECTHANDLER_H

class QtWProjectHandler
{
    public:
    QtWProjectHandler();
    ~QtWProjectHandler();

    void SetFilename(const wxString &filename);
    bool Read();
    bool Write();

    wxArrayString GetAvailableVariables();
    wxArrayString GetValuesFor(const wxString &identifier,const wxString &qmakeOperator);
    void SetValuesFor(const wxString &identifier, const wxArrayString& contents,const wxString &qmakeOperator);

    bool Contains(const wxString &identifier, const wxString& value,const wxString &qmakeOperator);
    void Add(const wxString &identifier, const wxString& value,const wxString &qmakeOperator);
    void Remove(const wxString &identifier, const wxString& value,const wxString &qmakeOperator);

    private:
    wxString m_FileName;

    WX_DECLARE_STRING_HASH_MAP(wxArrayString, wxArrayStringMap);
    WX_DECLARE_STRING_HASH_MAP(wxArrayStringMap, QMakeVariablesMap);
    QMakeVariablesMap m_VariableMap;

    WX_DECLARE_STRING_HASH_MAP(wxString, wxStringMap);
    //wxStringMap m_VariableMap;
};

#endif

