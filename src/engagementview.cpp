#include "engagementview.h"

wxButton * EMakeButton (wxWindow * parent, int id, wxString title, wxString tooltip, bool enabled=true)
{
	wxButton * button = new wxButton (parent, id, title);
	if (!tooltip.IsEmpty ()) button->SetToolTip (tooltip);
	button->Enable (enabled);
	return button;
}

BEGIN_EVENT_TABLE(EngagementTrigramsView, wxFrame)
  EVT_BUTTON(ID_ENGAGEMENT_RANK_F, EngagementTrigramsView::OnRankDoc)
  EVT_BUTTON(ID_ENGAGEMENT_RANK_U, EngagementTrigramsView::OnRankCount)
  EVT_BUTTON(wxID_HELP, EngagementTrigramsView::OnHelp)
  EVT_CLOSE(EngagementTrigramsView::OnClose)
  EVT_SIZE(EngagementTrigramsView::OnResize)
  EVT_BUTTON(wxID_CLOSE, EngagementTrigramsView::OnCloseEvent)
END_EVENT_TABLE()

void EngagementTrigramsView::OnRankDoc (wxCommandEvent & WXUNUSED(event))
{
  _engagementObserver->SortOnDocument ();
}

void EngagementTrigramsView::OnRankCount (wxCommandEvent & WXUNUSED(event))
{
  _engagementObserver->SortOnCount ();
}

void EngagementTrigramsView::OnHelp (wxCommandEvent & WXUNUSED(event))
{
  wxGetApp().ShowEngagementHelp ();
}

EngagementTrigramsView::EngagementTrigramsView (ComparisonTableView * parent, DocumentList & documentlist)
  : wxFrame((wxFrame *)parent, wxID_ANY, "Ferret: Engagement View",
    wxGetApp().GetNextFramePosition (650, 350), 
    wxSize (650, 350)),
  _documentlist (documentlist)
{
  // 1. comparison table -- set up so engagementObserver can be used in display
  wxPanel * tableView = new wxPanel (this, wxID_ANY);
  wxBoxSizer * tableSizer = new wxBoxSizer (wxVERTICAL);
  tableView->SetSizer (tableSizer);

  _engagementObserver = new EngagementTrigramsListCtrl (this, tableView);
  tableSizer->Add (_engagementObserver, 1, wxGROW);

  // create the main part of the display
  CreateStatusBar (3);
  int widths [] = {-2, -1, -1};
  SetStatusWidths (3, widths);
  SetStatusText ("Engagement view", 0);
  if (_documentlist.IsGrouped ())
  {
    SetStatusText (wxString::Format ("Groups: %d", _engagementObserver->GetItemCount ()), 1);
  }
  else
  {
    SetStatusText (wxString::Format ("Documents: %d", _engagementObserver->GetItemCount ()), 1);
  }

  // set up internal widgets
  wxBoxSizer * topsizer = new wxBoxSizer (wxHORIZONTAL);

  // return to the table view
  // -- insert two columns
  wxListItem itemCol;
  itemCol.SetText (_documentlist.IsGrouped() ? "Group" : "Document");
  _engagementObserver->InsertColumn (0, itemCol);
  _engagementObserver->SetColumnWidth (0, wxLIST_AUTOSIZE_USEHEADER);
  itemCol.SetText ("Count");
  _engagementObserver->InsertColumn (1, itemCol);
  _engagementObserver->SetColumnWidth (1, wxLIST_AUTOSIZE_USEHEADER);
  _engagementObserver->SetItemCount (_engagementObserver->GetItemCount ());

  // 2. buttons
  wxBoxSizer * buttonSizer = new wxBoxSizer (wxVERTICAL);
	wxButton * rank_f = EMakeButton (this, ID_ENGAGEMENT_RANK_F, (_documentlist.IsGrouped() ? "Group" : "Document"),
				"Put table into alphabetical order of name");
	wxButton * rank_u = EMakeButton (this, ID_ENGAGEMENT_RANK_U, "Count",
				"Put table into order with largest engagement count at top");
	wxStaticBoxSizer * rankSizer = new wxStaticBoxSizer (wxVERTICAL, this, "Rearrange table by");
	rankSizer->Add (rank_f, 0, wxGROW | wxALL, 5);
	rankSizer->Add (rank_u, 0, wxGROW | wxALL, 5);
	buttonSizer->Add (rankSizer, 0, wxGROW);

	buttonSizer->AddStretchSpacer (); // separate window controls from Ferret controls
	buttonSizer->Add (new wxButton (this, wxID_HELP), 0, wxGROW | wxALL, 5);
	buttonSizer->Add (new wxButton (this, wxID_CLOSE), 0, wxGROW | wxALL, 5);

  topsizer->Add (buttonSizer, 0, wxGROW | wxALL, 5);
  topsizer->Insert (0, tableView, 1, wxGROW);

	// compute best minimum height, and constrain window
	int best_height = buttonSizer->GetMinSize().GetHeight () + 40; // allow for space between widgets
	SetSizeHints (best_height, best_height); // keep the minimum size a square shape

  SetSizer (topsizer);
  _engagementObserver->SetFocus ();
  _engagementObserver->SortOnCount ();
  SetStatusText (wxString::Format ("Mean: %f", _engagementObserver->MeanCount ()), 2);
}

