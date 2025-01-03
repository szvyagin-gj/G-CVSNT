// TortoiseCVS - a Windows shell extension for easy version control

// Copyright (C) 2005 - March Hare Software Ltd
// <arthur.barrett@march-hare.com> - July 2005

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef REPO_TREECTRL_H
#define REPO_TREECTRL_H

#include <wx/treectrl.h>
#include "treelistctrl.h"
#include "../Utils/RepoUtils.h"


class ModuleBasicsPage;

class TreeItemData : public wxTreeItemData
{
public:
    TreeItemData(int index)
       : myIndex(index)
    {
    }

    int GetIndex() const { return myIndex; }

private:
   int myIndex;
};


class RepoTreeCtrl : public wxTreeListCtrl
{
public:
   // Needed for IMPLEMENT_DYNAMIC_CLASS()
   RepoTreeCtrl() {}
   
   RepoTreeCtrl(wxWindow* parent, const wxWindowID id,
                   const wxPoint& pos, const wxSize& size,
                   long style);
   virtual ~RepoTreeCtrl();

   void SetSortParams(bool ascending, int key);

private:
   virtual int OnCompareItems(const wxTreeItemId& item1,
                              const wxTreeItemId& item2);

   DECLARE_DYNAMIC_CLASS(RepoTreeCtrl)
};

#endif
