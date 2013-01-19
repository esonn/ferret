#include "uniqueview.h"

wxButton * UMakeButton (wxWindow * parent, int id, wxString title, wxString tooltip, bool enabled=true)
{
	wxButton * button = new wxButton (parent, id, title);
	if (!tooltip.IsEmpty ()) button->SetToolTip (tooltip);
	button->Enable (enabled);
	return button;
}

UniqueTrigramsView::UniqueTrigramsView (ComparisonTableView * parent, DocumentList & documentlist)
  : wxFrame((wxFrame *)parent, wxID_ANY, wxT("Ferret: Unique Trigrams View"),
    wxDefaultPosition, wxSize (650, 500)),
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

int UniqueTrigramsListCtrl::GetNumberItems () const
{
  _ferretparent->GetDocumentList().Size ();
}

// TODO: Include sorted indices
wxString UniqueTrigramsListCtrl::OnGetItemText (long item, long column) const
{
  switch (column)
  {
    case 0:
      return _ferretparent->GetDocumentList()[item]->GetName ();
    case 1:
      return wxString::Format("%d", _ferretparent->GetDocumentList()[item]->GetUniqueTrigramCount ());
  }
}
