#include "documentlist.h"

DocumentList::~DocumentList ()
{
	Clear ();
}

void DocumentList::AddDocument (wxString pathname)
{
	_documents.push_back (new Document (pathname, GetNewGroupId ()));
}

void DocumentList::AddDocument (wxString pathname, int id)
{
	_documents.push_back (new Document (pathname, id));
}

void DocumentList::AddDocument (wxString pathname, wxString name, int id)
{
	Document * doc = new Document (pathname, id);
	doc->SetName (name);
	_documents.push_back (doc);
}

// Use file defined in pathname as a list of definitions
// -- returns true if managed to open and process definition file correctly
bool DocumentList::AddDocumentsFromDefinitionFile (wxString pathname)
{
	assert (wxFile::Exists (pathname));
	wxFileInputStream file (pathname);
	wxTextInputStream file_definition (file);
	bool within_group = false;
	int current_id = 0;

	while (!file.Eof ()) 
	{
		wxString line = file_definition.ReadLine ();
		line.Trim ();       // remove white space from right
		line.Trim (false);  // and from left

		if (line.IsSameAs (wxT("START GROUP"), false)) // -- ignore case on comparison
		{
			within_group = true;
			current_id = GetNewGroupId (); // get a new id for this group
		}
		else if (line.IsSameAs (wxT("END GROUP"), false))
		{
			within_group = false;
		}
		else if (wxFile::Exists (line)) // not a special command line, so just add document, if it's a file
		{
			if (within_group)
			{
				AddDocument (line, current_id);
			}
			else
			{
				AddDocument (line);
			}
		}
	}

	return true;
}

Document * DocumentList::operator [] (std::size_t i) const
{
	assert (i >= 0 && i < _documents.size ());
	return _documents[i];
}

void DocumentList::RemoveDocument (Document * doc)
{
	for (std::vector<Document *>::iterator it = _documents.begin();
		it != _documents.end();
		++it)
	{
		if (*it == doc)
		{
			_documents.erase (it);
			return;
		}
	}
}

TokenSet & DocumentList::GetTokenSet ()
{
	return _token_set;
}

TupleSet & DocumentList::GetTupleSet ()
{
	return _tuple_set;
}

// A Document list owns the documents, so is responsible for deleting them
void DocumentList::Clear ()
{
	for (int i=0, n=_documents.size(); i<n; ++i)
	{
		delete _documents[i];
	}
	_documents.clear ();
	ResetReading ();
}

// return a new, unique group id.  
int DocumentList::GetNewGroupId ()
{
	_last_group_id += 1;
	return _last_group_id;
}

void DocumentList::ResetReading ()
{
	_token_set.Clear ();
	_tuple_set.Clear ();
	_matches.clear ();
}

int DocumentList::Size () const
{
	return _documents.size ();
}

// don't count pairs of documents in same group
int DocumentList::NumberOfPairs () const
{
	int num_pairs = 0;
	for (int i = 0; i < _documents.size (); ++i)
		for (int j = i+1; j < _documents.size (); ++j)
		{
			if (_documents[i]->GetGroupId () != _documents[j]->GetGroupId ())
				num_pairs++;
		}
	return num_pairs;
}

bool DocumentList::MayNeedConversions () const
{
	for (int i=0, n = _documents.size(); i < n; ++i)
	{
		if (_documents[i]->IsUnknownType ()) return true;
		if (_documents[i]->IsWordProcessorType ()) return true;
		if (_documents[i]->IsPdfType ()) return true;
	}
	
	return false;
}

void DocumentList::RunFerret (int first_document)
{
	// phase 1 -- read each file in turn, finding trigrams
	for (int i = first_document; i < _documents.size (); ++i)
	{
		ReadDocument (i);
	}

	// phase 2 -- compute the similarities
	ComputeSimilarities ();
}

void DocumentList::ReadDocument (int i)
{
	_documents[i]->StartInput (_token_set);
	_documents[i]->ResetTrigramCount ();
	while ( _documents[i]->ReadTrigram (_token_set) )
	{
		if (_tuple_set.AddDocument (
					_documents[i]->GetToken (0),
					_documents[i]->GetToken (1),
					_documents[i]->GetToken (2),
					i))
		{
			_documents[i]->IncrementTrigramCount ();
		}
	}
	_documents[i]->CloseInput ();
}

