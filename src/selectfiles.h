#if !defined selectfiles_h
#define selectfiles_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/spinctrl.h>
#include <vector>

#include "document.h"
#include "documentlist.h"
#include "ferretapp.h"
#include "helpframe.h"
#include "resultstable.h"

// some constants, for identifying widgets in the different displays
enum { 	ID_ADD_FILES = wxID_HIGHEST + 100,
	ID_CLEAR_FILES,
	ID_RUN_FERRET,
	ID_EXTRACT_BROWSE,
	ID_EXTRACT_DIR_NAME,
	ID_DOWNLOAD_BROWSE,
	ID_DOWNLOAD_DIR_NAME,
	ID_COPY_ALL,
	ID_EXTRACT_ALL,
	ID_IGNORE_UNKNOWN,
	ID_TYPE_SELECTION,
	ID_FILE_LIST,
	ID_SETTINGS,
	ID_WEBFERRET,
	ID_MAX_DOWNLOADS,
	ID_MAX_RESULTS,
	ID_MAX_TUPLE_SEARCHES
};

// *** Utility class: ListCtrl without choice of selection, used for main document list
class MyListCtrl: public wxListCtrl
{
	public:
		MyListCtrl (wxWindow * parent, wxWindowID id)
			: wxListCtrl (parent, id, 
					wxDefaultPosition, wxDefaultSize,
					wxLC_LIST | wxLC_SINGLE_SEL | wxSIMPLE_BORDER)
			{}
	private:
		void OnMouseEvent (wxMouseEvent & WXUNUSED(event)) { ; }
		void OnKeyEvent (wxKeyEvent & WXUNUSED(event)) { ; }
		DECLARE_EVENT_TABLE()
};

/** SelectFiles is the initial frame seen in the main ferret program.
  * User is asked to add documents to the list for comparing.
  */
class SelectFiles: public wxFrame
{
	enum {TextType, CodeType};
	public:
		SelectFiles ();
	private:
		void DownloadFiles ();
		bool ExtractFiles (int start_from = 0);
		void OnAdd (wxCommandEvent & event);
		void OnSearchClicked (wxCommandEvent & event);
		void UpdateButtons ();
		void OnClear (wxCommandEvent & event);
		void OnOptions (wxCommandEvent & event);
		void OnHelp (wxCommandEvent & event);
		void OnRun (wxCommandEvent & event);
		void OnClose (wxCloseEvent & event);
		void AddDocuments (wxArrayString & paths);
		void WarnOfProblemFiles ();
		void CreateComparisonView ();
		void ReadDocuments (int start_from = 0);
		Document::DocumentType GetDocumentType () const;
		DocumentList * _document_list;
		DECLARE_EVENT_TABLE()
};


// *** dialog to set options

class OptionSettings: public wxDialog
{
	public:
		OptionSettings (wxWindow * parent);
	private:
		void OnDownloadBrowse (wxCommandEvent & event);
		void OnExtractBrowse (wxCommandEvent & event);
		void OnBrowse (int dir_name_id);
		void OnOk (wxCommandEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif
	
