#if !defined engagement_view_h
#define engagement_view_h

/* Copyright (c) Peter Lane, 2013 */

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "documentlist.h"
#include "resultstable.h"

enum { ID_ENGAGEMENT_RANK_F = wxID_HIGHEST + 1000,
  ID_ENGAGEMENT_RANK_U
};

class EngagementTrigramsView;
class EngagementTrigramsListCtrl: public wxListCtrl
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
    EngagementTrigramsListCtrl (EngagementTrigramsView * ferretparent, wxPanel * parent);
    void SortOnDocument ();
    void SortOnCount ();
    void OnSortColumn (wxListEvent & event);
    int GetItemCount () const;
    float MeanCount () const;
    wxString OnGetItemText (long item, long column) const;
  private:
    DECLARE_EVENT_TABLE()
    EngagementTrigramsView * _ferretparent;
    std::vector<int> _sortedIndices;
    Sort _lastsort;
};

class ComparisonTableView; // forward declaration, for compiling documentview.h
class EngagementTrigramsView: public wxFrame
{
  public:
    EngagementTrigramsView (ComparisonTableView * parent, DocumentList & documentlist);
    void OnRankDoc (wxCommandEvent & event);
    void OnRankCount (wxCommandEvent & event);
    void OnHelp (wxCommandEvent & event);
    void OnClose (wxCloseEvent & event);
    void OnCloseEvent (wxCommandEvent & WXUNUSED(event));
    void OnResize (wxSizeEvent & event);
    DocumentList & GetDocumentList ()
		{ 
			return _documentlist;
		}
  private:
    DocumentList & _documentlist;
    EngagementTrigramsListCtrl * _engagementObserver;
    DECLARE_EVENT_TABLE()
};

#endif

