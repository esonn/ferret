#include "helpframe.h"

BEGIN_EVENT_TABLE (HelpFrame, wxDialog)
	EVT_BUTTON (wxID_CLOSE, HelpFrame::OnUserClose)
	EVT_CLOSE (HelpFrame::OnSystemClose)
END_EVENT_TABLE()

HelpFrame::HelpFrame ()
	: wxDialog (NULL, wxID_ANY, "Ferret: Help",
		wxDefaultPosition, wxSize (450, 400), 
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX ) 
{
	wxBoxSizer * top_sizer = new wxBoxSizer (wxVERTICAL);
	
	// -- 1. a notebook, with separate help topics on each page
	wxNotebook * notebook = new wxNotebook (this, ID_HELP_PAGES);

	wxString about_text = "Ferret is a tool developed at the University of Hertfordshire for identifying copied passages in textual documents.  A document may contain natural language text, such as English, or computer programs, such as Java or C.  A measure of similarity is computed for each pair of documents, based on matching sequences of words or tokens.  Ferret is particularly suited to assist teachers in checking for collusion and plagiarism within groups of students.\n\n" 
"Large collections of documents can be processed in a single run, producing a table with every pair of compared documents arranged in order of similarity.  A detailed analysis of each pair of documents shows every occurrence of copied text.  Reports of the table of comparisons and the detailed analysis may be saved for later reviewing or printing.\n\n" 
"A typical session proceeds in three steps:\n1. The files containing the documents to analyse are selected, using the computer's file selector.\n2. A table is created, showing every pair of comparisons made, arranged in order of similarity.\n3. A detailed analysis of the copying present in each pair of documents may be viewed or saved.\n\n" 
"More information about Ferret can be found at \nhttp://peterlane.info/ferret.html\nFor information about the Plagiarism Detection Group see: \nhttp://homepages.stca.herts.ac.uk/~pdgroup.\n\n"
"IMPORTANT DISCLAIMER: This software comes with ABSOLUTELY NO WARRANTY; users of this software do so at their own risk.  Neither the University of Hertfordshire nor the individuals involved in any part of this software will accept any liability for any damage or harm caused to the data, computer or computer software by using this software.\n";
	
	wxString credits_text = "The original concept of using trigrams for measuring copying was developed by Caroline Lyon and James Malcolm.  JunPeng Bao, Ruth Barrett and Bob Dickerson also contributed to the development of earlier versions of Ferret.  The ideas for measuring uniqueness, engagement and group relationships were developed by Pam Green in her dissertation.\n\n" 
"Since version 3.0, Ferret has been rewritten using the wxWidgets cross-platform library - Peter Lane has produced the current implementation.\n\nText conversion is achieved by calling abiword: http://www.abisource.com, and pdftotext: http://www.xpdf.com";

	wxString selection_text = "The first step in running Ferret is to identify the documents to be compared.  Add the filenames of documents to the list by using the 'Add Documents ...' button, and choosing the filenames to compare.  You can repeat the operation to add documents from different folders.  (If all your files are in a single folder, you can usually open that folder, select the first item, and then press CONTROL and A together to select all the files.)\n\n"

			"If you make a mistake, you can start again by clicking on 'Clear Documents', which will remove every filename from the list.\n\nFinally, when you have selected all the documents you want to compare, click on 'Run Ferret' to perform the analysis and comparisons.  (Note, 'Run Ferret' is only available if there are at least two documents in the list.)\n\n"

			"Further settings:\n\nAdvanced users may use the 'Settings ...' button to reveal some further settings.  The settings affect how Ferret handles documents which do not appear to be plain text.\n\n" 
"When converting files, Ferret will save the files into the named folder.  The user can direct Ferret to use a different folder using the 'Browse ...' button.  This option is particularly useful for collecting together files from multiple folders into one place.\nFerret will only copy the converted files to the destination folder; check the first box if you want every file to be copied.\n\nIf you want every file to be treated as a word/rtf file (for example, with mislabelled student or pupil work), check the second box.\n\nIf you want to ignore any unknown files (for example, output from compilers or image files), check the third box.  (Ferret's initial settings mean it will ignore unknown filetypes, so uncheck if you want Ferret to process all files.)\n\n" 
"Ferret will treat files ending in .doc, .docx or .rtf as word/rtf files, and use 'abiword' to convert them to text.  Files ending in .pdf will be converted using 'pdftotext'.  Files ending in .txt will be treated as plain text files, and not converted.  Files for major programming languages are parsed specially.\n\n";
	
	wxString table_text = "Ferret produces a similarity score for every pair of documents in the selected list.  The similarity score runs from 0 (no copied sequences) to 1 (every sequence is copied).  Initially, the table is arranged with the most similar pairs of documents at the top.\n\nThe table may be rearranged into alphabetical order of name or numerical order of similarity by clicking on the respective button to the right (clicking on the column heading also rearranges the table).  The table, along with other relevant information, may be saved as a pdf or xml file, using the 'Save Report ...' button; the user will be prompted for a folder and filename to save the report in.\n\n" "You can see details of how the two documents are analysed, and the copied sequences, by clicking on a line in the table.  The 'Show Analysis' button will open a new window, showing the two documents; you can also show the analysis by double-clicking on the line.  The 'Save Analysis ...' button creates a pdf report highlighting the common trigrams in the two documents; the user will be prompted for a folder and filename to save the analysis in.\n\nYou may open as many of the analysis windows as you wish.\n";

	wxString comparison_text = "The chosen documents are shown with common trigrams highlighted in blue.\n\nThe list on the right shows all of the common trigrams contained in these two documents.  Click on a trigram in the list to highlight every occurrence of that trigram in the documents.  The 'Find' buttons below each document's display will allow you to step through every occurrence of the trigram in that document.\n\nYou may save a pdf or xml report of the comparison by clicking the 'Save Analysis' button on the lower right; the report will highlight the common trigrams. (XML reports can be viewed in a browser, but also download the associated style sheet from the ferret web page.)\n";

  wxString unique_text = "The uniqueness table shows a list of the documents or groups along with the total number of unique trigrams for that document or group.  Click on the column headings or buttons to resort the data.\n";

	// -- add pages
	notebook->AddPage (MakePage (notebook, wxGetApp().GetVersionString () + "\n\n", about_text), 
			"About", true);
	notebook->AddPage (MakePage (notebook, "How Ferret was Created\n\n", credits_text), 
			"Credits", false);
	notebook->AddPage (MakePage (notebook, "Selecting the Documents to Compare\n\n", selection_text),
			"Select", false);
	notebook->AddPage (MakePage (notebook, "Ferret's Table of Comparisons\n\n", table_text),
			"Compare", false);
	notebook->AddPage (MakePage (notebook, "Analysis Window\n\n", comparison_text),
			"Analyse", false);
  notebook->AddPage (MakePage (notebook, "Uniqueness Table\n\n", unique_text),
      "Uniqueness", false);

	top_sizer->Add (notebook, 1, wxGROW | wxALL, 5);

	// -- 2. single close button at base
	top_sizer->Add (new wxStaticLine (this, wxID_ANY), 0, wxGROW | wxALL, 5);
	top_sizer->Add (new wxButton (this, wxID_CLOSE), 0, wxALIGN_RIGHT | wxALL, (
#if __WXMAC__ 
				10
#else
				5
#endif
				));
	
	SetSizeHints (300, 200);
	SetSizer (top_sizer);
#if __WXMSW__
	SetBackgroundColour (wxNullColour); // ensure background coloured
#endif
};

