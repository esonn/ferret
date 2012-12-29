#if !defined tokenset_h
#define tokenset_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/file.h>

#include <assert.h>
#include <vector>
#include <map>

/** A Token is a sequence of characters read in by a TokenReader
  * -- this class provides a dynamic storage for the token supporting
  *    addition of characters
  * -- when finished, the token can be queried for its length and made into a string
*/
class Token
{
	public:
		Token ();
		~Token ();
		void Erase ();
		void AddChar (wxChar c);
		wxString GetString () const;
		int GetLength () const;
	private:
		void Grow ();
		wxChar * _token; // storage for the token
		int	_capacity; // size of the stored token
		int 	_top;	// pointer to end of token
};

/** A TokenSet maps strings to token indices 
  * -- this is for memory efficiency, ensuring every token's string is 
  *    stored once within the application 
  *   (this class could be removed if wxString had same property)
  */
class TokenSet
{
	public:
		TokenSet ();
		std::size_t GetIndexFor (wxString token);
		wxString GetStringFor (std::size_t token);
		void Clear ();
		bool IsUncommonWord (std::size_t token);
		// methods to Save/Retrieve tokenset
		void Save (wxFile & file);
		void SetNextIndex (int index);
		void SetIndexString (wxString token, int index);
	private:
		wxSortedArrayString _common_words;
		std::map<wxString, std::size_t> _tokens;
		std::map<wxString, std::size_t>::const_iterator _tokens_it;
		std::size_t _nextindex; // next free index for new string
		std::map<std::size_t, wxString> _strings;
		std::map<std::size_t, wxString>::const_iterator _strings_it;
};

#endif

