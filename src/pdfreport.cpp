#include "pdfreport.h"

void PdfTable::FancyTable(wxArrayString& header, wxArrayPtrVoid & data)
{
	//Colours, line width and bold font
	SetFillColour (wxColour (255, 0, 0));
	SetTextColour (255);
	SetDrawColour (wxColour (128, 0, 0));
	SetLineWidth (.3);
	SetFont (_T(""), _T("B"));
	// Header
	double w[3] = {75, 75, 40};	// these give the widths of the columns
	for (size_t i = 0; i < header.GetCount(); i++)
	{
		Cell(w[i], 7, header[i], wxPDF_BORDER_FRAME, 0, wxPDF_ALIGN_CENTER, 1);
	}
	Ln ();
	// Colour and font restoration
	SetFillColour (wxColour (224, 235, 255));
	SetTextColour (0);
	SetFont (_T(""));
	// Data
	int fill = 0;
	for (size_t j = 0; j < data.GetCount(); j++)
	{
		wxArrayString* row = (wxArrayString*) data[j];
		// TODO: Check for overflowing of cells when printing
		Cell(w[0], 6, (*row)[0], wxPDF_BORDER_LEFT | wxPDF_BORDER_RIGHT, 0, wxPDF_ALIGN_CENTER, fill);
		Cell(w[1], 6, (*row)[1], wxPDF_BORDER_LEFT | wxPDF_BORDER_RIGHT, 0, wxPDF_ALIGN_CENTER, fill);
		Cell(w[2], 6, (*row)[2], wxPDF_BORDER_LEFT | wxPDF_BORDER_RIGHT, 0, wxPDF_ALIGN_CENTER, fill);
		Ln ();
		fill = 1 - fill;
	}
	Cell ((w[0]+w[1]+w[2]), 0, _T(""), wxPDF_BORDER_TOP);
}

void PdfTable::Footer()
{
	SetY (-15); // position -1.5cm from bottom
	SetFont (_T("Arial"), _T("I"), 9);
	WriteCell (10, wxGetApp().GetGeneratedByString ());
	Cell (0, 10, wxString::Format(_T("Page %d/{nb}"), PageNo()),
			0, 0, wxPDF_ALIGN_RIGHT);
}

void PdfTable::PrintLine (wxString line)
{
	Cell (50, 10, line, 0, 0, wxPDF_ALIGN_LEFT);
	Ln (5);
}

void PdfTable::PrintList (wxString title, wxSortedArrayString list)
{
	if (list.IsEmpty ()) return; // display nothing for an empty list
	PrintLine (title);
	for (int i = 0, n = list.size (); i < n; ++i)
	{
		PrintLine (list[i]);
	}
}

SaveTableThread::SaveTableThread (wxString save_report_path, DocumentList & doclist, DocumentListCtrl & resObs)
	: _save_report_path (save_report_path), 
  _documentlist (doclist), 
  _resemblanceObserver (resObs)
{}

void * SaveTableThread::Entry ()
{
	PdfTable pdf;
	wxArrayString header;
	header.Add ("Document 1");
	header.Add ("Document 2");
	header.Add ("Similarity");

	pdf.AliasNbPages();
	pdf.AddPage ();
	// put initial summary information
	pdf.SetFont (_T("Arial"), _T("B"), 14);
	pdf.Cell (50);
	pdf.Cell (50, 10, "Table of Similarity Scores from Ferret", 
			0, 1, wxPDF_ALIGN_CENTER);
  if (_resemblanceObserver.RemoveCommonTrigramsSet ())
  {
    pdf.Cell (50, 10, "(Pairwise similarity ignores trigrams in common with other documents)",
        0, 1, wxPDF_ALIGN_CENTER);
  }
	pdf.SetFont (_T("Arial"), _T(""), 11);
	pdf.PrintLine (wxString::Format("Number of documents compared: %d",
				_documentlist.Size()));
	pdf.PrintLine (wxString::Format("Number of pairs of documents: %d",
				_documentlist.NumberOfPairs ()));

	// add summary of problem or ignored files, if appropriate
	pdf.PrintList ("Files from which text could not be extracted:",
			wxGetApp().GetProblemFiles ());
	pdf.PrintList ("Files which were ignored, due to unknown filetypes:",
			wxGetApp().GetIgnoredFiles ());
	pdf.Ln (5);

	// create data to display from resemblanceObserver -- so order is same as in display
	wxArrayPtrVoid data;
	for (int i = 0, n= _resemblanceObserver.GetNumberItems(); i < n; ++i)
	{
		wxArrayString* row = new wxArrayString;
		data.Add(row);
		row->Add(_resemblanceObserver.OnGetItemText ((long)i, (long)0));
		row->Add(_resemblanceObserver.OnGetItemText ((long)i, (long)1));
		row->Add(_resemblanceObserver.OnGetItemText ((long)i, (long)2));
	}

	pdf.FancyTable (header, data);
	// clean up, deleting each of the rows manually
	for (size_t j = 0, n= data.GetCount(); j < n; ++j)
	{
		wxArrayString * row = (wxArrayString *) data[j];
		delete row;
	}

	pdf.SaveAsFile (_save_report_path);
}

