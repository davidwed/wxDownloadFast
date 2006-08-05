//
// C++ Implementation: Metalink
//
// Description: Implements the Metalink Class
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

bool mMetalinkDocument::GetMetalinkData(mMetalinkData *data)
{
    bool result = FALSE;
    wxXmlNode *rootnode;
    if (IsOk())
    {
        rootnode = this->GetRoot();
        if (rootnode->GetName().Lower() == wxT("metalink"))
        {
            wxXmlNode *node = rootnode->GetChildren();
            while (node)
            {
                if (node->GetName().Lower() == wxT("publisher"))
                {
                    wxXmlNode *subnode = node->GetChildren();
                    while (subnode)
                    {
                        if (subnode->GetName().Lower() == wxT("name"))
                            data->publishername = subnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                        else if (subnode->GetName().Lower() == wxT("url"))
                            data->publisherurl = subnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                        subnode = subnode->GetNext();
                    }
                }
                else if (node->GetName().Lower() == wxT("description"))
                {
                    data->description = node->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                }
                else if (node->GetName().Lower() == wxT("files"))
                {
                    wxXmlNode *subnode = node->GetChildren();
                    while (subnode)
                    {
                        if (subnode->GetName().Lower() == wxT("file"))
                        {
                            data->filename = subnode->GetPropVal(wxT("name"),wxT("")).Trim(TRUE).Trim(FALSE);
                            wxXmlNode *subsubnode = subnode->GetChildren();
                            while (subsubnode)
                            {
                                if (subsubnode->GetName().Lower() == wxT("version"))
                                    data->version = subsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                                else if (subsubnode->GetName().Lower() == wxT("size"))
                                    data->size = MyUtilFunctions::wxstrtolonglong(subsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE));
                                else if (subsubnode->GetName().Lower() == wxT("language"))
                                    data->language = subsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                                else if (subsubnode->GetName().Lower() == wxT("os"))
                                    data->os = subsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                                else if (subsubnode->GetName().Lower() == wxT("verification"))
                                {
                                    wxXmlNode *subsubsubnode = subsubnode->GetChildren();
                                    while (subsubsubnode)
                                    {
                                        if (subsubsubnode->GetName().Lower() == wxT("hash"))
                                        {
                                            if (subsubsubnode->GetPropVal(wxT("type"),wxEmptyString).Lower() == wxT("md5"))
                                                data->md5 = subsubsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                                            else if (subsubsubnode->GetPropVal(wxT("type"),wxEmptyString).Lower() == wxT("sha1"))
                                                data->sha1 = subsubsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE);
                                        }
                                        subsubsubnode = subsubsubnode->GetNext();
                                    }
                                }
                                else if (subsubnode->GetName().Lower() == wxT("resources"))
                                {
                                    wxXmlNode *subsubsubnode = subsubnode->GetChildren();
                                    while (subsubsubnode)
                                    {
                                        if (subsubsubnode->GetName().Lower() == wxT("url"))
                                        {
                                            mUrlName *urltmp = new mUrlName(subsubsubnode->GetChildren()->GetContent().Trim(TRUE).Trim(FALSE));
                                            if ((urltmp->IsComplete()) && (!urltmp->GetFullRealName().Contains(wxT(".torrent"))))
                                            {
                                                data->urllist.Append(urltmp);
                                                result = TRUE;
                                            }
                                        }
                                        subsubsubnode = subsubsubnode->GetNext();
                                    }
                                }
                                subsubnode = subsubnode->GetNext();
                            }
                        }
                        subnode = subnode->GetNext();
                    }
                }

                node = node->GetNext();
            }

        }
    }

    return result;
}

void mMetalinkData::Clear()
{
    publishername.Clear();
    publisherurl.Clear();
    description.Clear();
    filename.Clear();
    version.Clear();
    size = 0;
    language.Clear();
    os.Clear();
    md5.Clear();
    sha1.Clear();
    urllist.Clear();
}
