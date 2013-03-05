#if !defined document_h
#define document_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

#include "ferretapp.h"
#include "tokenset.h"
#include "tokenreader.h"

/** Document points to a document on the local filestore.  
  * -- each Document is initialised with a pathname and the type of a document
  *    or it may take these values from a given Document
  * -- the group_id is used to place documents into groups: documents with the same id
  *    will not be compared against each other
  * -- Document owns a TokenReader, which is created on heap during initialisation
  * -- Note the paths/names for this document:
  *    _original_pathname -- this is the path to the original source form of the document
  *    _pathname -- this is the path to the displayed text form of the document
  *                 When files have been converted to text, this holds the converted file's path.
  *    _short_path -- this is the base directory for the file provided in select files
  *                   used in results table to show short pathname
  *    _name -- this is the 'name' of the document.  For user files, it is the name of the 
  *             document.
  * -- the important part of the class is the set of methods for iterating 
  *    across the trigrams, using ReadTrigram, GetTrigramStart/End and GetToken
  */
class Document
{
	public:
		Document (wxString pathname, int id = 0);
		Document (Document * document);
		// basic information about document
		wxString GetPathname () const;
		wxString GetOriginalPathname () const;
		void SetOriginalPathname (wxString pathname);
		void SetPathname (wxString pathname);
    void SetShortPath (wxString pathname);
		void SetName (wxString name);
		wxString GetName () const;
    wxString GetShortName () const;
		// accessor/setter for group_id
		int GetGroupId () const;
		void SetGroupId (int id);
		// information about trigrams in document
		int GetTrigramCount () const;
    int GetUniqueTrigramCount () const;
    int GetEngagementCount () const;
		void SetTrigramCount (int count);
		void ResetTrigramCount ();
		void IncrementTrigramCount ();
    void IncrementUniqueTrigramCount ();
    void IncrementEngagementTrigramCount ();
		// following methods used to start, read and end processing of trigrams
		bool StartInput (TokenSet & tokenset);
		bool StartInput (wxInputStream & input, TokenSet & tokenset);
		bool ReadTrigram (TokenSet & tokenset);
		std::size_t GetToken (int i) const;		// access token of current trigram
		std::size_t GetTrigramStart () const;		// access start position of trigram
		std::size_t GetTrigramStart (int i) const;	// access start of token i in trigram
		std::size_t GetTrigramEnd () const;		// access end position of trigram
		void CloseInput ();
		// following methods check the type of the document based on its filename
		bool IsPdfType () const;
		bool IsTxtType () const;
		bool IsWordProcessorType () const;
    bool IsTextType () const;
    bool IsActionScriptCodeType () const;
		bool IsCCodeType () const;
    bool IsCSharpCodeType () const;
    bool IsGroovyCodeType () const;
    bool IsHaskellCodeType () const;
		bool IsJavaCodeType () const;
    bool IsVBCodeType () const;
    bool IsPhpCodeType () const;
    bool IsRubyCodeType () const;
    bool IsPrologCodeType () const;
    bool IsPythonCodeType () const;
    bool IsLispCodeType () const;
    bool IsXmlCodeType () const;
		bool IsCodeType () const;
		bool IsUnknownType () const;
		// extract from non-text formats
		bool ExtractDocument (wxString & extract_folder, int index); // return true if file should be removed from list
		void ExtractFromWordProcessor (wxString & extract_folder, int index);
		void ExtractFromPdf (wxString & extract_folder, int index);
		// for save/retrieve document data
		void Save (wxFile & file);
	private:
		bool IsFileType (wxString extension) const;
		void InitialiseInput (TokenSet & tokenset);
		wxString	  _pathname; 		// -- [converted] source for this document
		wxString	  _original_pathname;   // -- original source for this document
    wxString    _short_path;  // -- base directory from select files (if present)
		wxString 	  _name;		// -- filename (without path)
		int 		    _num_trigrams;
    int         _num_unique_trigrams;
    int         _engagement_count;
		wxFile 		* _fb;
		wxInputStream	* _cin;
		TokenReader 	* _token_input; // this is a pointer, because initialised separately
		std::size_t	  _current_tuple[3];
		std::size_t	  _current_start[3];
		int		      _group_id;	// an index number indicating this document's group
};

#endif
