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
    wxString currentOperator(wxT(""));
    wxArrayStringMap currentContents;
    const wxChar EQ('=');
    const wxChar AND('+');
    const wxChar OR('*');
    const wxChar NOT('~');
    bool end=false;
    for (wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
    {
        if (end)
        {
            //FIXME what if double entries for current identifier and current operator?
            if (!currentIdentifier.IsEmpty() && !currentContents.empty())
            {
                m_VariableMap[currentIdentifier]=currentContents;
            }
            currentContents.clear();
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
                continue; //Empty line
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
                    currentOperator.Append(EQ);
                    token.Remove(token.Len()-1);
                    if (token[token.Len()-1] == AND ||
                            token[token.Len()-1] == OR ||
                            token[token.Len()-1] == NOT)
                    {
                        currentOperator.Append(token[token.Len()-1]);
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
                currentOperator = token;
                continue;
            }
            if (token != wxString(lineContinueChar))
            {
                currentContents[currentOperator].Add(token);
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
        if (!currentIdentifier.IsEmpty() && !currentContents.empty())
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
    QMakeVariablesMap::iterator it;
    size_t lineNumber=0;
    for ( it = m_VariableMap.begin(); it != m_VariableMap.end(); ++it )
    {
        wxString key = it->first;
        wxArrayStringMap values = it->second;

        wxArrayStringMap::iterator valueit;
        for (valueit = values.begin(); valueit != values.end(); ++valueit)
        {
            wxString qmakeOperator = valueit->first;
            wxArrayString value = valueit->second;

            wxString line = key + wxT(" ") + qmakeOperator;
            for (size_t i=0; i<value.GetCount(); i++)
            {
                line << wxT(" ");
                line << value[i];
            }
            file.InsertLine(line,lineNumber);
            lineNumber++;
        }
    }
    if (lineNumber)
    {
        // Parser needs an empty line in the end
        file.InsertLine(wxT(""),lineNumber);
        return file.Write();
    }
    return true;
}

wxArrayString QtWProjectHandler::GetAvailableVariables()
{
    wxArrayString variables;
    QMakeVariablesMap::iterator it;
    for ( it = m_VariableMap.begin(); it != m_VariableMap.end(); ++it )
    {
        wxString key = it->first;
        variables.Add(key);
    }
    return variables;
}

wxArrayString QtWProjectHandler::GetValuesFor(const wxString &identifier,const wxString &qmakeOperator)
{
    wxArrayStringMap values = m_VariableMap[identifier];
    return values[qmakeOperator];
}

void QtWProjectHandler::SetValuesFor(const wxString &identifier, const wxArrayString& contentsArray,const wxString &qmakeOperator)
{
    wxArrayStringMap values = m_VariableMap[identifier];
    wxArrayString current = values[qmakeOperator];
    if (!current.GetCount() && !contentsArray.GetCount())
    {
        return;
    }
    values[qmakeOperator] = contentsArray;
    m_VariableMap[identifier] = values;
}

bool QtWProjectHandler::Contains(const wxString &identifier, const wxString& value, const wxString &qmakeOperator)
{
    wxArrayStringMap values = m_VariableMap[identifier];
    wxArrayString contentsArray = values[qmakeOperator];
    return (contentsArray.Index(value) != wxNOT_FOUND);
}

void QtWProjectHandler::Add(const wxString &identifier, const wxString& value, const wxString &qmakeOperator)
{
    if (value.IsEmpty())
    {
        return;
    }

    wxArrayString values = GetValuesFor(identifier,qmakeOperator);
    if (values.Index(value) != wxNOT_FOUND)
    {
        return;
    }

    values.Add(value);
    SetValuesFor(identifier,values,qmakeOperator);
}

void QtWProjectHandler::Remove(const wxString &identifier, const wxString& value, const wxString &qmakeOperator)
{
    wxArrayString values = GetValuesFor(identifier,qmakeOperator);
    values.Remove(value);
    SetValuesFor(identifier,values,qmakeOperator);
}

