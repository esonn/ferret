#if !defined tokenreader_h 
#define tokenreader_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <ctype.h> // gives tests for if characters are numbers, alphanumerics, etc
#include <wx/wx.h>
#include <wx/stream.h>

#include "tokenset.h"

/** The TokenReader is the parent class of the different 'token-isers'
  * -- WordReader tokenises a document into strings of alphanumeric characters
  * -- CCodeReader tokenises a document into symbols matching a C-style language
  * The token reader is initialised with an input stream
  * -- GetToken is used to 'walk through' the document, one token at a time
  *    until IsFinished returns true.
  * -- the start and end points of the token can be retrieved using the given methods,
  *    and the string making up the token can be obtained by caller from the TokenSet
  */
class TokenReader
{
	public:
		TokenReader (wxInputStream & input);
		// return index of last read token
		std::size_t GetToken (TokenSet & tokenset); // retrieve current token identifier
		bool IsFinished () const;	// return true if end-of-file reached
		int GetTokenStart () const;	// return the start position of current token
		int GetTokenEnd () const;	// return the end position of current token
		// read token, return true if successful
		// -- user of class must provide this method
		virtual bool ReadToken () = 0;
	protected: // allow subclasses to access parameters
		wxInputStream  & _input;   // the stream from which to read
		int 		_position; // current position in stream
		Token		_token;    // last token read
		int		_token_start;	// start position of last token read
		wxChar		_look;	   // lookahead character
		bool		_done;	   // becomes true when stream is completed
};

// The WordReader separates its input stream into tokens, consisting of 
//     consecutive alphabetic characters 
//     -- every character is converted to lower case
class WordReader: public TokenReader
{
	public:
		WordReader (wxInputStream & input) : TokenReader (input) {}
		bool IsAlphabetChar (wxChar ch);
		bool IsSingleCharWord (wxChar ch);
		bool ReadToken ();
};

// LispReader matches a bracketed language, suitable for 
// matching lisp/scheme/racket/clojure files
class LispCodeReader: public TokenReader
{
  public:
    LispCodeReader (wxInputStream & input) : TokenReader (input) {}
    bool ReadToken ();
};

// Template for the 'usual' programming languages (non-lisp style)
// Individual languages provide their own 'IsSymbol' methods.
class CodeReader: public TokenReader
{
  public:
    CodeReader (wxInputStream & input) : TokenReader (input) {}
    bool ReadToken ();
  private:
    bool IsSymbol (wxChar c);
    virtual bool IsSymbol (wxString token, wxChar c) = 0;
};

class ActionScriptCodeReader: public CodeReader
{
  public:
    ActionScriptCodeReader (wxInputStream & input) : CodeReader (input) {}
  private:
    bool IsSymbol (wxString token, wxChar c);
};

class CCodeReader: public CodeReader
{
	public:
		CCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class CSharpCodeReader: public CodeReader
{
	public:
		CSharpCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class HaskellCodeReader: public CodeReader
{
	public:
		HaskellCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class GroovyCodeReader: public CodeReader
{
	public:
		GroovyCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class JavaCodeReader: public CodeReader
{
	public:
		JavaCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class PhpCodeReader: public CodeReader
{
  public:
    PhpCodeReader (wxInputStream & input) : CodeReader (input) {}
  private:
    bool IsSymbol (wxString token, wxChar c);
};

class VbCodeReader: public CodeReader
{
	public:
		VbCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class RubyCodeReader: public CodeReader
{
	public:
		RubyCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class PrologCodeReader: public CodeReader
{
	public:
		PrologCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

class PythonCodeReader: public CodeReader
{
	public:
		PythonCodeReader (wxInputStream & input) : CodeReader (input) {}
	private:
		bool IsSymbol (wxString token, wxChar c);
};

// For reading XML/HTML, simply recognise the tokens such as '<? />' etc
// and process rest like a standard computer program.
// May need revisiting to more correctly parse this kind of document.
class XmlCodeReader: public CodeReader
{
  public:
    XmlCodeReader (wxInputStream & input) : CodeReader (input) {}
  private:
    bool IsSymbol (wxString token, wxChar c);
};

#endif

