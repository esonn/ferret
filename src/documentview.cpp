#include "documentview.h"

TextctrlReport::TextctrlReport (DocumentList & doclist, DocumentView * view, bool unique, bool ignore)
	: OutputReport (doclist, unique, ignore), _view (view)
{
}

void TextctrlReport::ProcessTrigram (wxString trigram, int start, int end)
{
	_view->AddTupleStart (trigram, start, _text->GetLastPosition ());
}

void TextctrlReport::WriteReport (wxTextCtrl * text, int doc1, int doc2)
{
	_text = text;
	wxFont normal_font = _text->GetFont ();
	_normal_style = wxTextAttr (*wxBLACK, wxNullColour, normal_font);
	wxFont bold_font = _text->GetFont ();
	bold_font.SetWeight (wxFONTWEIGHT_BOLD);
	_bold_style = wxTextAttr (*wxBLUE, wxNullColour, bold_font);
  _bold_red_style = wxTextAttr (*wxRED, wxNullColour, bold_font);

	_text->Freeze ();
	WriteDocument (doc1, doc2);
	_text->Thaw ();
	_text->SetInsertionPoint (0); // make sure display shows beginning
}

void TextctrlReport::StartCopiedBlock (bool unique)
{
	_text->SetDefaultStyle ((unique ? _bold_red_style : _bold_style));
}

void TextctrlReport::StartNormalBlock ()
{
	_text->SetDefaultStyle (_normal_style);
}

void TextctrlReport::WriteString (wxString str)
{
	_text->WriteText (str);
}

BEGIN_EVENT_TABLE(TrigramList, wxListCtrl)
	EVT_LIST_ITEM_SELECTED (wxID_ANY, TrigramList::OnListItemActivated)
	EVT_LIST_ITEM_ACTIVATED (wxID_ANY, TrigramList::OnListItemActivated)
END_EVENT_TABLE()

wxString TrigramList::OnGetItemText (long item, long column) const
{
	return _parent->GetTuple (item);
}

void TrigramList::OnListItemActivated (wxListEvent & event)
{
	wxString tuple = _parent->GetTuple (event.GetIndex ());
	_parent->GetDocument1View()->StartTupleSearch (tuple);
	_parent->GetDocument2View()->StartTupleSearch (tuple);
}

BEGIN_EVENT_TABLE(DocumentView, wxPanel)
	EVT_BUTTON (wxID_FIND, DocumentView::OnFind)
END_EVENT_TABLE()

void DocumentView::StartFind ()
{
	wxButton * findbutton = (wxButton *) FindWindow (wxID_FIND);
	if (_search_starts.size() > 1)
		findbutton->Enable (); // enable if more than one search
	else
		findbutton->Disable ();
	_search_posn = 0;
	int token_start = _search_starts[_search_posn];
	int token_end = _search_ends[_search_posn];
	_docObserver->SetSelection (token_start, token_end);
	_docObserver->ShowPosition (token_start);
	_search_posn++;
}

// Do a new search for text from current position
void DocumentView::OnFind (wxCommandEvent & event)
{
	if (_inside_search)
	{
		// look for next position
		if (_search_posn < _search_starts.size())
		{
			int token_start = _search_starts[_search_posn];
			int token_end = _search_ends[_search_posn];
			_docObserver->SetSelection (token_start, token_end);
			_docObserver->ShowPosition (token_start);
			_search_posn ++;
		}
		else
		{
			// wrap around to start of file
			StartFind ();
		}
	}
}

DocumentView::DocumentView (wxWindow * parent, ComparisonTableView * ferretparent, 
		int main_document, int compared_document, bool unique, bool ignore)
	: wxPanel (parent, wxID_ANY), 
	  _ferretparent (ferretparent),
	  _main_document (main_document), 
	  _compared_document (compared_document),
	  _inside_search (false),
    _unique (unique),
    _ignore_template_material (ignore)
{
	wxBoxSizer * docViewSizer = new wxBoxSizer (wxVERTICAL);
	SetSizer (docViewSizer);

	_pathname = _ferretparent->GetDocumentList()[main_document]->GetPathname ();
	wxFileName filename (_pathname);

	wxStaticBox * docLabel = new wxStaticBox (this, wxID_ANY, _ferretparent->GetName (main_document));
	wxStaticBoxSizer * staticBoxDocSizer = new wxStaticBoxSizer (docLabel, wxVERTICAL);
	docViewSizer->Add (staticBoxDocSizer, 1, wxGROW | wxALL, 2);
	// the text display
	if (_ferretparent->GetDocumentList()[main_document]->IsTextType ())
	{
		_docObserver = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition,
				wxDefaultSize,
				wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_NOHIDESEL);
	}
	else // don't wrap code
	{
		_docObserver = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition,
				wxDefaultSize,
				wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP | wxTE_NOHIDESEL);

	}

	staticBoxDocSizer->Add (_docObserver, 1, wxGROW | wxALL, 2);

	// add a row of buttons
	wxPanel * buttons = new wxPanel (this, wxID_ANY);
	wxBoxSizer * buttonsSizer = new wxBoxSizer (wxHORIZONTAL);
	buttons->SetSizer (buttonsSizer);

	wxString label = wxString::Format ("Number of distinct trigrams: %d",
			_ferretparent->GetDocumentList().CountTrigrams (main_document));
	buttonsSizer->Add (new wxStaticText (buttons, wxID_ANY, label), 0, wxALIGN_CENTER | wxALL, 2);

	wxButton * find_button = new wxButton (buttons, wxID_FIND);
	find_button->SetToolTip ("Find next highlighted trigram");
	find_button->Disable ();

	buttonsSizer->AddStretchSpacer ();
	buttonsSizer->Add (find_button, 0, wxALL, 2);

	staticBoxDocSizer->Add (buttons, 0, wxGROW | wxALL, 2);
}

