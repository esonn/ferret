#if !defined comparison_view_h
#define comparison_view_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/splitter.h>
#include <wx/sstream.h>

#include "tokenset.h"
#include "document.h"
#include "ferretapp.h"
#include "resultstable.h"

// the document view displays the given two documents, with matching text highlighted
// ComparisonTableView is provided as a parent, so these new frames are automatically closed 
// on exiting the main window.

// A parent panel for a document display
class DocumentView: public wxPanel
{
	public:
		DocumentView (wxWindow * parent, ComparisonTableView * ferretparent, 
				int main_document, int compared_document, bool unique, bool ignore);
		void LoadDocument ();
		void AddTupleStart (wxString tuple, int start, int end);
		void StartTupleSearch (wxString tuple);
		wxTextCtrl * GetTextCtrl () const { return _docObserver; }
	private:
		void OnFind (wxCommandEvent & event);
		void StartFind ();
		int 			  _main_document;
		int 			  _compared_document;
		wxTextCtrl 		* _docObserver;
		ComparisonTableView 	* _ferretparent;
		wxString		  _pathname;
		std::map<wxString, std::vector<int> > _trigram_starts;
		std::map<wxString, std::vector<int> > _trigram_ends;
		bool			  _inside_search;
		std::vector<int>	  _search_starts;
		std::vector<int>	  _search_ends;
    std::vector<wxTextAttr> _search_style;
		int			  _search_posn;
    bool      _unique; // display unique trigrams only
    bool      _ignore_template_material; 
		DECLARE_EVENT_TABLE()
};

// used to paint a marked up document onto the Textctrl
class TextctrlReport : public OutputReport
{
	public:
		TextctrlReport (DocumentList & doclist, DocumentView * view, bool unique, bool ignore);
		void WriteReport (wxTextCtrl * text, int doc1, int doc2);
	private:
		void ProcessTrigram (wxString trigram, int start, int end);
		void StartCopiedBlock (bool is_unique, bool is_template);
		void StartNormalBlock ();
		void WriteString (wxString str);
	private:
		DocumentView * _view;
		wxTextCtrl * _text;
		wxTextAttr _normal_style;
		wxTextAttr _bold_style;
		wxTextAttr _bold_red_style;
    wxTextAttr _template_style;
};

// implement our own form of list control to display the tuples
class DocumentComparisonView;	// Forward declaration
class TrigramList: public wxListCtrl
{
	public:
		TrigramList (DocumentComparisonView * parent, wxPanel * parentpanel)
			: wxListCtrl (parentpanel, wxID_ANY,
					wxDefaultPosition, wxDefaultSize,
					wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxLC_VIRTUAL),
			  _parent (parent)
		{}
		wxString OnGetItemText (long item, long column) const;
		void OnListItemActivated (wxListEvent & event);
	private:
		DECLARE_EVENT_TABLE()
		DocumentComparisonView	* _parent;
};

class DocumentComparisonView: public wxFrame
{
	public:
		DocumentComparisonView (ComparisonTableView * parent, wxString title, int document1, int document2, bool unique, bool ignore);
		~DocumentComparisonView ();
		void LoadDocuments ();
		wxString GetTuple (long item);
		void OnHelp (wxCommandEvent & event);
		void OnCreateReport (wxCommandEvent & event);
		void OnClose (wxCommandEvent & event);
		DocumentView * GetDocument1View () { return _document1_view; }
		DocumentView * GetDocument2View () { return _document2_view; }
	private:
		DocumentView		* _document1_view;
		DocumentView		* _document2_view;
		int			  _document1;
		int			  _document2;
    bool      _unique;
    bool      _ignore_template_material;
		ComparisonTableView	* _ferretparent;
		TrigramList		* _trigramList;
		wxSortedArrayString	  _matchingtrigrams;
		DECLARE_EVENT_TABLE()
};

#endif

