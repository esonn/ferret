#if !defined unique_view_h
#define unique_view_h

/* Copyright (c) Peter Lane, 2013 */

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "documentlist.h"
#include "documentview.h"

enum { ID_RANK_F = wxID_HIGHEST + 10,
  ID_RANK_U
};

class UniqueTrigramsView;
class UniqueTrigramsListCtrl: public wxListCtrl
{
  public:
    UniqueTrigramsListCtrl (UniqueTrigramsView * ferretparent, wxPanel * parent)
      : wxListCtrl (parent, wxID_ANY,
          wxDefaultPosition, wxDefaultSize,
          wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL | wxSIMPLE_BORDER ),
      _ferretparent (ferretparent)
  {}
  int GetNumberItems () const;
  wxString OnGetItemText (long item, long column) const;
  private:
    UniqueTrigramsView * _ferretparent;
};

class UniqueTrigramsView: public wxFrame
{
  public:
    UniqueTrigramsView (ComparisonTableView * parent, DocumentList & documentlist);
    void OnClose (wxCloseEvent & event);
    DocumentList & GetDocumentList ()
		{ 
			return _documentlist;
		}
  private:
    DocumentList & _documentlist;
    UniqueTrigramsListCtrl * _uniqueObserver;
//    DECLARE_EVENT_TABLE()
};

#endif