void PdfDocumentComparison::Footer()
{
	// Position at 1.5 cm from bottom
	SetY (-15);
	SetFont (_T("Arial"), _T("I"), 9);
	SetTextColour (wxColour ("BLACK"));
	WriteCell (10, wxGetApp().GetGeneratedByString ());
	Cell (0, 10, wxString::Format(_T("Page %d of {nb}"), PageNo()),
			0, 0, wxPDF_ALIGN_RIGHT);
}

PdfReport::PdfReport (DocumentList & doclist, bool unique)
	: OutputReport (doclist, unique)
{
}

void PdfReport::WritePdfReport (wxString save_report_path, int document1, int document2)
{
	_pdf.AliasNbPages ();
	_pdf.AddPage ();
	// initial summary
	_pdf.SetFont (_T("Arial"), _T("B"), 14);
	PrintLine (wxString::Format ("Comparing '%s' with '%s'",
				_doclist[document1]->GetName().c_str(), 
				_doclist[document2]->GetName().c_str()), 
			wxPDF_ALIGN_CENTER);
	_pdf.SetFont (_T("Arial"), _T(""), 11);
	PrintLine (wxString::Format ("Document 1 source: %s",
				_doclist[document1]->GetOriginalPathname().c_str()));
	PrintLine (wxString::Format ("Document 2 source: %s",
				_doclist[document2]->GetOriginalPathname().c_str()));
	PrintLine (wxString::Format ("Number of trigrams in Document 1: %d",
				_doclist.CountTrigrams (document1, _unique)));
	PrintLine (wxString::Format ("Number of trigrams in Document 2: %d",
				_doclist.CountTrigrams (document2, _unique)));
	PrintLine (wxString::Format ("Number of common trigrams: %d",
				_doclist.CountMatches (document1, document2, _unique)));
	PrintLine (wxString::Format ("Similarity measure: %f",
				_doclist.ComputeResemblance (document1, document2, _unique)));
	PrintLine (wxString::Format ("Containment of Document 1 in Document 2: %f",
				_doclist.ComputeContainment (document1, document2, _unique)));
	PrintLine (wxString::Format ("Containment of Document 2 in Document 1: %f",
				_doclist.ComputeContainment (document2, document1, _unique)));
	_pdf.Ln();
  if (_unique)
  {
    PrintLine ("(Pairwise similarity ignores trigrams in common with other documents)");
  }
	PrintLine ("(Text highlighted in bold/blue is duplicated in the two documents)");
	// do documents
	PrintDocument (1, document1, document2);
	PrintDocument (2, document2, document1);
	// done
	_pdf.SaveAsFile (save_report_path);
}

void PdfReport::DocumentTitle (int num, const wxString & label)
{
	_pdf.SetFont (_T("Arial"), _T(""), 12);
	_pdf.SetFillColour (wxColour (200, 220, 255));  // colour for title
	_pdf.MultiCell (0, 6, wxString::Format (_T("Document %d : "), num) + label, 0, wxPDF_ALIGN_LEFT, 1);
	_pdf.Ln (4);
}

void PdfReport::PrintDocument (int num, int doc1, int doc2)
{
	_pdf.AddPage ();
	_pdf.SetFont (_T("Times"), _T(""), 12); // set font to Times 12pt
	DocumentTitle (num, _doclist[doc1]->GetName ());
	WriteDocument (doc1, doc2);
}

void PdfReport::PrintLine (wxString line, int alignment)
{
	_pdf.MultiCell (0, 10, line, 0, alignment); // permit printing on multiple lines
}

void PdfReport::WriteDocumentFooter ()
{
	// Finish off with a comment
	_pdf.Ln();
	_pdf.Ln();
	_pdf.SetFont(_T(""),_T("I"));
	_pdf.Cell(0,5,_T("(end of document)"));
}

void PdfReport::EndBlock ()
{
	StartNormalBlock (); // change font to black text
}

void PdfReport::StartCopiedBlock (bool unique)
{
	_pdf.SetFont (_T(""), _T("B"));
	_pdf.SetTextColour (wxColour ((unique ? "RED" : "BLUE")));
}

void PdfReport::StartNormalBlock ()
{
	_pdf.SetFont (_T(""), _T(""));
	_pdf.SetTextColour (wxColour ("BLACK"));
}

void PdfReport::WriteString (wxString str)
{
	_pdf.WriteCell (5, str);
}

