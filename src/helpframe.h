#if !defined helpframe_h
#define helpframe_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/statline.h>

#include "ferretapp.h"

enum { ID_HELP_PAGES = wxID_HIGHEST + 1};

/** HelpFrame - display help, allowing topic to be matched to help buttons
  *           - dialog is never deleted by application, only hidden when 'close' is called by user
  */
class HelpFrame: public wxDialog
{
	public:
		HelpFrame ();
		void ShowSelectionHelp ();
		void ShowTableHelp ();
		void ShowComparisonHelp ();
	private:
		wxPanel * MakePage (wxWindow * parent, wxString title, wxString displayed_text);
		void ShowHelpPage (int n);
		void OnUserClose (wxCommandEvent & WXUNUSED(event));
		void OnSystemClose (wxCloseEvent & WXUNUSED(event));
		DECLARE_EVENT_TABLE()
};

#endif