void DocumentList::ClearSimilarities ()
{
	for (int i=0; i < _documents.size() * _documents.size(); ++i)
	{
		_matches.push_back (0);
	}
}

void DocumentList::ComputeSimilarities ()
{
	ClearSimilarities ();
	for (_tuple_set.Begin (); _tuple_set.HasMore (); _tuple_set.GetNext ())
	{
		const std::vector<int> & fvector = _tuple_set.GetDocumentsForCurrentTuple ();
		// take each pair of documents in the vector, and add one to matches
		for (unsigned int fi = 0, n = fvector.size (); fi < n; ++fi)
		{
			for (unsigned int fj=fi+1; fj < n; ++fj)
			{
				// ensure that first index is smaller than the second
				int doc1 = fvector[(fi <= fj ? fi : fj)];
				int doc2 = fvector[(fi <= fj ? fj : fi)];
				assert (doc1 * _documents.size() + doc2 < _matches.size());
				_matches[doc1 * _documents.size() + doc2]++;
			}
		}

	}
}

int DocumentList::GetTotalTrigramCount ()
{
	return _tuple_set.Size ();
}

int DocumentList::CountTrigrams (int doc_i) 
{
	return _documents[doc_i]->GetTrigramCount ();
}

int DocumentList::CountMatches (int doc_i, int doc_j)
{
	assert (doc_j > doc_i); // _matches is only completed from one side, with doc_j > doc_i
	assert ((doc_i * _documents.size() + doc_j) < _matches.size());
	return _matches[doc_i * _documents.size() + doc_j];
}

float DocumentList::ComputeResemblance (int doc_i, int doc_j)
{
	float num_matches = (float)CountMatches (doc_i, doc_j);
	float total_trigrams = (float)(CountTrigrams (doc_i) + CountTrigrams (doc_j) - CountMatches (doc_i, doc_j));
	if (total_trigrams == 0.0) return 0.0; // check for divide by zero
	return num_matches/total_trigrams;
}

float DocumentList::ComputeContainment (int doc_i, int doc_j)
{
	float num_matches = (float)(doc_j > doc_i ? CountMatches (doc_i, doc_j) : CountMatches (doc_j, doc_i));
	float target_trigrams = (float)(CountTrigrams (doc_j));
	if (target_trigrams == 0.0) return 0.0; // check for divide by zero
	return num_matches/target_trigrams;
}

bool DocumentList::IsMatchingTrigram (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2)
{
	return _tuple_set.IsMatchingTuple (t0, t1, t2, doc1, doc2);
}

wxString DocumentList::MakeTrigramString (std::size_t t0, std::size_t t1, std::size_t t2)
{
	wxString tuple = wxT("");
	tuple += _token_set.GetStringFor (t0);
	tuple += wxT(" ") + _token_set.GetStringFor (t1);
	tuple += wxT(" ") + _token_set.GetStringFor (t2);
	
	return tuple;
}

wxSortedArrayString DocumentList::CollectMatchingTrigrams (int doc1, int doc2) 
{
	return _tuple_set.CollectMatchingTuples (doc1, doc2, _token_set);
}

// make names of comparison structure visible
DocumentList * DocumentList::similaritycmp::doclist;
std::vector<int> * DocumentList::similaritycmp::document1;
std::vector<int> * DocumentList::similaritycmp::document2;

struct DocumentList::similaritycmp DocumentList::GetSimilarityComparer (std::vector<int> * document1, std::vector<int> * document2)
{
	similaritycmp comparer;
	comparer.doclist = this;
	comparer.document1 = document1;
	comparer.document2 = document2;

	return comparer;
}

void DocumentList::SaveDocumentList (wxString path)
{
	wxFile file;
	if (file.Open (path, wxFile::write))
	{
		file.Write (wxString::Format (wxT("next-group-id\t%d\n"), _last_group_id));
		file.Write (wxT("begin-documents\n"));
		for (unsigned int i = 0, n = _documents.size (); i < n; ++i)
		{
			_documents[i]->Save (file);
		}
		file.Write (wxT("end-documents\n"));

		file.Write (wxT("begin-tokens\n"));
		_token_set.Save (file);
		file.Write (wxT("end-tokens\n"));

		file.Write (wxT("begin-tuples\n"));
		_tuple_set.Save (file);
		file.Write (wxT("end-tuples\n"));


		file.Close ();
	}
}

