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

bool TupleSet::AddDocument (std::size_t token_0, std::size_t token_1, std::size_t token_2, int document, bool is_template)
{
	bool has_doc = false;
  TupleDocs & tuple_docs = _tuple_map[token_0][token_1][token_2];
  if (is_template) 
  {
    tuple_docs.is_template_material = true;
  }
	std::vector<int> & fvector = tuple_docs.docs;
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

bool TupleSet::IsMatchingTuple (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2, bool unique, bool ignore)
{
  TupleDocs & tuple_docs = _tuple_map[t0][t1][t2];
  if (ignore && tuple_docs.is_template_material) return false;

	std::vector<int> & fvector = tuple_docs.docs;
  if (unique && fvector.size () != 2) return false;

	bool has_doc1 = false;
	bool has_doc2 = false;
	for (int i=0, n=fvector.size(); i<n; ++i)
	{
		if (fvector[i] == doc1) has_doc1 = true;
		if (fvector[i] == doc2) has_doc2 = true;
	}
	return ( has_doc1 && has_doc2 );
}

bool TupleSet::IsTemplateTuple (std::size_t t0, std::size_t t1, std::size_t t2) 
{
  TupleDocs & tuple_docs = _tuple_map[t0][t1][t2];
  return tuple_docs.is_template_material;
}

wxSortedArrayString TupleSet::CollectMatchingTuples (int doc1, int doc2, TokenSet & tokenset, bool unique, bool ignore)
{
	wxSortedArrayString tuples;
	for (Begin (); HasMore (); GetNext ())
	{
		if (IsMatchingTuple (GetToken (0), GetToken (1), GetToken (2), 
					doc1, doc2, unique, ignore))
		{
			tuples.Add (GetStringForCurrentTuple (tokenset));
		}
	}
	return tuples; // note: wx library provides copy-on-write semantics
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
	return _tuple_map[_ti->first][_pi->first][_wi->first].docs;
}

wxString TupleSet::GetStringForCurrentTuple (TokenSet & tokenset) const
{
	wxString tuple = "";
	tuple += tokenset.GetStringFor (_ti->first);
	tuple += " " + tokenset.GetStringFor (_pi->first);
	tuple += " " + tokenset.GetStringFor (_wi->first);
	
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
		file.Write (wxString::Format ("%d %d %d", 
					GetToken (0), GetToken (1), GetToken (2)));
		file.Write (" FILES:[ ");
		for (int i = 0, n = indices.size (); i < n; ++i)
		{
			file.Write (wxString::Format ("%d ", indices[i]));
		}
		file.Write ("] \n");
	}
}
