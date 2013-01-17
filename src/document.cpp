#include "document.h"

Document::Document (wxString pathname, int id)
	: _pathname (pathname),
	  _original_pathname (pathname),
	  _num_trigrams (0),
	  _group_id (id)
{
	wxFileName filename (pathname);
	_name = filename.GetFullName ();
}

Document::Document (Document * document)
	: _pathname (document->_pathname),
	  _original_pathname (document->_original_pathname),
	  _name (document->_name),
	  _num_trigrams (0),
	  _group_id (document->_group_id)
{}

wxString Document::GetPathname () const 
{
	return _pathname;
}

// Retrieve the pathname of the source for this document
// -- _pathname differs from _original_pathname if it represents text
//    extracted from a non-text source, such as pdf.
wxString Document::GetOriginalPathname () const
{
	return _original_pathname;
}

void Document::SetOriginalPathname (wxString pathname)
{
	_original_pathname = pathname;
}

void Document::SetPathname (wxString pathname)
{
	_pathname = pathname;
}

void Document::SetName (wxString name)
{
	_name = name;
}

wxString Document::GetName () const
{
	return _name;
}

int Document::GetGroupId () const
{
	return _group_id;
}

void Document::SetGroupId (int id) 
{
	_group_id = id;
}

int Document::GetTrigramCount () const
{
	return _num_trigrams;
}

// WARNING: This method should only be used when document definitions are 
// loaded from a file.
void Document::SetTrigramCount (int count)
{
	_num_trigrams = count;
}

void Document::ResetTrigramCount ()
{
	_num_trigrams = 0;
}

void Document::IncrementTrigramCount ()
{
	_num_trigrams += 1;
}

// Start input from the file referred to by this document
bool Document::StartInput (TokenSet & tokenset)
{
	_fb = new wxFile (GetPathname ());
	if (_fb->IsOpened ())
	{
		_cin = new wxFileInputStream (* _fb);
		InitialiseInput (tokenset);
		return true;	// signify file opened correctly
	}
	else
	{
		return false;
	}
}

// Start input from a provided input stream
bool Document::StartInput (wxInputStream & input, TokenSet & tokenset)
{
	_cin = &input;
	InitialiseInput (tokenset);
	return true;
}

// Start input by constructing a new Reader based on current document type
// TokenSet is provided by caller, so Reader uses common set of labels for tokens
void Document::InitialiseInput (TokenSet & tokenset)
{
	if (IsTextType ())
	{
		_token_input = new WordReader (* _cin);
	}
  else if (IsCCodeType ())
	{
		_token_input = new CCodeReader (* _cin);
	}
  else if (IsHaskellCodeType ())
  {
    _token_input = new HaskellCodeReader (* _cin);
  }
  else if (IsGroovyCodeType ())
  {
    _token_input = new GroovyCodeReader (* _cin);
  }
  else if (IsJavaCodeType ())
  {
    _token_input = new JavaCodeReader (* _cin);
  }
  else if (IsVBCodeType ())
  {
    _token_input = new VbCodeReader (* _cin);
  }
  else if (IsRubyCodeType ())
  {
    _token_input = new RubyCodeReader (* _cin);
  }
  else if (IsPrologCodeType ())
  {
    _token_input = new PrologCodeReader (* _cin);
  }
  else if (IsPythonCodeType ())
  {
    _token_input = new PythonCodeReader (* _cin);
  }
  else if (IsLispCodeType ())
  {
    _token_input = new LispCodeReader (* _cin);
  }
  else if (IsXmlCodeType ())
  {
    _token_input = new XmlCodeReader (* _cin);
  }
  else // default -- treat as text type
  {
		_token_input = new WordReader (* _cin);
  }
	ReadTrigram (tokenset); // read first two tokens so next call to 
	ReadTrigram (tokenset); // ReadTrigram returns the first complete trigram
}

// returns true if this document's filetype is the same as the given extension
// -- note, case is ignored, so "txt" == "TXT" == "tXt"
bool Document::IsFileType (wxString extension) const
{
	int dot_posn = _pathname.Find (wxChar('.'), true); // search for last dot, i.e. from end
	if (dot_posn == wxNOT_FOUND) return false;
	wxString file_extension = _pathname.Mid (dot_posn+1);

	return file_extension.IsSameAs (extension, false); // ignore case in comparison
}

// Test if file extension represents a pdf document
bool Document::IsPdfType () const
{
	return IsFileType (wxT ("pdf"));
}

