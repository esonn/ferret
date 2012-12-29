#if !defined pdfreport_h
#define pdfreport_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/pdfdoc.h>
#include <wx/sstream.h>

#include "tokenset.h"
#include "documentlist.h"
#include "outputreport.h"
#include "resultstable.h"

/** PdfTable is to create a table
  * Based heavily on examples provided with wxPdfDocument
  */
class PdfTable: public wxPdfDocument
{
	public:
		void FancyTable (wxArrayString & header, wxArrayPtrVoid & data);
		void Footer ();
		void PrintLine (wxString line);
		void PrintList (wxString title, wxSortedArrayString list);
};

/** Thread class for saving the table
  */
class DocumentListCtrl; // Forward declaration
class SaveTableThread: public wxThread
{
	public:
		SaveTableThread (wxString save_report_path, DocumentList & doclist, DocumentListCtrl & resObs);
		virtual void * Entry ();
	private:
		wxString	   _save_report_path;
		DocumentList 	 & _documentlist;
		DocumentListCtrl & _resemblanceObserver;
};

/** PdfDocumentComparison is to write the two provided documents, highlighting common tuples
  */
class PdfDocumentComparison : public wxPdfDocument
{
	public:
		void Footer ();
};

class PdfReport : public OutputReport
{
	public:
		PdfReport (DocumentList & doclist);
		void WritePdfReport (wxString save_report_path, int document1, int document2);
	private:
		void DocumentTitle (int num, const wxString& label);
		void PrintDocument (int num, int doc1, int doc2);
		void PrintLine (wxString line, int alignment = wxPDF_ALIGN_LEFT);
		void WriteDocumentFooter ();
		void EndBlock ();
		void StartCopiedBlock ();
		void StartNormalBlock ();
		void WriteString (wxString str);
	private:
		PdfDocumentComparison _pdf;
};

#endif

