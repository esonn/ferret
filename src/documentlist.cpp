#include "documentlist.h"

DocumentList::~DocumentList ()
{
	Clear ();
}

bool DocumentList::IsGrouped () const
{
  return (_group_names.size () > 0);
}

// pathname may be a single file or a directory name
// -- recurse through directories, and add all files to the list
// -- if grouped is set, give all files in directory same id
// -- if id0 is set, give all files id=0
void DocumentList::AddDocument (wxString pathname, bool grouped, bool id0)
{
  if (id0) _has_template_material = true;

  if (wxFileName::DirExists (pathname))
  {
    wxFileName filename (pathname);
    wxString short_name = filename.GetName ();

    wxArrayString files;
    wxDir::GetAllFiles (pathname, &files, wxEmptyString);

    int id = -1;
    for (int i=0; i < files.GetCount (); i += 1)
    {
      if (id0)
      { // set id to 0
        id = 0;
      }
      else
      {
        // give document a new id if this is the first document (id == -1)
        // or if grouped is not set
        if (!id0 && ((id == -1) || (!grouped)))
        {
          id = GetNewGroupId ();
        }
      }
      // store id->short_name in _group_names
      if (grouped)
      {
        _group_names[id] = short_name;
      }
      Document * newDocument = new Document (files[i], id);
      newDocument->SetShortPath (short_name); // use the last directory name
      _documents.push_back (newDocument);
    }
  }
  else 
  {
    wxFileName filename (pathname);
    if (filename.IsFileReadable ())
    {
  	  _documents.push_back (new Document (pathname, (id0 ? 0 : GetNewGroupId ())));
    }
  }
}

// Return the number of items in the list of groups,
// or simply the number of documents if no groups created.
int DocumentList::GroupSize () const
{
  if (IsGrouped ())
  {
    return _group_names.size ();
  }
  else
  {
    return _documents.size ();
  }
}

// if we are grouping the documents then given i is not a template
// if we are not grouping the documents, then i is template if its id == 0
bool DocumentList::IsTemplateMaterial (int i) const
{
  if (IsGrouped ())
  {
    return false;
  }
  else
  {
    return (_documents[i]->GetGroupId () == 0);
  }
}

wxString DocumentList::GetGroupName (int index)
{
  if (IsGrouped ())
  {
    return _group_names[index+1]; // index == 0 special role
  }
  else
  {
    return _documents[index]->GetName ();
  }
}

// add document with given id and given name to list
// -- ignores any pathnames which are not readable files
// -- this method is used during file conversion, to keep the file's earlier name
void DocumentList::AddDocument (wxString pathname, wxString name, int id)
{
  wxFileName filename (pathname);
  if (filename.IsFileReadable ())
  {
  	Document * doc = new Document (pathname, id);
  	doc->SetName (name);
  	_documents.push_back (doc);
  }
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

		if (line.IsSameAs ("START GROUP", false)) // -- ignore case on comparison
		{
			within_group = true;
			current_id = GetNewGroupId (); // get a new id for this group
		}
		else if (line.IsSameAs ("END GROUP", false))
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
		_matches.push_back (new MatchPair());
	}
}

void DocumentList::ComputeSimilarities ()
{
	ClearSimilarities ();
	for (_tuple_set.Begin (); _tuple_set.HasMore (); _tuple_set.GetNext ())
	{
		const std::vector<int> & fvector = _tuple_set.GetDocumentsForCurrentTuple ();
    // if fvector is only size 1, then that tuple is unique to the document
    // so keep track of the number of unique tuples
    if (fvector.size () == 1) 
    {
      _documents[fvector[0]]->IncrementUniqueTrigramCount ();
    }

		// take each pair of documents in the vector, and add one to matches
		for (unsigned int fi = 0, n = fvector.size (); fi < n; ++fi)
		{
			for (unsigned int fj=fi+1; fj < n; ++fj)
			{
				// ensure that first index is smaller than the second
				int doc1 = fvector[(fi <= fj ? fi : fj)];
				int doc2 = fvector[(fi <= fj ? fj : fi)];
				assert (doc1 * _documents.size() + doc2 < _matches.size());
				_matches[doc1 * _documents.size() + doc2]->common += 1;
        if (fvector.size () == 2) 
        {
          _matches[doc1 * _documents.size() + doc2]->unique += 1;
        }
			}
		}

	}
}

int DocumentList::GetTotalTrigramCount ()
{
	return _tuple_set.Size ();
}

// return a count of the trigrams in document i
int DocumentList::CountTrigrams (int doc_i) 
{
 return _documents[doc_i]->GetTrigramCount ();
}

int DocumentList::CountMatches (int doc_i, int doc_j, bool unique)
{
	assert (doc_j > doc_i); // _matches is only completed from one side, with doc_j > doc_i
	assert ((doc_i * _documents.size() + doc_j) < _matches.size());
  if (unique)
  {
    return _matches[doc_i * _documents.size() + doc_j]->unique;
  }
  else
  {
	  return _matches[doc_i * _documents.size() + doc_j]->common;
  }
}

float DocumentList::ComputeResemblance (int doc_i, int doc_j, bool unique)
{
	float num_matches = (float)CountMatches (doc_i, doc_j, unique);
	float total_trigrams = (float)(CountTrigrams (doc_i) + CountTrigrams (doc_j) - num_matches);
	if (total_trigrams == 0.0) return 0.0; // check for divide by zero
	return num_matches/total_trigrams;
}

