#if !defined ferretapp_h
#define ferretapp_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

/** FerretApp is the main class of the application.
  * Program starts in OnInit method, which opens an instance of the document selector frame.
  * FerretApp is responsible for maintaining the help frame
  * and for maintaining options and information across the different frames of the system.
  */
class HelpFrame; // forward declaration
class FerretApp: public wxApp
{
	public:
		FerretApp ();
		virtual bool OnInit ();
		void CloseHelp ();
		void ShowSelectionHelp ();
		void ShowTableHelp ();
		void ShowComparisonHelp ();
		wxString GetVersionString ();
		wxString GetGeneratedByString ();
		// BEGIN options and information maintained for ferret application
		wxString GetExtractFolder () const;
		void SetExtractFolder (wxString extract_folder);
		bool IsTextType () const;
		void SetTextType (bool text_type);
		bool GetCopyAll () const;
		void SetCopyAll (bool copy_all);
		bool GetConvertAll () const;
		void SetConvertAll (bool convert_all);
		bool GetIgnoreUnknown () const;
		void SetIgnoreUnknown (bool ignore_unknown);
		void AddProblemFile (wxString file);
		const wxSortedArrayString & GetProblemFiles () const;
		void AddIgnoredFile (wxString file);
		const wxSortedArrayString & GetIgnoredFiles () const;
		// END
		wxPoint GetNextFramePosition (int window_width, int window_height);
	private:
		HelpFrame * _ferret_help;
		// parameters and information for file conversion
		wxString _extract_folder;
		wxSortedArrayString _problem_files;
		wxSortedArrayString _ignored_files;
		bool _text_type;
		bool _copy_all;
		bool _convert_all;
		bool _ignore_unknown;
		// parameters for placing widgets
		int _last_x;
		int _last_y;
};

DECLARE_APP(FerretApp)

#endif

