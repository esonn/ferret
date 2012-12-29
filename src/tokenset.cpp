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
{
	// the following list of common words will be placed into a sorted array
	// on construction of TokenSet.
	// list taken from: http://www.askoxford.com/worldofwords/wordfrom/revisedcoed11/?view=uk
	// -- extend list just by adding new strings
	// -- must end with END !
	wxString list_of_words [] = { 
		wxT("the"), wxT("be"), wxT("to"), wxT("of"), wxT("and"), wxT("a"), wxT("in"), wxT("that"), wxT("have"), wxT("I"), 
		wxT("it"), wxT("for"), wxT("not"), wxT("on"), wxT("with"), wxT("he"), wxT("as"), wxT("you"), wxT("do"), wxT("at"), 
		wxT("this"), wxT("but"), wxT("his"), wxT("by"), wxT("from"), wxT("they"), wxT("we"), wxT("say"), wxT("her"), wxT("she"), 
		wxT("or"), wxT("an"), wxT("will"), wxT("my"), wxT("one"), wxT("all"), wxT("would"), wxT("there"), wxT("their"), wxT("what"), 
		wxT("so"), wxT("up"), wxT("out"), wxT("if"), wxT("about"), wxT("who"), wxT("get"), wxT("which"), wxT("go"), wxT("me"), 
		wxT("when"), wxT("make"), wxT("can"), wxT("like"), wxT("time"), wxT("no"), wxT("just"), wxT("him"), wxT("know"), wxT("take"), 
		wxT("person"), wxT("into"), wxT("year"), wxT("your"), wxT("good"), wxT("some"), wxT("could"), wxT("them"), wxT("see"), wxT("other"), 
		wxT("than"), wxT("then"), wxT("now"), wxT("look"), wxT("only"), wxT("come"), wxT("its"), wxT("over"), wxT("think"), wxT("also"), 
		wxT("back"), wxT("after"), wxT("use"), wxT("two"), wxT("how"), wxT("our"), wxT("work"), wxT("first"), wxT("well"), wxT("way"), 
		wxT("even"), wxT("new"), wxT("want"), wxT("because"), wxT("any"), wxT("these"), wxT("give"), wxT("day"), wxT("most"), wxT("us"),
		//  single letters from contractions (won't, let's, ...)  and common html tags, etc
		wxT("b"), wxT("c"), wxT("p"), wxT("s"), wxT("e"), wxT("www"), wxT("h"), wxT("was"), wxT("were"), wxT("html"), 
		wxT("t"), wxT("br"), wxT("http"), wxT("is"), wxT("went"), wxT("are"),       
		wxT("END") // required to finish the array without having to count the words
	};
	int i = 0;
	while (!list_of_words[i].IsSameAs (wxT("END"))) 
	{
		_common_words.Add (list_of_words[i]);
		i += 1;
	}
}

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

bool TokenSet::IsUncommonWord (std::size_t token)
{
	return (_common_words.Index (GetStringFor (token)) == wxNOT_FOUND); // uncommon if word not found
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