float DocumentList::ComputeContainment (int doc_i, int doc_j, bool unique)
{
	float num_matches = (float)(doc_j > doc_i ? CountMatches (doc_i, doc_j, unique) : CountMatches (doc_j, doc_i, unique));
	float target_trigrams = (float)(CountTrigrams (doc_j));
	if (target_trigrams == 0.0) return 0.0; // check for divide by zero
	return num_matches/target_trigrams;
}

// unique count adds up all counts for files in given group index,
// or returns document's unique count if no groups used.
int DocumentList::UniqueCount (int index) const
{
  if (IsGrouped ())
  {
    int total = 0;
    for (int i = 0; i < _documents.size (); i++)
    {
      if (_documents[i]->GetGroupId () == index+1)
      {
        total += _documents[i]->GetUniqueTrigramCount ();
      }
    }
    return total;
  }
  else
  {
    return _documents[index]->GetUniqueTrigramCount ();
  }
}

bool DocumentList::IsMatchingTrigram (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2, bool unique)
{
	return _tuple_set.IsMatchingTuple (t0, t1, t2, doc1, doc2, unique);
}

wxString DocumentList::MakeTrigramString (std::size_t t0, std::size_t t1, std::size_t t2)
{
	wxString tuple = "";
	tuple += _token_set.GetStringFor (t0);
	tuple += " " + _token_set.GetStringFor (t1);
	tuple += " " + _token_set.GetStringFor (t2);
	
	return tuple;
}

wxSortedArrayString DocumentList::CollectMatchingTrigrams (int doc1, int doc2, bool unique) 
{
	return _tuple_set.CollectMatchingTuples (doc1, doc2, _token_set, unique);
}

// make names of comparison structure visible
DocumentList * DocumentList::uniquecountcmp::doclist;
struct DocumentList::uniquecountcmp DocumentList::GetUniqueCountComparer ()
{
  uniquecountcmp comparer;
  comparer.doclist = this;

  return comparer;
}

// make names of comparison structure visible
DocumentList * DocumentList::similaritycmp::doclist;
std::vector<int> * DocumentList::similaritycmp::document1;
std::vector<int> * DocumentList::similaritycmp::document2;

struct DocumentList::similaritycmp DocumentList::GetSimilarityComparer (std::vector<int> * document1, std::vector<int> * document2, bool unique)
{
	similaritycmp comparer (unique);
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
		file.Write (wxString::Format ("next-group-id\t%d\n", _last_group_id));
		file.Write ("begin-documents\n");
		for (unsigned int i = 0, n = _documents.size (); i < n; ++i)
		{
			_documents[i]->Save (file);
		}
		file.Write ("end-documents\n");

		file.Write ("begin-tokens\n");
		_token_set.Save (file);
		file.Write ("end-tokens\n");

		file.Write ("begin-tuples\n");
		_tuple_set.Save (file);
		file.Write ("end-tuples\n");


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

bool DocumentList::HasTemplateMaterial () const
{
  return _has_template_material;
}

// -- currently assumes file is correct, ignoring EOF
bool DocumentList::ReadDocumentDefinitions (wxTextInputStream & stored_data)
{
	wxString line = stored_data.ReadLine ();
	wxString data;

	if (!line.StartsWith ("next-group-id\t", & data)) return false;
	_last_group_id = wxAtoi (data);

	line = stored_data.ReadLine ();
	if (!line.IsSameAs ("begin-documents")) return false; // did not find start of group
	line = stored_data.ReadLine ();
	while (!line.IsSameAs ("end-documents")) // repeat until hit end of group
	{
		if (line.IsSameAs ("start-document"))
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
	while (!line.IsSameAs ("end-document"))
	{
		wxString data;
		if (line.StartsWith ("path\t", & data))
		{
			pathname = data;
		}
		else if (line.StartsWith ("original-path\t", & data))
		{
			original_pathname = data;
		}
		else if (line.StartsWith ("name\t", & data))
		{
			name = data;
		}
		else if (line.StartsWith ("num-trigrams\t", & data))
		{
			num_trigrams = wxAtoi (data);
		}
		else if (line.StartsWith ("group-id\t", & data))
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
	
	if (!line.StartsWith ("next-index\t", & index)) return false;
	_token_set.SetNextIndex (wxAtoi (index));

	line = stored_data.ReadLine ();
	while (!line.IsSameAs ("end-tokens"))
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

	while (!line.IsSameAs ("end-tuples"))
	{
		wxStringTokenizer items (line, " ");
		if (items.CountTokens () < 5) return false; // not enough tokens!
		int index0 = wxAtoi (items.GetNextToken ());
		int index1 = wxAtoi (items.GetNextToken ());
		int index2 = wxAtoi (items.GetNextToken ());
		if (!items.GetNextToken().IsSameAs ("FILES:[")) return false; // error!
		while (items.HasMoreTokens ())
		{
			wxString next = items.GetNextToken ();
			if (next.IsSameAs ("]")) break; // finish loop
			_tuple_set.AddDocument (index0, index1, index2, wxAtoi (next));
		}

		line = stored_data.ReadLine ();
	}
	return true;
}

