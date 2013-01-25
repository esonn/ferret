#if !defined selectfiles_h
#define selectfiles_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include <wx/dnd.h>
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
  ID_ADD_DIR,
	ID_CLEAR_FILES,
	ID_RUN_FERRET,
	ID_EXTRACT_BROWSE,
	ID_EXTRACT_DIR_NAME,
	ID_COPY_ALL,
	ID_EXTRACT_ALL,
	ID_IGNORE_UNKNOWN,
	ID_FILE_LIST,
	ID_SETTINGS,
  ID_GROUP_DIRS
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
    void ClearPaths ();
    void AddPath (wxString path);
    int GetCount () const;
    wxString GetItem (int i) const;
	private:
		void OnMouseEvent (wxMouseEvent & WXUNUSED(event)) { ; }
		void OnKeyEvent (wxKeyEvent & WXUNUSED(event)) { ; }
		DECLARE_EVENT_TABLE()
    wxArrayString _paths;
};

/** SelectFiles is the initial frame seen in the main ferret program.
  * User is asked to add documents to the list for comparing.
  */
class SelectFiles: public wxFrame
{
	public:
		SelectFiles ();
	private:
		void DownloadFiles ();
		bool ExtractFiles (int start_from = 0);
		void OnAdd (wxCommandEvent & event);
    void OnAddDir (wxCommandEvent & event);
		void OnSearchClicked (wxCommandEvent & event);
    bool ContainsOnlyDirectories ();
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
		DocumentList * _document_list;
		DECLARE_EVENT_TABLE()
  friend class DropFiles;
};

// *** Class to accept dropped list of files 
class DropFiles: public wxFileDropTarget
{
  public:
    DropFiles (SelectFiles * document_list) : wxFileDropTarget ()
    { 
      _document_list = document_list;
    }
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
  private:
    SelectFiles * _document_list;
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
	
