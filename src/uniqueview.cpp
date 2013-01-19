#include "uniqueview.h"

wxButton * UMakeButton (wxWindow * parent, int id, wxString title, wxString tooltip, bool enabled=true)
{
	wxButton * button = new wxButton (parent, id, title);
	if (!tooltip.IsEmpty ()) button->SetToolTip (tooltip);
	button->Enable (enabled);
	return button;
}

BEGIN_EVENT_TABLE(UniqueTrigramsView, wxFrame)
  EVT_BUTTON(ID_RANK_F, UniqueTrigramsView::OnRankDoc)
  EVT_BUTTON(ID_RANK_U, UniqueTrigramsView::OnRankCount)
  EVT_CLOSE(UniqueTrigramsView::OnClose)
  EVT_SIZE(UniqueTrigramsView::OnResize)
  EVT_BUTTON(wxID_CLOSE, UniqueTrigramsView::OnCloseEvent)
END_EVENT_TABLE()

void UniqueTrigramsView::OnRankDoc (wxCommandEvent & WXUNUSED(event))
{
  _uniqueObserver->SortOnDocument ();
}

void UniqueTrigramsView::OnRankCount (wxCommandEvent & WXUNUSED(event))
{
  _uniqueObserver->SortOnCount ();
}

UniqueTrigramsView::UniqueTrigramsView (ComparisonTableView * parent, DocumentList & documentlist)
  : wxFrame((wxFrame *)parent, wxID_ANY, wxT("Ferret: Unique Trigrams View"),
    wxGetApp().GetNextFramePosition (650, 350), 
    wxSize (650, 350)),
  _documentlist (documentlist)
{
  CreateStatusBar (2);
  int widths [] = {-2, -1};
  SetStatusWidths (2, widths);
  SetStatusText (wxT("Unique trigrams view"), 0);
  SetStatusText (wxString::Format (wxT("Documents: %d"), _documentlist.Size()), 1);

  // set up internal widgets
  wxBoxSizer * topsizer = new wxBoxSizer (wxHORIZONTAL);

  // 1. comparison table
  wxPanel * tableView = new wxPanel (this, wxID_ANY);
  wxBoxSizer * tableSizer = new wxBoxSizer (wxVERTICAL);
  tableView->SetSizer (tableSizer);

  _uniqueObserver = new UniqueTrigramsListCtrl (this, tableView);
  tableSizer->Add (_uniqueObserver, 1, wxGROW);

  // -- insert two columns
  wxListItem itemCol;
  itemCol.SetText (wxT("Document"));
  _uniqueObserver->InsertColumn (0, itemCol);
  _uniqueObserver->SetColumnWidth (0, wxLIST_AUTOSIZE_USEHEADER);
  itemCol.SetText (wxT("Count"));
  _uniqueObserver->InsertColumn (1, itemCol);
  _uniqueObserver->SetColumnWidth (1, wxLIST_AUTOSIZE_USEHEADER);
  _uniqueObserver->SetItemCount (_documentlist.Size ());

  // 2. buttons
  wxBoxSizer * buttonSizer = new wxBoxSizer (wxVERTICAL);
	wxButton * rank_f = UMakeButton (this, ID_RANK_F, wxT("Document"),
				wxT("Put table into alphabetical order of document"));
	wxButton * rank_u = UMakeButton (this, ID_RANK_U, wxT("Count"),
				wxT("Put table into order with largest unique count at top"));
	wxStaticBoxSizer * rankSizer = new wxStaticBoxSizer (wxVERTICAL, this, wxT("Rearrange table by"));
	rankSizer->Add (rank_f, 0, wxGROW | wxALL, 5);
	rankSizer->Add (rank_u, 0, wxGROW | wxALL, 5);
	buttonSizer->Add (rankSizer, 0, wxGROW);

	buttonSizer->AddStretchSpacer (); // separate window controls from Ferret controls
	buttonSizer->Add (new wxButton (this, wxID_CLOSE), 0, wxGROW | wxALL, 5);

  topsizer->Add (buttonSizer, 0, wxGROW | wxALL, 5);
  topsizer->Insert (0, tableView, 1, wxGROW);

	// compute best minimum height, and constrain window
	int best_height = buttonSizer->GetMinSize().GetHeight () + 40; // allow for space between widgets
	SetSizeHints (best_height, best_height); // keep the minimum size a square shape

  SetSizer (topsizer);
  _uniqueObserver->SetFocus ();
}