void EngagementTrigramsView::OnClose (wxCloseEvent & WXUNUSED(event))
{
  Destroy ();
}

void EngagementTrigramsView::OnCloseEvent (wxCommandEvent & WXUNUSED(event))
{
  Destroy ();
}

// Set column widths of list control to fill out the available space on screen
void EngagementTrigramsView::OnResize (wxSizeEvent & event)
{
	if (_engagementObserver == NULL) return;  // just forget it, if there is nothing to observe

	wxPanel * button = (wxPanel *) FindWindow (ID_ENGAGEMENT_RANK_F); // pick a widget off frame for width

	// TODO: Get this to work without need for - 35
	int new_width = GetClientSize().GetWidth () - button->GetSize().GetWidth() - 35;
	int current_width = _engagementObserver->GetColumnWidth (0) + 
		_engagementObserver->GetColumnWidth (1);
	if (new_width > 0 && current_width > 0)
	{
		double scale_factor = (double)new_width / (double)current_width;
		for (int i = 0; i < 2; ++i)
		{
			int newcolwidth = (int)(_engagementObserver->GetColumnWidth (i) * scale_factor);
			_engagementObserver->SetColumnWidth (i, newcolwidth);
		}
	}
	event.Skip (); // process resize event for superclasses
}

BEGIN_EVENT_TABLE(EngagementTrigramsListCtrl, wxListCtrl)
  EVT_LIST_COL_CLICK (wxID_ANY, EngagementTrigramsListCtrl::OnSortColumn)
END_EVENT_TABLE()

EngagementTrigramsListCtrl::EngagementTrigramsListCtrl (EngagementTrigramsView * ferretparent, wxPanel * parent)
  : wxListCtrl (parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL | wxSIMPLE_BORDER ),
  _ferretparent (ferretparent)
{
  // create list of indices - position in list altered by sorting
  _sortedIndices.clear ();
  for (int i=0; i < _ferretparent->GetDocumentList().GroupSize (); i++)
  {
    if (!_ferretparent->GetDocumentList().IsTemplateMaterial (i)) 
    {
      _sortedIndices.push_back (i);
    }
  }
}

int EngagementTrigramsListCtrl::GetItemCount () const
{
  return _sortedIndices.size ();
}

float EngagementTrigramsListCtrl::MeanCount () const
{
  float total = 0.0;
  for (int i = 0, n = _sortedIndices.size (); i < n; i++)
  {
    total += _ferretparent->GetDocumentList().EngagementCount (_sortedIndices[i]);
  }

  return total / _sortedIndices.size ();
}

wxString EngagementTrigramsListCtrl::OnGetItemText (long item, long column) const
{
  int sorteditem = _sortedIndices[(int)item];

  switch (column)
  {
    case 0:
      return _ferretparent->GetDocumentList().GetGroupName (sorteditem);
    case 1:
      return wxString::Format("%d", _ferretparent->GetDocumentList().EngagementCount (sorteditem));
  }
}

std::vector<wxString> * EngagementTrigramsListCtrl::namecmp::names;

void EngagementTrigramsListCtrl::SortOnDocument ()
{
  if (_lastsort == sortDoc) return;
  _lastsort = sortDoc;

  std::vector<int> newIndices;

  for (int i = 0 ; i < _sortedIndices.size (); ++i)
  {
    newIndices.push_back (_sortedIndices[i]);
  }

  // cache the names
  std::vector<wxString> names;
  for (int i=0; i<_ferretparent->GetDocumentList().GroupSize (); i++)
  {
    names.push_back (_ferretparent->GetDocumentList().GetGroupName (i));
  }
  // set up sort
  namecmp comparer;
  comparer.names = &names;
  std::sort (newIndices.begin(), newIndices.end (), comparer);
  _sortedIndices = newIndices;
  RefreshItems (0, _sortedIndices.size()-1);
  _ferretparent->SetStatusText ("Rearranged table by name", 0);

}

void EngagementTrigramsListCtrl::SortOnCount ()
{
  if (_lastsort == sortCount) return;
  _lastsort = sortCount;
  
  std::vector<int> newIndices;
 
  // initialise the set of new indices
  for (int i = 0; i < _sortedIndices.size (); ++i)
  {
    newIndices.push_back (_sortedIndices[i]);
  }
  
  std::sort (newIndices.begin(), newIndices.end(), 
      _ferretparent->GetDocumentList().GetEngagementCountComparer ());

  _sortedIndices = newIndices;
  RefreshItems (0, _sortedIndices.size()-1);
  _ferretparent->SetStatusText ("Rearranged table by engagement count", 0);
}

void EngagementTrigramsListCtrl::OnSortColumn (wxListEvent & event)
{
  if (event.GetColumn () == 0)
  {
    SortOnDocument ();
  }
  else
  {
    SortOnCount ();
  }
}

