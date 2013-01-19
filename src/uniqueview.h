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
  enum Sort { sortDoc, sortCount };
  struct namecmp {
    static std::vector<wxString> * names;

    bool operator() (int x, int y) const
    {
      return ((*names)[x] < (*names)[y]);
    }
  };
  public:
    UniqueTrigramsListCtrl (UniqueTrigramsView * ferretparent, wxPanel * parent);
    void SortOnDocument ();
    void SortOnCount ();
    void OnSortColumn (wxListEvent & event);
    int GetNumberItems () const;
    wxString OnGetItemText (long item, long column) const;
  private:
    DECLARE_EVENT_TABLE()
    UniqueTrigramsView * _ferretparent;
    std::vector<int> _sortedIndices;
    Sort _lastsort;
};

class UniqueTrigramsView: public wxFrame
{
  public:
    UniqueTrigramsView (ComparisonTableView * parent, DocumentList & documentlist);
    void OnRankDoc (wxCommandEvent & event);
    void OnRankCount (wxCommandEvent & event);
    void OnClose (wxCloseEvent & event);
    void OnCloseEvent (wxCommandEvent & WXUNUSED(event));
    void OnResize (wxSizeEvent & event);
    DocumentList & GetDocumentList ()
		{ 
			return _documentlist;
		}
  private:
    DocumentList & _documentlist;
    UniqueTrigramsListCtrl * _uniqueObserver;
    DECLARE_EVENT_TABLE()
};

#endif