// Test if file extension represents a pure text document
bool Document::IsTxtType () const
{
	return IsFileType (wxT ("txt"));
}

// Test if file extension represents a word-processor format
bool Document::IsWordProcessorType () const
{
	return 	IsFileType (wxT ("abw")) ||
		IsFileType (wxT ("doc")) ||
		IsFileType (wxT ("docx")) ||
		IsFileType (wxT ("rtf"));
}

// Test if document should be processed using WordReader tokens.
bool Document::IsTextType () const
{
  return IsWordProcessorType () || IsPdfType () || IsTxtType ();
}

bool Document::IsCCodeType () const
{
	return  IsFileType (wxT ("cpp")) ||
		IsFileType (wxT ("c")) ||
		IsFileType (wxT ("h"));
}

bool Document::IsHaskellCodeType () const
{
  return IsFileType (wxT ("hs")) || IsFileType (wxT ("lhs"));
}

bool Document::IsJavaCodeType () const
{
	return IsFileType (wxT ("java"));
}

bool Document::IsGroovyCodeType () const
{
	return IsFileType (wxT ("groovy"));
}

bool Document::IsVBCodeType () const
{
  return IsFileType (wxT ("vb"));
}

bool Document::IsRubyCodeType () const
{
  return IsFileType (wxT ("rb"));
}

bool Document::IsPrologCodeType () const
{
  return IsFileType (wxT ("pl"));
}

bool Document::IsPythonCodeType () const
{
  return IsFileType (wxT ("py"));
}

bool Document::IsLispCodeType () const
{
  return IsFileType (wxT ("lisp")) || IsFileType (wxT ("lsp")) ||
    IsFileType (wxT ("scm")) || 
    IsFileType (wxT ("rkt")) || IsFileType (wxT ("ss")) ||
    IsFileType (wxT ("clj"));
}

bool Document::IsXmlCodeType () const
{
  return IsFileType (wxT ("xml")) || IsFileType (wxT ("html"));
}
 
bool Document::IsCodeType () const
{
  return IsCCodeType () || IsJavaCodeType () || IsVBCodeType () ||
    IsRubyCodeType () || IsPythonCodeType () || IsLispCodeType () ||
    IsXmlCodeType ();
}

// Test if file is not a known type
bool Document::IsUnknownType () const
{
	return ! (IsCodeType () || IsTextType ());
}

// Perform extraction of text from given document, if required
// -- returns true to indicate the document is in text form, or has been successfully converted,
//            and so can be processed by caller
// -- adds to list of problem files and returns false if there is a problem in conversion
bool Document::ExtractDocument (wxString & extract_folder)
{
	wxFileName this_file (_pathname);
	// consider extension on file and user's selection
	// -- extract as word-type if we are extracting all or the file type is 
	//    a word-processor extension
	// -- if extract all, ignore unknown types if that setting has been selected
	if ((wxGetApp().GetConvertAll () || IsWordProcessorType ()) &&
			(!IsUnknownType () || !wxGetApp().GetIgnoreUnknown ()))
	{
		ExtractFromWordProcessor (extract_folder);
	}
	else if (IsPdfType ())
	{
		ExtractFromPdf (extract_folder);
	}
	else if (wxGetApp().GetCopyAll () &&
			(IsTxtType () || IsCodeType () || !wxGetApp().GetIgnoreUnknown ()))
	{
		// copy file
		wxFileName new_file (extract_folder, this_file.GetFullName ());
		wxCopyFile (_pathname, new_file.GetFullPath (), true);
		SetPathname (new_file.GetFullPath ());
	}
	else if (wxGetApp().GetIgnoreUnknown () && // ignore unknown file types, if requested
			IsUnknownType ())
	{
		wxGetApp().AddIgnoredFile (_original_pathname);
		return false; // return false so as not to process this document
	}

	// check that the new document exists
	wxFileName checkfile (_pathname);
	if (!checkfile.FileExists()) // converted file does not exist, so indicate a problem
	{
		wxGetApp().AddProblemFile (_original_pathname);
		return false; // return false so as not to process this document
	}

	return true; // return true to process this document
}

