#include "tupleset.h"

TupleSet::TupleSet ()
{}

void TupleSet::Clear ()
{
	_tuple_map.clear ();
}

int TupleSet::Size ()
{
	int trigram_count = 0;
	for (Begin (); HasMore (); GetNext ())
	{
		trigram_count++;
	}

	return trigram_count;
}

std::vector<int> & TupleSet::GetDocumentsForTuple (std::size_t token_0, std::size_t token_1, std::size_t token_2)
{
	return _tuple_map[token_0][token_1][token_2];
}

bool TupleSet::AddDocument (std::size_t token_0, std::size_t token_1, std::size_t token_2, int document)
{
	bool has_doc = false;
	std::vector<int> & fvector = _tuple_map[token_0][token_1][token_2];
	// check if document is already in the trigram
	for (int i = 0, n = fvector.size(); i < n; ++i)
	{
		if (fvector[i] == document)
		{
			has_doc = true;
			break;
		}
	}

	if (!has_doc) // didn't have document, so add it
	{
		fvector.push_back (document);
		return true;  // indicate that document added
	}
	return false;
}

bool TupleSet::IsMatchingTuple (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2)
{
	std::vector<int> fvector = GetDocumentsForTuple (t0, t1, t2);
	bool has_doc1 = false;
	bool has_doc2 = false;
	for (int i=0, n=fvector.size(); i<n; ++i)
	{
		if (fvector[i] == doc1) has_doc1 = true;
		if (fvector[i] == doc2) has_doc2 = true;
	}
	return ( has_doc1 && has_doc2 );
}

wxSortedArrayString TupleSet::CollectMatchingTuples (int doc1, int doc2, TokenSet & tokenset)
{
	wxSortedArrayString tuples;
	for (Begin (); HasMore (); GetNext ())
	{
		if (IsMatchingTuple (GetToken (0), GetToken (1), GetToken (2), 
					doc1, doc2))
		{
			tuples.Add (GetStringForCurrentTuple (tokenset));
		}
	}
	return tuples; // note: wx library provides copy-on-write semantics
}

bool TupleSet::IsUncommonTuple (TokenSet & token_set) 
{
	return  token_set.IsUncommonWord (GetToken (0)) && 
		token_set.IsUncommonWord (GetToken (1)) &&
		token_set.IsUncommonWord (GetToken (2));
}

wxArrayString TupleSet::GetUncommonTuples (TokenSet & token_set)
{
	wxArrayString tuples;
	for (Begin (); HasMore (); GetNext ())
	{
		if (IsUncommonTuple (token_set))
		{
			// retrieve information for this trigram
			tuples.Add (GetStringForCurrentTuple (token_set));
		}
	}
	return tuples; // note: wx library uses copy-on-write semantics
}

void TupleSet::Begin ()
{
	_ti = _tuple_map.begin ();
	_pi = (_ti->second).begin ();
	_wi = (_pi->second).begin ();
}

void TupleSet::GetNext ()
{
	_wi++; // move to next word position
	if (_wi == (_pi->second).end ())  // if words have finished, then move to next pair position
	{
		_pi++;
		if (_pi == (_ti->second).end ()) // if pairs have finished, then move to next triple position
		{
			_ti++;
			if (_ti == _tuple_map.end ()) return; // finished
			_pi = (_ti->second).begin ();  // get next pair iterator
		}
		_wi = (_pi->second).begin ();  // get next word iterator
	}
}

bool TupleSet::HasMore () const
{
	return _ti != _tuple_map.end ();
}

std::vector<int> & TupleSet::GetDocumentsForCurrentTuple ()
{
	return GetDocumentsForTuple (_ti->first, _pi->first, _wi->first);
}

wxString TupleSet::GetStringForCurrentTuple (TokenSet & tokenset) const
{
	wxString tuple = wxT("");
	tuple += tokenset.GetStringFor (_ti->first);
	tuple += wxT(" ") + tokenset.GetStringFor (_pi->first);
	tuple += wxT(" ") + tokenset.GetStringFor (_wi->first);
	
	return tuple;

}

std::size_t TupleSet::GetToken (int i) const
{
	assert (i>=0 && i<=2);
	if (i == 0)
		return _ti->first;
	else if (i == 1)
		return _pi->first;
	else // if (i == 2)
		return _wi->first;
}

void TupleSet::Save (wxFile & file)
{
	for (Begin (); HasMore (); GetNext ())
	{
		const std::vector<int> indices = GetDocumentsForCurrentTuple ();
		file.Write (wxString::Format (wxT("%d %d %d"), 
					GetToken (0), GetToken (1), GetToken (2)));
		file.Write (wxT(" FILES:[ "));
		for (int i = 0, n = indices.size (); i < n; ++i)
		{
			file.Write (wxString::Format (wxT("%d "), indices[i]));
		}
		file.Write (wxT("] \n"));
	}
}
