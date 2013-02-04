#if !defined documentlist_h
#define documentlist_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <assert.h>
#include <map>
#include <vector>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>

#include "tokenset.h"
#include "tupleset.h"
#include "document.h"

/** Pair used in matches - 
 * keeps a count of number of matches where only A & B are present, 
 * and of all trigrams that A & B occur in.
 */
class MatchPair
{
  public:
    int common;
    int unique;
};

/** DocumentList maintains a list of documents, a TokenSet of identified Tokens and 
  *    a TupleSet, which maps from sequences of three tokens to lists of documents 
  *    in which the trigrams were found.  
  * -- Methods are provided to calculate information about pairs of documents,
  *    such as Resemblance and Containment.
  * -- Note that the Documents are owned by this class although not created by it,
  *    and hence all Documents are destroyed with the DocumentList.
  */
class DocumentList
{
	struct similaritycmp { // structure for sorting indices to document by similarity
    similaritycmp (bool unique): _unique (unique) {}
		static DocumentList * doclist;
		static std::vector<int> * document1;
		static std::vector<int> * document2;
    bool _unique;

		bool operator()(int x, int y) const 
		{
			return (doclist->ComputeResemblance((*document1)[x], (*document2)[x], _unique)
				>
				doclist->ComputeResemblance((*document1)[y], (*document2)[y], _unique));
		}
	};
  struct uniquecountcmp { // structure for sorting indices to document by unique count
		static DocumentList * doclist;

		bool operator()(int x, int y) const 
		{
			return (doclist->UniqueCount(x)
				>
				doclist->UniqueCount(y));
		}
	};
	public:
		DocumentList () : _last_group_id (0) {}
		~DocumentList ();
		void AddDocument (wxString pathname, bool grouped=false);
		void AddDocument (wxString pathname, int id);
		void AddDocument (wxString pathname, wxString name, int id);
		bool AddDocumentsFromDefinitionFile (wxString pathname);
		Document * operator [] (std::size_t i) const;
		void RemoveDocument (Document * doc);
		TokenSet & GetTokenSet ();
		TupleSet & GetTupleSet ();
		void Clear ();
		int GetNewGroupId ();
		void ResetReading ();
		int Size () const;
		int NumberOfPairs () const;
		bool MayNeedConversions () const;
		void RunFerret (int first_document = 0);
		void ReadDocument (int i);
		void ClearSimilarities ();
		void ComputeSimilarities ();
		int GetTotalTrigramCount ();
		int CountTrigrams (int doc_i, bool unique=false);
		int CountMatches (int doc_i, int doc_j, bool unique=false);
		float ComputeResemblance (int doc_i, int doc_j, bool unique=false);
		float ComputeContainment (int doc_i, int doc_j, bool unique=false);
    int UniqueCount (int doc);
		// check if given trigram is in both the indexed documents
		bool IsMatchingTrigram (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2, bool unique=false);
		// convert given trigram into a string
		wxString MakeTrigramString (std::size_t t0, std::size_t t1, std::size_t t2);
		// collect all the matching trigrams in the two documents into a vector of strings
		wxSortedArrayString CollectMatchingTrigrams (int doc1, int doc2, bool unique=false);
		// for sorting pairs of indices
    struct uniquecountcmp GetUniqueCountComparer ();
		struct similaritycmp GetSimilarityComparer (std::vector<int> * document1, std::vector<int> * document2, bool unique);
		// for storing/retrieving list of documents and token/tuple definitions
		void SaveDocumentList (wxString path);
		bool RetrieveDocumentList (wxString path);
	private:
		bool ReadDocumentDefinitions (wxTextInputStream & stored_data);
		bool ReadSingleDocumentDefinition (wxTextInputStream & stored_data);
		bool ReadTokenDefinitions (wxTextInputStream & stored_data);
		bool ReadTupleDefinitions (wxTextInputStream & stored_data);
	private:
		std::vector<Document *>	_documents;
    std::map<int, wxString> _group_names;
		TokenSet		_token_set;
		TupleSet		_tuple_set;
		std::vector<MatchPair *>	_matches;
		int			    _last_group_id;
};

#endif