// Call out to abiword to convert document
// -- note use of paths so that converted file ends in extract_folder
void Document::ExtractFromWordProcessor (wxString & extract_folder)
{
	wxFileName this_file (_pathname);
	wxFileName new_file (extract_folder, this_file.GetFullName ());
	new_file.SetExt (wxT("txt"));
#if __WXGTK__
	wxExecute (wxT("abiword --to=txt ") + 
			this_file.GetFullPath () + 
			wxT(" -o ") +
			new_file.GetFullPath (), wxEXEC_SYNC);
#elif __WXMSW__  // different calling sequence for MSW, as cannot handle paths (?? Bad coding ??)
	// copy old file
	wxFileName copy_old (extract_folder, this_file.GetFullName ());
	wxCopyFile (_pathname, copy_old.GetFullPath (), true);
	// cd to extract folder
	wxFileName::SetCwd (extract_folder);
	// do the extraction
	wxArrayString outputs;
	wxArrayString errors;
	wxExecute (wxT("\"AbiWord.exe\" --to=txt ") + 
			copy_old.GetFullName () + 
			wxT(" -o ") +
			new_file.GetFullName (), 
			outputs, errors);
	// remove the old file 
	wxRemoveFile (copy_old.GetFullPath ());
#endif
	SetPathname (new_file.GetFullPath());
}

// Call out to pdftotext to convert document
// -- note use of paths so that converted files ends in extract_folder
void Document::ExtractFromPdf (wxString & extract_folder)
{
	wxFileName this_file (_pathname);
	wxFileName new_file (extract_folder, this_file.GetFullName ());
	new_file.SetExt (wxT("txt"));
#if __WXGTK__
	wxExecute (wxT("pdftotext -layout -enc ASCII7 -nopgbrk ") + // changed Latin1 to ASCII7
			this_file.GetFullPath () +
			wxT(" ") +
			new_file.GetFullPath (), wxEXEC_SYNC);
#elif __WXMSW__   // different calling sequence for MSW, as cannot handle paths (?? Bad coding ??)
	// copy old file
	wxFileName copy_old (extract_folder, this_file.GetFullName ());
	wxCopyFile (_pathname, copy_old.GetFullPath (), true);
	// cd to extract folder
	wxFileName::SetCwd (extract_folder);
	// do the extraction
	wxArrayString outputs;
	wxArrayString errors;
	wxExecute (wxT("\"pdftotext\" -layout -enc Latin1 -nopgbrk ") + 
			copy_old.GetFullName () + 
			wxT(" ") +
			new_file.GetFullName (), 
			outputs, errors);
	// remove the old file
	wxRemoveFile (copy_old.GetFullPath ());
#endif
	SetPathname (new_file.GetFullPath());
}

// Reads next input token and updates information held on current trigram.
// return true if a trigram has been read and is ready for retrieval
bool Document::ReadTrigram (TokenSet & tokenset)
{
	_current_tuple[0] = _current_tuple[1];
	_current_tuple[1] = _current_tuple[2];
	_current_start[0] = _current_start[1];
	_current_start[1] = _current_start[2];
	if ( _token_input->ReadToken () )
	{
		_current_tuple[2] = _token_input->GetToken (tokenset);
		_current_start[2] = _token_input->GetTokenStart ();
		return true;
	}
	else 
	{
		return false;
	}
	
}

// retrieve a token of the current tuple, based on position within tuple
// -- as we only deal with trigrams, index must be in [0,2]
std::size_t Document::GetToken (int i) const
{
	assert (i>=0 && i<=2);
	return _current_tuple[i];
}

// retrieve the start position of current trigram
std::size_t Document::GetTrigramStart () const
{
	return _current_start[0];
}

// retrieve the start position of token i within current trigram
// -- used to get start position of second word
std::size_t Document::GetTrigramStart (int i) const
{
	assert (i>=0 && i<=2);
	return _current_start[i];
}

// retrieve the end position of the current token
std::size_t Document::GetTrigramEnd () const
{
	return _token_input->GetTokenEnd ();
}

// Close up the input file buffer
void Document::CloseInput ()
{
	delete _token_input;
	if (_fb != NULL && _fb->IsOpened ())
	{
		_fb->Close ();
	}
}

void Document::Save (wxFile & file)
{
	file.Write (wxT("start-document\n"));
	file.Write (wxString::Format (wxT("path\t%s\n"), _pathname.c_str ()));
	file.Write (wxString::Format (wxT("original-path\t%s\n"), _original_pathname.c_str ()));
	file.Write (wxString::Format (wxT("name\t%s\n"), _name.c_str ()));
	file.Write (wxString::Format (wxT("num-trigrams\t%d\n"), _num_trigrams));
	file.Write (wxString::Format (wxT("group-id\t%d\n"), _group_id));
	file.Write (wxT("end-document\n"));
}