void UniqueTrigramsView::OnClose (wxCloseEvent & WXUNUSED(event))
{
  Destroy ();
}

void UniqueTrigramsView::OnCloseEvent (wxCommandEvent & WXUNUSED(event))
{
  Destroy ();
}

// Set column widths of list control to fill out the available space on screen
void UniqueTrigramsView::OnResize (wxSizeEvent & event)
{
	if (_uniqueObserver == NULL) return;  // just forget it, if there is nothing to observe

	wxPanel * button = (wxPanel *) FindWindow (ID_RANK_F); // pick a widget off frame for width

	// TODO: Get this to work without need for - 35
	int new_width = GetClientSize().GetWidth () - button->GetSize().GetWidth() - 35;
	int current_width = _uniqueObserver->GetColumnWidth (0) + 
		_uniqueObserver->GetColumnWidth (1);
	if (new_width > 0 && current_width > 0)
	{
		double scale_factor = (double)new_width / (double)current_width;
		for (int i = 0; i < 2; ++i)
		{
			int newcolwidth = (int)(_uniqueObserver->GetColumnWidth (i) * scale_factor);
			_uniqueObserver->SetColumnWidth (i, newcolwidth);
		}
	}
	event.Skip (); // process resize event for superclasses
}

BEGIN_EVENT_TABLE(UniqueTrigramsListCtrl, wxListCtrl)
  EVT_LIST_COL_CLICK (wxID_ANY, UniqueTrigramsListCtrl::OnSortColumn)
END_EVENT_TABLE()

UniqueTrigramsListCtrl::UniqueTrigramsListCtrl (UniqueTrigramsView * ferretparent, wxPanel * parent)
  : wxListCtrl (parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL | wxSIMPLE_BORDER ),
  _ferretparent (ferretparent)
{
  // create list of indices - position in list altered by sorting
  _sortedIndices.clear ();
  for (int i=0; i < _ferretparent->GetDocumentList().Size (); i++)
  {
    _sortedIndices.push_back (i);
  }
}

int UniqueTrigramsListCtrl::GetNumberItems () const
{
  _ferretparent->GetDocumentList().Size ();
}

// TODO: Include sorted indices
wxString UniqueTrigramsListCtrl::OnGetItemText (long item, long column) const
{
  int sorteditem = _sortedIndices[(int)item];

  switch (column)
  {
    case 0:
      return _ferretparent->GetDocumentList()[sorteditem]->GetName ();
    case 1:
      return wxString::Format("%d", _ferretparent->GetDocumentList()[sorteditem]->GetUniqueTrigramCount ());
  }
}

std::vector<wxString> * UniqueTrigramsListCtrl::namecmp::names;

void UniqueTrigramsListCtrl::SortOnDocument ()
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
  for (int i=0; i<_ferretparent->GetDocumentList().Size (); i++)
  {
    names.push_back (_ferretparent->GetDocumentList()[i]->GetName ());
  }
  // set up sort
  namecmp comparer;
  comparer.names = &names;
  std::sort (newIndices.begin(), newIndices.end (), comparer);
  _sortedIndices = newIndices;
  RefreshItems (0, _sortedIndices.size()-1);
  _ferretparent->SetStatusText (wxT("Rearranged table by document name"), 0);

}

void UniqueTrigramsListCtrl::SortOnCount ()
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
      _ferretparent->GetDocumentList().GetUniqueCountComparer ());

  _sortedIndices = newIndices;
  RefreshItems (0, _sortedIndices.size()-1);
  _ferretparent->SetStatusText (wxT("Rearranged table by uniqueness count"), 0);
}

void UniqueTrigramsListCtrl::OnSortColumn (wxListEvent & event)
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