void DocumentView::LoadDocument ()
{
	wxBusyCursor wait; // Note: cannot use wxWindowDisabler as messes up textctrl highlight
	wxBusyInfo info ("Loading and comparing documents, please wait ...", this);
	wxGetApp().Yield ();

	TextctrlReport textoutput (_ferretparent->GetDocumentList (), this, _unique, _ignore_template_material);
	textoutput.WriteReport (_docObserver, _main_document, _compared_document);
}

void DocumentView::AddTupleStart (wxString tuple, int start, int end)
{
	_trigram_starts[tuple].push_back (start);
	_trigram_ends[tuple].push_back (end);
}

void DocumentView::StartTupleSearch (wxString tuple)
{
	if (_docObserver->GetNumberOfLines () < 2) return; // no text to search
	// if already in a search, clear out old highlighting
	wxFont bold_font = GetFont ();
	bold_font.SetWeight (wxFONTWEIGHT_BOLD);

	if (_inside_search)
	{
		for (int i=0, n=_search_starts.size(); i<n; ++i)
		{
			_docObserver->SetStyle(_search_starts[i], _search_ends[i], _search_style[i]);
		}
	}

	// set up the new set of tuples to search
	_inside_search = true;
	_search_starts = _trigram_starts[tuple];
	_search_ends = _trigram_ends[tuple];
  _search_style.clear ();
	// and highlight the tuples
	wxColour yellow_colour = wxTheColourDatabase->Find ("YELLOW");
	for (int i=0, n=_search_starts.size(); i<n; ++i)
	{
    wxTextAttr style;
     _docObserver->GetStyle ((long)_search_starts[i], style);
    _search_style.push_back (style);
	  wxTextAttr yellow (style.GetTextColour(), yellow_colour, bold_font);
		_docObserver->SetStyle(_search_starts[i], _search_ends[i], yellow);
	}	
	StartFind ();
}

BEGIN_EVENT_TABLE(DocumentComparisonView, wxFrame)
	EVT_BUTTON (wxID_HELP, DocumentComparisonView::OnHelp)
	EVT_BUTTON (wxID_CLOSE, DocumentComparisonView::OnClose)
	EVT_BUTTON (ID_CREATE_REPORT, DocumentComparisonView::OnCreateReport)
END_EVENT_TABLE()