wxPanel * HelpFrame::MakePage (wxWindow * parent, wxString title, wxString displayed_text)
{
	wxPanel * page = new wxPanel (parent, wxID_ANY);
	wxBoxSizer * page_sizer = new wxBoxSizer (wxVERTICAL);
	page->SetSizer (page_sizer);

	wxTextCtrl * text = new wxTextCtrl (page, wxID_ANY, 
			wxEmptyString,
			wxDefaultPosition, wxDefaultSize,
			wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_NOHIDESEL);
	page_sizer->Add (text, 1, wxGROW | wxALL, 5);

	wxTextAttr title_style = text->GetDefaultStyle ();
	wxFont title_font = title_style.GetFont ();
#if __WXMAC__
	title_font.SetPointSize (15);
#endif
	title_font.SetWeight (wxFONTWEIGHT_BOLD);
	title_style.SetFont (title_font);
	wxTextAttr default_style = text->GetDefaultStyle ();

	text->SetDefaultStyle (title_style);
	text->AppendText (title);
	text->SetDefaultStyle (default_style);
	text->AppendText (displayed_text);
	text->SetInsertionPoint (0);

	return page;
}

void HelpFrame::ShowHelpPage (int n)
{
	((wxNotebook *) FindWindow (ID_HELP_PAGES))->SetSelection (n);
	Show (true);
	Raise ();
}

void HelpFrame::ShowSelectionHelp ()
{
	ShowHelpPage (2);
}

void HelpFrame::ShowTableHelp ()
{
	ShowHelpPage (3);
}

void HelpFrame::ShowComparisonHelp ()
{
	ShowHelpPage (4);
}

void HelpFrame::ShowUniquenessHelp ()
{
  ShowHelpPage (5);
}

void HelpFrame::OnUserClose (wxCommandEvent & WXUNUSED(event))
{
	Show (false);
}

void HelpFrame::OnSystemClose (wxCloseEvent & WXUNUSED(event))
{
	Show (false);
}


