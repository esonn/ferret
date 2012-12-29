#if !defined results_table_h 
#define results_table_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <algorithm>

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/config.h>
#include <wx/cursor.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/html/htmlwin.h>
#include <wx/listctrl.h>
#include <wx/progdlg.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/sstream.h>
#include <wx/statline.h>
#include <wx/textfile.h>

#include "tokenset.h"
#include "documentlist.h"
#include "pdfreport.h"
#include "xmlreport.h"
#include "ferretapp.h"
#include "helpframe.h"

// some constants, for identifying widgets in the different displays
enum { 	ID_RANK_1 = wxID_HIGHEST + 1,
	ID_RANK_2,
	ID_RANK_R,
	ID_SAVE_REPORT,
	ID_CREATE_REPORT,
	ID_DISPLAY_TEXTS
};

// *** helper functions
wxStaticText * MakeStaticText (wxWindow * parent, wxString title);
wxButton * MakeButton (wxWindow * parent, int id, wxString title, wxString tooltip, bool enabled = true);
wxCheckBox * MakeCheckBox (wxWindow * parent, int id, wxString title, wxString tooltip, bool checked = false);
wxSpinCtrl * MakeSpinCtrl (wxWindow * parent, int id, wxString tooltip, int min_value, int max_value, int start_value);

class ComparisonTableView;	// Forward declaration
class DocumentListCtrl: public wxListCtrl
{
	enum Sort { sortDoc1, sortDoc2, sortResemblance };
	struct namecmp {
		static std::vector<wxString> * names;
		static std::vector<int> * documents;

		bool operator() (int x, int y) const
		{
			return ((*names)[(*documents)[x]] < 
				(*names)[(*documents)[y]]);
		}
	};
	public:
		DocumentListCtrl (ComparisonTableView * ferretparent, wxPanel * parent)
			: wxListCtrl (parent, wxID_ANY,
					wxDefaultPosition, wxDefaultSize,
					wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL | wxSIMPLE_BORDER ),
			  _ferretparent (ferretparent)
		{}
		void UpdatedDocumentList ();
		void SelectFirstItem ();
		void SortOnDocument1 ();
		void SortOnDocument2 ();
		void SortOnResemblance ();
		wxString OnGetItemText (long item, long column) const;
		void OnListItemActivated (wxListEvent & event);
		void ShowSelectedItem ();
		void SaveReportFor (int document1, int document2);
		void SaveSelectedItem ();
		void OnSortColumn (wxListEvent & event);
		int GetNumberItems () const;
		wxString GetPathname (int i) const;
		wxString GetName (int i) const;
	private:
		void SortOnDocument (int num_doc);
		void ShowItem (long item_number);
		DECLARE_EVENT_TABLE()
		ComparisonTableView 	* _ferretparent;
		std::vector<int> _document1;
		std::vector<int> _document2;
		std::vector<int> _sortedIndices;
		Sort _lastsort;
};

// The ComparisonTableView displays the list of compared document pairs, allowing the user to 
// select a pair to view
class ComparisonTableView: public wxFrame
{
	public:
		ComparisonTableView ();
		void OnRank1   	   (wxCommandEvent & event);
		void OnRank2   	   (wxCommandEvent & event);
		void OnRank3   	   (wxCommandEvent & event);
		void OnDisplayTexts (wxCommandEvent & event);
		void OnCreateReport (wxCommandEvent & event);
		void OnHelp   	   (wxCommandEvent & event);
		void OnSaveReport  (wxCommandEvent & event);
		void OnQuit        (wxCommandEvent & event);
		void OnClose (wxCloseEvent & event);
		void SetDocumentList (DocumentList & documentlist);
		void SaveReportFor (int document1, int document2);
		wxString GetName (int document) const;
		DocumentList & GetDocumentList ()
		{ 
			return _documentlist;
		}
	private:
		void OnResize (wxSizeEvent & event);
		void CalculateMeasures ();
		DECLARE_EVENT_TABLE()
		DocumentList 	 _documentlist;
		DocumentListCtrl * _resemblanceObserver;
};

#endif
