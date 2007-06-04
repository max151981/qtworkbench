/***************************************************************
 * Name:      qtwprojecthandler.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yorgos Pagles<y.pagles@gmail.com>
 * Copyright: (c) Yorgos Pagles
 * License:   GPL
 **************************************************************/

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#include "qtwprojecthandler.h"

QtWProjectHandler::QtWProjectHandler()
{
    m_FileName = wxT("");
    m_VariableMap.clear();
}

QtWProjectHandler::~QtWProjectHandler()
{}

void QtWProjectHandler::SetFilename(const wxString &filename)
{
    m_FileName = filename;
}

bool QtWProjectHandler::Read()
{
    m_VariableMap.clear();

    wxTextFile file(m_FileName);
    if (!file.Exists())
    {
        return true;
    }
    if (!file.Open())
    {
        return false;
    }

    wxString currentIdentifier(wxT(""));
    wxString currentContents(wxT(""));
    const wxChar EQ('=');
    const wxChar AND('+');
    const wxChar OR('*');
    const wxChar NOT('~');
    bool end=false;
    for (wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
    {
        if (end)
        {
            //FIXME what if double entries?
            if (!currentIdentifier.IsEmpty() && !currentContents.IsEmpty())
            {
                m_VariableMap[currentIdentifier]=currentContents;
            }
            currentContents= wxT("");
            currentIdentifier = wxT("");
            end=false;
        }
        str.Trim();
        str.Trim(false);
        if (str.IsEmpty())
        {
            continue; // Empty line
        }
        wxChar lineContinueChar('\\');
        wxStringTokenizer tkz(str, wxT(" \t\r\n"));
        while ( tkz.HasMoreTokens() )
        {
            wxString token = tkz.GetNextToken();
            if (token.IsEmpty())
            {
                continue; // Probably empty line
            }
            wxChar commentChar('#');
            if (token[0] == commentChar)
            {
                // the rest of the line is a comment
                break;
            }
            if (currentIdentifier.IsEmpty())
            {
                if (token[token.Len()-1] == EQ)
                {
                    token.Remove(token.Len()-1);
                    if (token[token.Len()-1] == AND ||
                            token[token.Len()-1] == OR ||
                            token[token.Len()-1] == NOT)
                    {
                        token.Remove(token.Len()-1);
                    }
                }
                currentIdentifier = token;
                continue;
            }
            if (token == wxString(EQ) ||
                    token == wxString(wxString(AND) + wxString(EQ)) ||
                    token == wxString(wxString(OR) + wxString(EQ)) ||
                    token == wxString(wxString(NOT) + wxString(EQ)))
            {
                continue;
            }
            if (token != wxString(lineContinueChar))
            {
                currentContents.Append(wxT("$"));
                currentContents.Append(token);
            }
        }
        if (str[str.Len()-1] != lineContinueChar)
        {
            end=true;
        }
    }

    // Get the last entry
    if (end)
    {
        if (!currentIdentifier.IsEmpty() && !currentContents.IsEmpty())
        {
            m_VariableMap[currentIdentifier]=currentContents;
        }
    }
    return file.Close();
}

bool QtWProjectHandler::Write()
{
    wxTextFile file(m_FileName);

    if (file.Exists())
    {
        if (!file.Open(m_FileName))
        {
            return false;
        }
    }
    else if (!file.Create(m_FileName))
    {
        return false;
    }

    file.Clear();
    wxMap::iterator it;
    size_t lineNumber=0;
    for ( it = m_VariableMap.begin(); it != m_VariableMap.end(); ++it )
    {
        wxString key = it->first, value = it->second;
        value.Replace(wxT("$"),wxT(" "));
        wxString line = key + wxT(" = ") + value;
        file.InsertLine(line,lineNumber);
        lineNumber++;
    }
    if (lineNumber)
        return file.Write();
    return true;
}

wxArrayString QtWProjectHandler::GetValuesFor(const wxString &identifier)
{
    wxString valuesString = m_VariableMap[identifier];
    valuesString.Trim();
    valuesString.Trim(false);
    return wxStringTokenize(valuesString, wxT("$"));
}

void QtWProjectHandler::SetValuesFor(const wxString &identifier, const wxArrayString& contentsArray)
{
    wxString contents;
    for (size_t i=0; i< contentsArray.GetCount(); i++)
    {
        wxString content=contentsArray[i];
        content.Trim();
        content.Trim(false);

        contents.Append(content);
        contents.Append(wxT("$"));
    }
    m_VariableMap[identifier] = contents;
}

bool QtWProjectHandler::Contains(const wxString &identifier, const wxString& value)
{
    wxArrayString values = GetValuesFor(identifier);

    if (values.Index(value) != wxNOT_FOUND)
    {
        return true;
    }

    return false;
}

void QtWProjectHandler::Add(const wxString &identifier, const wxString& value)
{
    if (value.IsEmpty())
    {
        return;
    }

    wxArrayString values = GetValuesFor(identifier);
    if (values.Index(value) != wxNOT_FOUND)
    {
        return;
    }

    values.Add(value);
    SetValuesFor(identifier,values);
}

void QtWProjectHandler::Remove(const wxString &identifier, const wxString& value)
{
    wxArrayString values = GetValuesFor(identifier);
    values.Remove(value);
    SetValuesFor(identifier,values);
}

