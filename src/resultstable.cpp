// GUI interface to Ferret program
// written by Peter Lane, March-June 2006, September 2007

#include "resultstable.h"
#include "documentview.h"

wxStaticText * MakeStaticText (wxWindow * parent, wxString title)
{
	return new wxStaticText (parent, wxID_ANY, title); 
}

wxButton * MakeButton (wxWindow * parent, int id, wxString title, wxString tooltip, bool enabled)
{
	wxButton * button = new wxButton (parent, id, title);
	if (!tooltip.IsEmpty ()) button->SetToolTip (tooltip);
	button->Enable (enabled);
	return button;
}

wxCheckBox * MakeCheckBox (wxWindow * parent, int id, wxString title, wxString tooltip, bool checked)
{
	wxCheckBox * check = new wxCheckBox (parent, id, title);
	if (!tooltip.IsEmpty ()) check->SetToolTip (tooltip);
	check->SetValue (checked);
	return check;
}

wxSpinCtrl * MakeSpinCtrl (wxWindow * parent, int id, wxString tooltip, int min_value, int max_value, int start_value)
{
	wxSpinCtrl * value = new wxSpinCtrl (parent, id, wxString::Format("%d", start_value),
				wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,
				min_value, max_value, start_value);
	value->SetToolTip (tooltip);

	return value;
}

// make the names of the comparison structure visible
std::vector<wxString> * DocumentListCtrl::namecmp::names;
std::vector<int> * DocumentListCtrl::namecmp::documents;

