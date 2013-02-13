#if !defined tupleset_h
#define tupleset_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  * Initial triple map idea by Bob Dickerson
  */

#include <assert.h>
#include <wx/wx.h>
#include <wx/file.h>
#include <map>
#include <vector>

#include "tokenset.h"

/* Class to hold fvector and flag for documents per tuple */
class TupleDocs
{
  public:
    std::vector<int> docs;
    bool is_template_material;
};

/** TupleSet maintains the database mapping trigrams to identifier of documents which contain them.
  * The mapping is held as a sequence of std::maps, each map taking a std::size_t reference to 
  * a token as a key.  The end result of the three maps is a vector of document identifiers.
  *
  * The most important feature of the TupleSet is the collection of methods for iterating over 
  * all tuples in the TupleSet.
  * e.g. with the definition:  TupleSet tuple_set; 
  * use:                       for (tuple_set.Begin (); tuple_set.HasMore (); tuple_set.GetNext ())
  *                            {}
  * to iterate over all the tuples.  The methods: GetDocumentsForCurrentTuple, GetStringForCurrentTuple,
  * and GetToken0, GetToken1, GetToken2 return information on the current tuple.
  */
class TupleSet
{
 
	// typedef's to simplify declarations
	typedef std::map<std::size_t, TupleDocs> WordMap;
	typedef WordMap::const_iterator WordMapIter;

	typedef std::map<std::size_t, WordMap> PairMap;
	typedef PairMap::const_iterator PairMapIter;

	typedef std::map<std::size_t, PairMap> TripMap;
	typedef TripMap::const_iterator TripMapIter;

	public:
		TupleSet ();
		void Clear ();
		int Size ();
		// given a tuple and a document identifier, 
		// - make sure that the document is in the list for that tuple
		// - returns true if the document was not already in trigram's list
		bool AddDocument (std::size_t token_0, std::size_t token_1, std::size_t token_2, 
				int document, bool is_template);
		// check if two documents share the given tuple
		bool IsMatchingTuple (std::size_t t0, std::size_t t1, std::size_t t2, int doc1, int doc2, bool unique = false, bool ignore = false);
		// collect and return all tuples in the two given documents
		wxSortedArrayString CollectMatchingTuples (int doc1, int doc2, TokenSet & tokenset, bool unique = false, bool ignore = false);
	private:
		TripMap	_tuple_map;
	public: // following methods and data structures are to handle an iterator on tupleset
		void Begin ();			// start the iterator
		void GetNext ();		// advance the iterator
		bool HasMore () const;		// check for end
		// retrieve current tuple's documents
		std::vector<int> & GetDocumentsForCurrentTuple ();	
		// retrieve string for current tuple
		wxString GetStringForCurrentTuple (TokenSet & tokenset) const;	
		// retrieve identifiers for individual tokens
		std::size_t GetToken (int i) const;
		// methods to save/retrieve tuples
		void Save (wxFile & file);
	private:
		TripMapIter	_ti;	// iterator from first token to pairs
		PairMapIter	_pi;	// iterator from second token to words
		WordMapIter	_wi;	// iterator from third token to document list
};

#endif

