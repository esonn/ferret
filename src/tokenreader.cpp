#include "tokenreader.h"

TokenReader::TokenReader (wxInputStream & input)
	: _input (input),
	  _position (0),
	  _done (false)
{}

std::size_t TokenReader::GetToken (TokenSet & tokenset)
{
	return tokenset.GetIndexFor (_token.GetString ());
}

bool TokenReader::IsFinished () const 
{
	return _done;
}

int TokenReader::GetTokenStart () const
{
	return _token_start;
}

int TokenReader::GetTokenEnd () const
{
	return _token_start + _token.GetLength ();
}

// check if given character is a member of standard roman alphabet
bool WordReader::IsAlphabetChar (wxChar ch)
{
	return wxIsalnum (ch); // (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

// this function checks if the input character is from a language
// representing words as single characters.  Currently, this works 
// only for Chinese.
bool WordReader::IsSingleCharWord (wxChar ch)
{
	return (ch >= 0x3400 && ch < 0xa000); // check if Chinese
}

// WordReader identifies words as sequences of alphabetic characters
//  -- using IsSingleCharWord, WordReader also separates out words from
//     languages like Chinese which can represent a complete words as a 
//     single character
bool WordReader::ReadToken ()
{
	if (_done) return false;	// reading is done
	// step to first alphabetical character
	do
	{
		_look = _input.GetC ();
		_position++;
	}
	while (!IsAlphabetChar (_look) && _input.CanRead ());
	// check for finished
	if (!_input.CanRead ())
	{
		_done = true;	// mark reading as done
		return false;	// return with no token read
	}
	// read in the alphabetical characters
	_token.Erase ();		// start a new token
	_token_start = _position-1;	// - 1 because first character is in _look
	if (IsSingleCharWord (_look))
	{
		_token.AddChar (_look);
	}
	else
	{
		do
		{
			_token.AddChar (tolower (_look)); // put everything into lower case
			_look = _input.GetC ();
			_position++;
		}
		while (IsAlphabetChar (_look) && !IsSingleCharWord (_look) && _input.CanRead ());
		_input.Ungetch (_look);	// replace last character, as not part of token
		_position--;
	}
	// check for finished
	if (!_input.CanRead ()) _done = true;	// mark reading as done

	return true;
}

bool CCodeReader::IsSymbol (wxChar c)
{
	return ( c == '!' || c == '%' || c == '/' || c == '*' || c == '+' ||
		 c == '-' || c == '=' || c == '|' || c == ',' || c == '?' || 
		 c == '.' || c == '&' || c == '(' || c == ')' || c == '{' || 
		 c == '}' || c == '<' || c == '>' || c == ':' || c == ';' || 
		 c == '^' || c == '[' || c == ']' || c == '"' || c == '#' ||
		 c == '~' );
}

bool CCodeReader::IsSymbol (wxString token, wxChar c)
{
	wxString candidate = token + c;
	return ( candidate == wxT("!=") || candidate == wxT("++") || 
		 candidate == wxT("--") || candidate == wxT("==") || 
		 candidate == wxT(">=") || candidate == wxT("<=") || 
		 candidate == wxT("||") || candidate == wxT("&&") || 
		 candidate == wxT("+=") || candidate == wxT("-=") || 
		 candidate == wxT("*=") || candidate == wxT("/=") || 
		 candidate == wxT("%=") || candidate == wxT("&=") || 
		 candidate == wxT("|=") || candidate == wxT("^=") || 
		 candidate == wxT("::") || candidate == wxT("->") || 
		 candidate == wxT("//") || candidate == wxT("<<") || 
		 candidate == wxT(">>") || candidate == wxT("##") || 
		 candidate == wxT("/*") || candidate == wxT("*/") || 
		 candidate == wxT("/**") );
}

bool CCodeReader::ReadToken ()
{
	if (_done) return false;
	// step to first non-whitespace character
	do
	{
		_look = _input.GetC ();
		_position++;
	}
	while (std::isspace (_look) && _input.CanRead ());
	// check for finished
	if (!_input.CanRead ())
	{
		_done = true;	// mark reading as done
		return false;	// return with no token read
	}
	// read in the token
	_token.Erase ();		// start a new token
	_token_start = _position-1;	// - 1 because first character is in _look
	// check for different cases -- note, precise syntax not important!
	if (IsSymbol (_look))
	{
		// read in a symbol
		do
		{
			_token.AddChar (_look);
			_look = _input.GetC ();
			_position++;
		}
		while ((IsSymbol (_token.GetString (), _look)) && (_input.CanRead ()));
	}
	else if (std::isdigit(_look) || _look == '.')
	{
		// read in a number
		do
		{
			_token.AddChar (_look);
			_look = _input.GetC ();
			_position++;
		}
		while ((std::isdigit (_look) || _look == '.') && (_input.CanRead ()));
	}
	else
	{ // assume we have characters for a variable or other name 
		do
		{
			_token.AddChar (_look);
			_look = _input.GetC ();
			_position++;
		}
		while ((std::isalnum (_look) || _look == '_') && (_input.CanRead ()));
	}
	_input.Ungetch (_look); // replace last character, as not part of token
	_position--;
	// check for finished
	if (!_input.CanRead ()) _done = true;	// mark reading as done

	return true;
}

