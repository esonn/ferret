#include "tokenset.h"

Token::Token ()
	: _capacity (2), _top (0)
{
	_token = new wxChar [_capacity];
}

Token::~Token ()
{
	delete[] _token;
}

void Token::Erase ()
{
	_top = 0;
}

void Token::AddChar (wxChar c)
{
	if (_top == _capacity)
		Grow ();
	_token [_top] = c;
	++_top;
}

wxString Token::GetString () const
{
	return wxString (_token, _top);
}

int Token::GetLength () const
{
	return _top;
}

void Token::Grow ()
{
	wxChar * newtoken = new wxChar [2 * _capacity];
	for (int i = 0; i < _capacity; ++i)
		newtoken[i] = _token[i];
	_capacity = 2 * _capacity;
	delete _token;
	_token = newtoken;
}

// *** TokenSet

TokenSet::TokenSet ()
	: _nextindex (0)
{}

std::size_t TokenSet::GetIndexFor (wxString token)
{
	_tokens_it = _tokens.find (token);
	if (_tokens_it != _tokens.end())  // found it
		return _tokens_it->second;
	else // otherwise, make a new index
	{
		_tokens[token] = _nextindex;
		_strings[_nextindex] = token;
		_nextindex++;
		return _nextindex-1;
	}
}

wxString TokenSet::GetStringFor (std::size_t token)
{
	_strings_it = _strings.find (token);
	assert (_strings_it != _strings.end ()); // it's an error if token not in token set
	return wxString (_strings_it->second.c_str ());
}

void TokenSet::Clear ()
{
	_tokens.clear ();
	_strings.clear ();
	_nextindex = 0;
}

// save just one of the maps, as the other can be reconstructed (it's the inverse)
void TokenSet::Save (wxFile & file)
{
	file.Write (wxString::Format (wxT("next-index\t%d\n"), _nextindex));
	for (std::size_t i = 0; i < _nextindex; ++i)
	{
		file.Write (wxString::Format (wxT("%d\t%s\n"), i, _strings[i].c_str ()));
	}
}

void TokenSet::SetNextIndex (int index)
{
	_nextindex = index;
}

void TokenSet::SetIndexString (wxString token, int index)
{
	_strings[index] = token;
	_tokens[token] = index;
}