DocumentComparisonView::DocumentComparisonView (ComparisonTableView * parent, wxString title, int document1, int document2, bool unique, bool ignore)
	: wxFrame((wxFrame *)parent, wxID_ANY, 
			title,
			wxGetApp().GetNextFramePosition (800, 550), 
			wxSize (800, 550)),
	  _document1 (document1),
	  _document2 (document2),
	  _ferretparent (parent),
    _unique (unique),
    _ignore_template_material (ignore)
{
	wxPanel * paneButtons = new wxPanel (this, wxID_ANY);
	wxBoxSizer * paneButtonsSizer = new wxBoxSizer (wxVERTICAL);
	paneButtons->SetSizer (paneButtonsSizer);
	
	wxSplitterWindow * pane = new wxSplitterWindow (paneButtons, wxID_ANY,
			wxPoint (0, 0), wxDefaultSize);
	wxBoxSizer * paneSizer = new wxBoxSizer (wxHORIZONTAL);
	pane->SetSizer (paneSizer);
	
	wxSplitterWindow * docSplitter = new wxSplitterWindow (pane, wxID_ANY,
			wxPoint (0, 0), wxDefaultSize);

	// document views
	_document1_view = new DocumentView (docSplitter, _ferretparent, 
			_document1, _document2, _unique, _ignore_template_material);
	_document2_view = new DocumentView (docSplitter, _ferretparent, 
			_document2, _document1, _unique, _ignore_template_material);
	
	// -- trigram panel
	wxPanel * trigramPanel = new wxPanel (pane, wxID_ANY);
	wxBoxSizer * trigramSizer = new wxBoxSizer (wxVERTICAL);
	trigramPanel->SetSizer (trigramSizer);

	wxStaticBox * trigramLabel = new wxStaticBox (trigramPanel, wxID_ANY, "Matching trigrams");
	wxStaticBoxSizer * trigramLabelSizer = new wxStaticBoxSizer (trigramLabel, wxVERTICAL);
	trigramSizer->Add (trigramLabelSizer, 1, wxGROW | wxALL, 2);

	_trigramList = new TrigramList (this, trigramPanel);
	trigramLabelSizer->Add (_trigramList, 1, wxGROW | wxALL, 2);

	wxString num_matches_label = wxString::Format ("%d matches",
			_ferretparent->GetDocumentList().CountMatches(document1, document2, _unique, _ignore_template_material));
	trigramLabelSizer->Add (new wxStaticText (trigramPanel, wxID_ANY, num_matches_label), 
			0, wxALIGN_CENTER | wxALL, 2);

	// add components
	paneSizer->Add (docSplitter, 1, wxGROW | wxALL, 2);
	paneSizer->Add (trigramPanel, 1, wxGROW | wxALL, 2);

	// create some buttons under the views
	wxPanel * buttons = new wxPanel (paneButtons, wxID_ANY);
	wxBoxSizer * buttonSizer = new wxBoxSizer (wxHORIZONTAL);
	buttons->SetSizer (buttonSizer);

	wxFont boldfont (
#if __WXMAC__
    15,
#else
	11,
#endif
	wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD, false);
	wxString label2 = wxString::Format ("Similarity measure%s%s: %f",
      (_unique ? " (unique)" : ""),
      (_ignore_template_material ? " (no template)" : ""),
			_ferretparent->GetDocumentList().ComputeResemblance(document1, document2, _unique, _ignore_template_material));
	wxStaticText * labeltext = new wxStaticText (buttons, wxID_ANY, label2);
	labeltext->SetFont (boldfont);
	buttonSizer->Add (labeltext, 0, wxALIGN_CENTER | wxALL, 2);

	buttonSizer->AddStretchSpacer ();
	buttonSizer->Add (new wxButton (buttons, wxID_HELP), 0, wxALL, 2);
#if !__WXMAC__
	buttonSizer->Add (MakeButton (buttons, ID_CREATE_REPORT, "Save Analysis ...",
				"Save analysis of the selected documents"),
		       	0, wxALL, 2);
#endif
	buttonSizer->Add (new wxButton (buttons, wxID_CLOSE), 0, wxALL, 2);
	#if __WXMAC__
	buttonSizer->Add (new wxStaticText (buttons, wxID_ANY, ""), 0, wxRIGHT, 10);
	#endif
	
	paneButtonsSizer->Add (pane, 1, wxGROW | wxALL, 2);
	paneButtonsSizer->Add (buttons, 0, wxGROW | wxALL, 5);
	
	_matchingtrigrams = _ferretparent->GetDocumentList().CollectMatchingTrigrams (document1, document2, _unique, _ignore_template_material);
	
	_trigramList->InsertColumn (0, "Trigrams");
	_trigramList->SetItemCount (_matchingtrigrams.GetCount ());
	wxString longest_trigram;
	for (int i=0, n=_matchingtrigrams.GetCount (); i<n; ++i)
	{
		if (_matchingtrigrams.Item(i).Len () > longest_trigram.Len ())
			longest_trigram = _matchingtrigrams.Item (i);
	}
	longest_trigram += "   "; // add a little extra space
	int trigram_width, trigram_height;
	GetTextExtent (longest_trigram, &trigram_width, &trigram_height);
	_trigramList->SetColumnWidth (0, trigram_width);

	// Add elements to splitter windows -- this done last, so window sizes are known 
	// and pane must be done before docSplitter
	pane->SplitVertically (docSplitter, trigramPanel, - trigram_width);
	pane->SetMinimumPaneSize (50);
	pane->SetSashGravity (1.0);

	docSplitter->SplitHorizontally (_document1_view, _document2_view, 0);
	docSplitter->SetMinimumPaneSize (50);
	docSplitter->SetSashGravity (0.5);
  docSplitter->SetSashPosition (60);
}

DocumentComparisonView::~DocumentComparisonView ()
{
	_ferretparent->Raise ();
}

void DocumentComparisonView::LoadDocuments ()
{
	wxBusyCursor wait;
	wxGetApp().Yield ();
	_document1_view->LoadDocument ();
	_document2_view->LoadDocument ();
}

wxString DocumentComparisonView::GetTuple (long item)
{
	return _matchingtrigrams.Item (item);
}

void DocumentComparisonView::OnHelp (wxCommandEvent & WXUNUSED(event))
{
	wxGetApp().ShowComparisonHelp ();
}

void DocumentComparisonView::OnCreateReport (wxCommandEvent & WXUNUSED(event))
{
	_ferretparent->SaveReportFor (_document1, _document2, _unique, _ignore_template_material);
}

void DocumentComparisonView::OnClose (wxCommandEvent & WXUNUSED(event))
{
	Close ();
}