// Read definition of document list from file.
// -- return false if there is any error
bool DocumentList::RetrieveDocumentList (wxString path) 
{
	if (!(wxFile::Exists (path))) return false;
	
	wxFileInputStream file (path);
	wxTextInputStream stored_data (file);

	if (!ReadDocumentDefinitions (stored_data)) return false;
	if (!ReadTokenDefinitions (stored_data)) return false;
	if (!ReadTupleDefinitions (stored_data)) return false;

	return true;
}

// -- currently assumes file is correct, ignoring EOF
bool DocumentList::ReadDocumentDefinitions (wxTextInputStream & stored_data)
{
	wxString line = stored_data.ReadLine ();
	wxString data;

	if (!line.StartsWith (wxT("next-group-id\t"), & data)) return false;
	_last_group_id = wxAtoi (data);

	line = stored_data.ReadLine ();
	if (!line.IsSameAs (wxT("begin-documents"))) return false; // did not find start of group
	line = stored_data.ReadLine ();
	while (!line.IsSameAs (wxT("end-documents"))) // repeat until hit end of group
	{
		if (line.IsSameAs (wxT("start-document")))
		{
			ReadSingleDocumentDefinition (stored_data);
			line = stored_data.ReadLine ();
		}
		else
		{
			return false; // did not find start of document
		}
	}
	return true;
}

bool DocumentList::ReadSingleDocumentDefinition (wxTextInputStream & stored_data)
{
	wxString pathname;
	wxString name;
	wxString original_pathname;
	int id;
	int num_trigrams;

	wxString line = stored_data.ReadLine ();
	while (!line.IsSameAs (wxT("end-document")))
	{
		wxString data;
		if (line.StartsWith (wxT("path\t"), & data))
		{
			pathname = data;
		}
		else if (line.StartsWith (wxT("original-path\t"), & data))
		{
			original_pathname = data;
		}
		else if (line.StartsWith (wxT("name\t"), & data))
		{
			name = data;
		}
		else if (line.StartsWith (wxT("num-trigrams\t"), & data))
		{
			num_trigrams = wxAtoi (data);
		}
		else if (line.StartsWith (wxT("group-id\t"), & data))
		{
			id = wxAtoi (data);
		}
		else
		{
			return false; // something wrong in file
		}

		line = stored_data.ReadLine ();
	}
	// -- create the document and set all its parameters based on read data
	Document * doc = new Document (pathname, id);
	doc->SetName (name);
	doc->SetOriginalPathname (original_pathname);
	doc->SetTrigramCount (num_trigrams);
	_documents.push_back (doc);
	// -- all ok, so return true
	return true;
}

bool DocumentList::ReadTokenDefinitions (wxTextInputStream & stored_data)
{
	wxString line = stored_data.ReadLine ();
	line = stored_data.ReadLine ();
	wxString index;
	wxString word;
	
	if (!line.StartsWith (wxT("next-index\t"), & index)) return false;
	_token_set.SetNextIndex (wxAtoi (index));

	line = stored_data.ReadLine ();
	while (!line.IsSameAs (wxT("end-tokens")))
	{
		index = line.BeforeFirst ('\t');
		word = line.AfterFirst ('\t');
		_token_set.SetIndexString (word, wxAtoi (index));
		line = stored_data.ReadLine ();
	}
	return true;
}

// -- TODO: Some error checking on wxAtoi
bool DocumentList::ReadTupleDefinitions (wxTextInputStream & stored_data)
{
	wxString line = stored_data.ReadLine ();
	line = stored_data.ReadLine ();

	while (!line.IsSameAs (wxT("end-tuples")))
	{
		wxStringTokenizer items (line, wxT(" "));
		if (items.CountTokens () < 5) return false; // not enough tokens!
		int index0 = wxAtoi (items.GetNextToken ());
		int index1 = wxAtoi (items.GetNextToken ());
		int index2 = wxAtoi (items.GetNextToken ());
		if (!items.GetNextToken().IsSameAs (wxT("FILES:["))) return false; // error!
		while (items.HasMoreTokens ())
		{
			wxString next = items.GetNextToken ();
			if (next.IsSameAs (wxT("]"))) break; // finish loop
			_tuple_set.AddDocument (index0, index1, index2, wxAtoi (next));
		}

		line = stored_data.ReadLine ();
	}
	return true;
}