BEGIN_EVENT_TABLE(DocumentListCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED (wxID_ANY, DocumentListCtrl::OnListItemActivated)
	EVT_LIST_COL_CLICK (wxID_ANY, DocumentListCtrl::OnSortColumn)
END_EVENT_TABLE()

void DocumentListCtrl::UpdatedDocumentList ()
{
	DocumentList & doclist = _ferretparent->GetDocumentList ();
	int num_docs = doclist.Size ();

	// set up two indices, mapping position in the list control to a document
	// -- do not add a pair if in same group
	_document1.clear ();
	_document2.clear ();
	for (int i=0; i<num_docs; ++i)
		for (int j=i+1; j<num_docs; ++j)
		{
			if (doclist[i]->GetGroupId () != doclist[j]->GetGroupId ())
			{ // only add pair if not in same group
				_document1.push_back (i);
				_document2.push_back (j);
			}
		}
	assert (_document1.size () == _document2.size ()); // must be same number of items in both
	int num_pairs = _document1.size (); // use _document1 length to give number of pairs
	SetItemCount (num_pairs); // tell list ctrl the number of items in the list
	
	// create a list of indices: 
	// position in list can be altered by sorting
	_sortedIndices.clear ();
	for (int i=0; i<num_pairs; ++i)
	{
		_sortedIndices.push_back (i);
	}
}

void DocumentListCtrl::SelectFirstItem ()
{
	// select first item
	if (_sortedIndices.size() != 0) 
	{
		SetItemState (GetTopItem (), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		EnsureVisible (GetTopItem ());
	}
}

// toggle the type of displayed similarity measure
// false : use all trigrams in comparison
// true  : only use trigrams uniquely within the two documents
void DocumentListCtrl::SetSimilarityType (bool removeCommonTrigrams, bool ignoreTemplateMaterial)
{
  _remove_common_trigrams = removeCommonTrigrams;
  _ignore_template_material = ignoreTemplateMaterial;
  // repeat the last used sort
  switch (_lastsort)
  {
    case sortDoc1: SortOnDocument (0, true); break;
    case sortDoc2: SortOnDocument (1, true); break;
    case sortResemblance: SortOnResemblance (true); break;
  }
  // update the status
  _ferretparent->SetStatusText (wxString::Format ("Mean: %f", MeanResemblance()), 3);
}

void DocumentListCtrl::SortOnDocument (int doc_num, bool force_sort)
{
	// just return if last sort is re-requested
	if (doc_num == 0 && _lastsort == sortDoc1 && !force_sort) return;
	if (doc_num == 1 && _lastsort == sortDoc2 && !force_sort) return;
	// store this sort
	if (doc_num == 0) 
		_lastsort = sortDoc1;
	else 
		_lastsort = sortDoc2;
	// get current selected item
	long item_number = GetNextItem (-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	int selected_item = (item_number == -1 ? -1 : _sortedIndices[(int)item_number]);

	DocumentList & doclist = _ferretparent->GetDocumentList ();
	int num_docs = doclist.Size ();
	int num_pairs = doclist.NumberOfPairs ();
	std::vector<int> newIndices;

	// initialise the set of new indices
	for (int i=0; i < _sortedIndices.size(); ++i)
	{
		newIndices.push_back (_sortedIndices[i]);
	}
	
	// cache the names
	std::vector<wxString> names;
	for (int i=0; i<num_docs; ++i)
	{
		names.push_back (GetDisplayName (i));
	}

	// set up sort
	namecmp comparer;
	comparer.names = &names;
	if (doc_num == 0)
		comparer.documents = &_document1;
	else
		comparer.documents = &_document2;

	std::sort (newIndices.begin(), newIndices.end(), comparer);
	
	_sortedIndices = newIndices;

	RefreshItems (0, _sortedIndices.size()-1);
	// select original item
	if (selected_item != -1)
	{
		// find item in sorted indices, and select that position
		for (long i = 0, n = _sortedIndices.size(); i < n; ++i)
		{
			if (_sortedIndices[i] == selected_item)
			{
				SetItemState (i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
				EnsureVisible (i);
				break;
			}
		}
	}
	Refresh ();
}

void DocumentListCtrl::SortOnDocument1 ()
{
	SortOnDocument (0);
	_ferretparent->SetStatusText ("Rearranged table by first document", 0);
}

void DocumentListCtrl::SortOnDocument2 ()
{
	SortOnDocument (1);
	_ferretparent->SetStatusText ("Rearranged table by second document", 0);
}

void DocumentListCtrl::SortOnResemblance (bool force_sort)
{
	if (_lastsort == sortResemblance && !force_sort) return;
	_lastsort = sortResemblance;
	// get current selected item
	long item_number = GetNextItem (-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	int selected_item = (item_number == -1 ? -1 : _sortedIndices[(int)item_number]);

	std::vector<int> newIndices;

	// initialise the set of new indices
	for (int i=0; i < _sortedIndices.size(); ++i)
	{
		newIndices.push_back (_sortedIndices[i]);
	}
	
	std::sort (newIndices.begin(), newIndices.end(), 
			_ferretparent->GetDocumentList().GetSimilarityComparer (&_document1, &_document2, _remove_common_trigrams, _ignore_template_material));

	_sortedIndices = newIndices;
	RefreshItems (0, _sortedIndices.size()-1);
	// select original item
	if (selected_item != -1)
	{
		// find item in sorted indices, and select that position
		for (long i = 0, n = _sortedIndices.size(); i < n; ++i)
		{
			SetItemState (i, !wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			if (_sortedIndices[i] == selected_item)
			{
				SetItemState (i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
				EnsureVisible (i);
			}
		}
	}

	Refresh ();
	_ferretparent->SetStatusText ("Rearranged table by similarity", 0);
}

float DocumentListCtrl::MeanResemblance () const
{
  float total = 0.0;
  for (int i=0, n=_ferretparent->GetDocumentList().Size (); i<n; i++)
  {
    for (int j=i+1, m=_ferretparent->GetDocumentList().Size (); j < m; j++)
    {
      total += _ferretparent->GetDocumentList().ComputeResemblance (i, j, _remove_common_trigrams, _ignore_template_material);
    }
  }

  return total / _ferretparent->GetDocumentList().NumberOfPairs ();
}

wxString DocumentListCtrl::OnGetItemText (long item, long column) const
{
	int sorteditem = _sortedIndices[(int)item];
	int num_docs = _ferretparent->GetDocumentList().Size();
	if (num_docs > 0)
	{
		int doc1 = _document1 [sorteditem];
		int doc2 = _document2 [sorteditem];
		int position = 0;
		if (column == 0)
		{
			return GetDisplayName (doc1);
		}
		else if (column == 1)
		{
			return GetDisplayName (doc2);
		}
		else // if (column == 2)
		{
			return wxString::Format("%f", 
					_ferretparent->GetDocumentList().ComputeResemblance (doc1, doc2, _remove_common_trigrams, _ignore_template_material));
		}
	}
	else
	{
		return "No items in document list";
	}
}

void DocumentListCtrl::ShowItem (long item_number)
{
	wxBusyCursor wait;
	wxGetApp().Yield ();
	int index = _sortedIndices[item_number];
	wxString name1 = GetName (_document1[index]);
	wxString name2 = GetName (_document2[index]);
	wxString title = wxString::Format("Ferret: Analysis of copying between %s and %s", 
			name1.c_str(), name2.c_str());
	DocumentComparisonView * view = new DocumentComparisonView (_ferretparent, title, 
			_document1[index], _document2[index], _remove_common_trigrams, _ignore_template_material);
	view->Show (true);
	view->LoadDocuments ();
}

void DocumentListCtrl::OnListItemActivated (wxListEvent & event)
{
	ShowItem (event.GetIndex ());
}

void DocumentListCtrl::ShowSelectedItem ()
{
	long item_number = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (item_number != -1)
	{
		ShowItem (item_number);
	}
}

void ComparisonTableView::SaveReportFor (int document1, int document2, bool unique, bool ignore)
{
	_resemblanceObserver->SaveReportFor (document1, document2, unique, ignore);
}

// Save report of document comparison
// -- input is index of documents to compare
// -- uses file dialog to locate filename and type of output
// -- saves comparison in either .pdf or .xml format
void DocumentListCtrl::SaveReportFor (int document1, int document2, bool unique, bool ignore)
{
  wxFileName filename1 (GetPathname (document1));
  wxFileName filename2 (GetPathname (document2));
  wxString newname = wxString::Format ("%s-%s", 
      filename1.GetName().c_str(), 
      filename2.GetName().c_str());
  wxFileDialog dialog (NULL, "Create and save report",
      wxEmptyString,
      newname,
      "xml|*.xml", 
      //"pdf|*.pdf|xml|*.xml", 
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);
  if (dialog.ShowModal () == wxID_OK)
  {
    wxBusyCursor wait;
    wxBusyInfo info ("Saving comparison report, please wait ...", this);
    wxFileName path (dialog.GetPath ()); 
    /*
   	if (dialog.GetFilterIndex () == 0 || (path.GetExt () == "pdf"))
   	{
    	path.SetExt ("pdf"); // force .pdf as extension
    	PdfReport pdfreport (_ferretparent->GetDocumentList (), unique, ignore);
    	pdfreport.WritePdfReport (path.GetFullPath (), document1, document2);
    }
    else // if (dialog.GetFilterIndex () == 1 || (path.GetExt () == "xml"))
    {
      path.SetExt ("xml"); // force .xml as extension
      XmlReport xmlreport (_ferretparent->GetDocumentList (), unique, ignore);
      xmlreport.WriteXmlReport (path.GetFullPath (), document1, document2);
    }
    */
    path.SetExt ("xml"); // force .xml as extension
    XmlReport xmlreport (_ferretparent->GetDocumentList (), unique, ignore);
    xmlreport.WriteXmlReport (path.GetFullPath (), document1, document2);
  }
}

// Save report for pair of documents highlighted in List
void DocumentListCtrl::SaveSelectedItem ()
{
	long item_number = GetNextItem (-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (item_number == -1) return;	// nothing selected, so return
	
	long index = _sortedIndices[item_number];
	int document1 = _document1[index];
	int document2 = _document2[index];
	SaveReportFor (document1, document2, _remove_common_trigrams, _ignore_template_material);
}

// Called when user clicks top of table
// -- locates the column which the click was on
// -- calls appropriate function to sort table based on that column
void DocumentListCtrl::OnSortColumn (wxListEvent & event)
{
	int column = event.GetColumn ();

	if (column == 0)
	{
		SortOnDocument1 ();
	}
	else if (column == 1)
	{
		SortOnDocument2 ();
	}
	else if (column == 2)
	{
		SortOnResemblance ();
	}
}

int DocumentListCtrl::GetNumberItems () const
{
	return _sortedIndices.size();
}

// Return the pathname for given document, by retrieving item from parent document list
wxString DocumentListCtrl::GetPathname (int i) const
{
	return _ferretparent->GetDocumentList()[i]->GetPathname ();
}

// Return the file name for given document, by retrieving item from parent document list
wxString DocumentListCtrl::GetName (int i) const
{
	return _ferretparent->GetDocumentList()[i]->GetName ();
}

// Return the file or pathname for given document
// by retrieving item from parent document list.
wxString DocumentListCtrl::GetDisplayName (int i) const
{
  wxString result = "";
  // add an indicator for template code
  if (_ferretparent->GetDocumentList()[i]->GetGroupId () == 0)
  {
    result = "TM:";
  }
  if (_show_short)
  {
    result += _ferretparent->GetDocumentList()[i]->GetShortName ();
  }
  else
  {
	  result += _ferretparent->GetDocumentList()[i]->GetOriginalPathname ();
  }
  return result;
}

bool DocumentListCtrl::RemoveCommonTrigramsSet () const
{
  return _remove_common_trigrams;
}

bool DocumentListCtrl::IgnoreTemplateMaterialSet () const
{
  return _ignore_template_material;
}

BEGIN_EVENT_TABLE(ComparisonTableView, wxFrame)
	EVT_BUTTON(ID_RANK_1, ComparisonTableView::OnRank1)
	EVT_BUTTON(ID_RANK_2, ComparisonTableView::OnRank2)
	EVT_BUTTON(ID_RANK_R, ComparisonTableView::OnRank3)
	EVT_BUTTON(ID_SAVE_REPORT, ComparisonTableView::OnSaveReport)
  EVT_BUTTON(ID_UNIQUE_VIEW, ComparisonTableView::OnShowUniqueView)
  EVT_BUTTON(ID_ENGAGEMENT_VIEW, ComparisonTableView::OnShowEngagementView)
	EVT_BUTTON(ID_DISPLAY_TEXTS, ComparisonTableView::OnDisplayTexts)
	EVT_BUTTON(ID_CREATE_REPORT, ComparisonTableView::OnCreateReport)
  EVT_CHECKBOX(ID_REMOVE_COMMON, ComparisonTableView::OnCheckRemoveCommon)
  EVT_CHECKBOX(ID_IGNORE_TEMPLATE, ComparisonTableView::OnCheckIgnoreTemplate)
  EVT_CHECKBOX(ID_SHOW_SHORT, ComparisonTableView::OnCheckShortNames)
	EVT_BUTTON(wxID_HELP, ComparisonTableView::OnHelp)
	EVT_BUTTON(wxID_EXIT,  ComparisonTableView::OnQuit)
	EVT_CLOSE (ComparisonTableView::OnClose)
	EVT_SIZE(ComparisonTableView::OnResize)
END_EVENT_TABLE()

void ComparisonTableView::OnRank1 (wxCommandEvent & WXUNUSED(event))
{
	_resemblanceObserver->SortOnDocument1 ();
}

void ComparisonTableView::OnRank2 (wxCommandEvent & WXUNUSED(event))
{
	_resemblanceObserver->SortOnDocument2 ();
}

void ComparisonTableView::OnRank3 (wxCommandEvent & WXUNUSED(event))
{
	_resemblanceObserver->SortOnResemblance ();
}

void ComparisonTableView::OnDisplayTexts (wxCommandEvent & WXUNUSED(event))
{
	_resemblanceObserver->ShowSelectedItem ();
}

void ComparisonTableView::OnCreateReport (wxCommandEvent & WXUNUSED(event))
{
	_resemblanceObserver->SaveSelectedItem ();
}

void ComparisonTableView::OnCheckRemoveCommon (wxCommandEvent & WXUNUSED(event))
{
  UpdateSimilarity ();
}

void ComparisonTableView::OnCheckIgnoreTemplate (wxCommandEvent & WXUNUSED(event))
{
  UpdateSimilarity ();
}

void ComparisonTableView::UpdateSimilarity ()
{
  wxCheckBox * remove_box = (wxCheckBox *) FindWindow (ID_REMOVE_COMMON);
  wxCheckBox * ignore_box = (wxCheckBox *) FindWindow (ID_IGNORE_TEMPLATE);
  _resemblanceObserver->SetSimilarityType (remove_box->IsChecked (), ignore_box->IsChecked ());
}

void ComparisonTableView::OnCheckShortNames (wxCommandEvent & WXUNUSED(event))
{
  wxCheckBox * box = (wxCheckBox *) FindWindow (ID_SHOW_SHORT);
  if (box != 0 && box->IsChecked ())
  {
    _resemblanceObserver->SetShowShort (true);
  }
  else
  {
    _resemblanceObserver->SetShowShort (false);
  }
}

void ComparisonTableView::OnHelp (wxCommandEvent & WXUNUSED(event))
{
	wxGetApp().ShowTableHelp ();
}

void ComparisonTableView::OnSaveReport (wxCommandEvent & WXUNUSED(event))
{
	wxFileDialog dialog (NULL, "Save table of comparisons and other details as a pdf report",
			wxEmptyString, 
			"results-table.pdf",
			"pdf|*.pdf",
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);
	if (dialog.ShowModal () == wxID_OK)
	{
		wxString path = dialog.GetPath ();
		SaveTableThread * thread = new SaveTableThread (path, _documentlist, *_resemblanceObserver);
		if (thread->Create () == wxTHREAD_NO_ERROR)
		{
			thread->Run ();
			SetStatusText ("Saved table as pdf");
		}
		else
		{
			SetStatusText ("Error in saving table as pdf");
		}
	}
	else
	{
		SetStatusText ("Cancelled save table as pdf");
	}
}

void ComparisonTableView::OnShowUniqueView (wxCommandEvent & WXUNUSED(event))
{
 	wxBusyCursor wait;
	wxGetApp().Yield ();
	UniqueTrigramsView * view = new UniqueTrigramsView (this, _documentlist);
	view->Show (true);
}

void ComparisonTableView::OnShowEngagementView (wxCommandEvent & WXUNUSED(event))
{
 	wxBusyCursor wait;
	wxGetApp().Yield ();
	EngagementTrigramsView * view = new EngagementTrigramsView (this, _documentlist);
	view->Show (true);
}

void ComparisonTableView::OnQuit (wxCommandEvent & WXUNUSED(event))
{
	wxGetApp().CloseHelp ();
  wxGetApp().RemoveFolders ();
	Destroy ();
}

wxString ComparisonTableView::GetName (int document) const
{
	wxString name (_resemblanceObserver->GetName (document));
	return name;
}

// Set column widths of list control to fill out the available space on screen
void ComparisonTableView::OnResize (wxSizeEvent & event)
{
	if (_resemblanceObserver == NULL) return;  // just forget it, if there is nothing to observe

	wxPanel * button = (wxPanel *) FindWindow (ID_RANK_1); // pick a widget off frame for width

	// TODO: Get this to work without need for - 55
	int new_width = GetClientSize().GetWidth () - button->GetSize().GetWidth() - 55;
	int current_width = _resemblanceObserver->GetColumnWidth (0) + 
		_resemblanceObserver->GetColumnWidth (1) +
		_resemblanceObserver->GetColumnWidth (2);
	if (new_width > 0 && current_width > 0)
	{
		double scale_factor = (double)new_width / (double)current_width;
		for (int i = 0; i < 3; ++i)
		{
			int newcolwidth = (int)(_resemblanceObserver->GetColumnWidth (i) * scale_factor);
			_resemblanceObserver->SetColumnWidth (i, newcolwidth);
		}
	}
	event.Skip (); // process resize event for superclasses
}

ComparisonTableView::ComparisonTableView()
	: wxFrame(NULL, wxID_ANY, "Ferret: Table of comparisons", 
		wxDefaultPosition, wxSize (650, 610))
{
	CentreOnScreen ();
	CreateStatusBar(4);
  int widths [] = {-2, -1, -1, -1};
	SetStatusWidths (4, widths);
	SetStatusText("Welcome to Ferret", 0);
	SetStatusText("Documents: ", 1);
	SetStatusText("Pairs: ", 2);
  SetStatusText("Mean: ", 3);
	
	// set up internal widgets
	wxBoxSizer * topsizer = new wxBoxSizer (wxHORIZONTAL);
	
	// 1. comparison table
	wxPanel * docResemblanceView = new wxPanel (this, wxID_ANY);
	wxBoxSizer * resemblanceSizer = new wxBoxSizer (wxVERTICAL);
	docResemblanceView->SetSizer (resemblanceSizer);

	_resemblanceObserver = new DocumentListCtrl (this, docResemblanceView);
	resemblanceSizer->Add (_resemblanceObserver, 1, wxGROW);

	// -- insert three columns
	wxListItem itemCol;
	itemCol.SetText ("Document 1");
	_resemblanceObserver->InsertColumn (0, itemCol);
	_resemblanceObserver->SetColumnWidth (0, wxLIST_AUTOSIZE_USEHEADER);
	itemCol.SetText ("Document 2");
	_resemblanceObserver->InsertColumn (1, itemCol);
	_resemblanceObserver->SetColumnWidth (1, wxLIST_AUTOSIZE_USEHEADER);
	itemCol.SetText ("Similarity");
	_resemblanceObserver->InsertColumn (2, itemCol);
	_resemblanceObserver->SetColumnWidth (2, wxLIST_AUTOSIZE_USEHEADER);

	// 2. buttons 
	// -- note, buttons must be defined before their staticboxsizer, 
	// else tooltips do not display
	wxBoxSizer  * buttonSizer = new wxBoxSizer (wxVERTICAL);
  buttonSizer->Add (MakeButton (this, ID_SAVE_REPORT, "Save Report ...",
        "Save the table of comparisons and other details"),
      0, wxGROW | wxALL, 5);
  buttonSizer->Add (MakeButton (this, ID_UNIQUE_VIEW, "Show Uniqueness", 
        "Show number of unique trigrams per document or group"),
      0, wxGROW | wxALL, 5);
  buttonSizer->Add (MakeButton (this, ID_ENGAGEMENT_VIEW, "Show Engagement", 
        "Show overlap with template material per document or group"),
      0, wxGROW | wxALL, 5);

	wxButton * rank_1 = MakeButton (this, ID_RANK_1, "Document 1",
				"Put table into alphabetical order of first document");
	wxButton * rank_2 = MakeButton (this, ID_RANK_2, "Document 2",
				"Put table into alphabetical order of second document");
	wxButton * rank_r = MakeButton (this, ID_RANK_R, "Similarity",
				"Put table into order with most similar at top");
	wxStaticBoxSizer * rankSizer = new wxStaticBoxSizer (wxVERTICAL, this, "Rearrange table by");
	rankSizer->Add (rank_1, 0, wxGROW | wxALL, 5);
	rankSizer->Add (rank_2, 0, wxGROW | wxALL, 5);
	rankSizer->Add (rank_r, 0, wxGROW | wxALL, 5);
	buttonSizer->Add (rankSizer, 0, wxGROW);

	wxButton * showButton = MakeButton (this, ID_DISPLAY_TEXTS, "Show Analysis",
				"Show selected documents in a separate window");
	wxButton * saveButton = MakeButton (this, ID_CREATE_REPORT, "Save Analysis ...",
				"Save analysis of the selected documents");
	wxStaticBoxSizer * showSizer = new wxStaticBoxSizer (wxVERTICAL, this, "For selected pair");
	showSizer->Add (showButton, 0, wxGROW | wxALL, 5);
	showSizer->Add (saveButton, 0, wxGROW | wxALL, 5);
	buttonSizer->Add (showSizer, 0, wxGROW);

	buttonSizer->AddStretchSpacer (); // separate controls

  buttonSizer->Add (MakeCheckBox (this, ID_REMOVE_COMMON, "Remove Common Trigrams",
      "Compute similarity only from trigrams for the two documents", false), 
      0, wxGROW | wxALL, 5);  
  buttonSizer->Add (MakeCheckBox (this, ID_IGNORE_TEMPLATE, "Ignore Template Material",
      "Compute similarity but ignore any trigrams in template material", false), 
      0, wxGROW | wxALL, 5);  
  buttonSizer->Add (MakeCheckBox (this, ID_SHOW_SHORT, "Show Short Names",
      "Uncheck to show full pathnames in table", true), 
      0, wxGROW | wxALL, 5);

	buttonSizer->AddStretchSpacer (); // separate window controls from Ferret controls
	buttonSizer->Add (new wxButton (this, wxID_HELP), 0, wxGROW | wxALL, 5);
	buttonSizer->Add (new wxButton (this, wxID_EXIT), 0, wxGROW | wxALL, 5);
	
	topsizer->Add (buttonSizer, 0, wxGROW | wxALL, 5);
	topsizer->Insert (0, docResemblanceView, 1, wxGROW);
	
	// compute best minimum height, and constrain window
	int best_height = buttonSizer->GetMinSize().GetHeight () + 40; // allow for space between widgets
	SetSizeHints (best_height, best_height); // keep the minimum size a square shape

	SetSizer (topsizer);
	_resemblanceObserver->SetFocus ();  // give focus to the list control, to show highlighted item

#if __WXMSW__
	SetBackgroundColour (wxNullColour); // ensure background coloured
#endif
}

void ComparisonTableView::OnClose (wxCloseEvent & WXUNUSED(event))
{
	wxGetApp().CloseHelp ();
	Destroy ();
}

void ComparisonTableView::SetDocumentList (DocumentList & documentlist)
{
	_documentlist = documentlist;
	SetStatusText (wxString::Format ("Documents: %d", _documentlist.Size()), 1);
	SetStatusText (wxString::Format ("Pairs: %d", _documentlist.NumberOfPairs()), 2);
	_documentlist.ComputeSimilarities ();
 	_resemblanceObserver->UpdatedDocumentList ();
	_resemblanceObserver->SortOnResemblance ();
	_resemblanceObserver->SelectFirstItem ();
  SetStatusText (wxString::Format ("Mean: %f", _resemblanceObserver->MeanResemblance()), 3);
  ((wxButton *) FindWindow (ID_ENGAGEMENT_VIEW))->Enable (_documentlist.HasTemplateMaterial ());
  ((wxCheckBox *) FindWindow (ID_IGNORE_TEMPLATE))->Enable (_documentlist.HasTemplateMaterial ());
}


