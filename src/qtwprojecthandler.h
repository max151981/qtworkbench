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

    wxArrayString GetValuesFor(const wxString &identifier);
    void SetValuesFor(const wxString &identifier, const wxArrayString& contents);

    bool Contains(const wxString &identifier, const wxString& value);
    void Add(const wxString &identifier, const wxString& value);
    void Remove(const wxString &identifier, const wxString& value);

    private:
    wxString m_FileName;

    WX_DECLARE_STRING_HASH_MAP(wxString, wxMap);
    wxMap m_VariableMap;
};

#endif

