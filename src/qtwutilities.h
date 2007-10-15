/***************************************************************
 * Name:      qtwmkspecbrowser.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#ifndef QTWUTILITIES_H
#define QTWUTILITIES_H

#include <wx/dir.h>

class QtWMkSpecBrowser : public wxDirTraverser {
public:
    QtWMkSpecBrowser(wxArrayString& dirs) : mkspec_dirs(dirs) {}

    virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename)) {
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& dirname) {
        mkspec_dirs.Add(dirname);
        return wxDIR_CONTINUE;
    }

private:
    wxArrayString& mkspec_dirs;
};

WX_DECLARE_STRING_HASH_MAP(bool, QMakeEnabledProjectsMap);

#endif
