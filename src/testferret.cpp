// simple test scaffold for Ferret
// written by Peter Lane, March 2006

#include <iostream>
#include <sstream>
#include "coreferret.h"

#include <wx/sstream.h>

void test (bool result, wxString msg)
{
	if (result)
		std::cout << ".";
	else
		std::cout << std::endl << msg << std::endl;
}

void testEmptyStreamFn (wxString str, wxString msg)
{
	wxStringInputStream in (str);
	WordReader reader (in);
	bool result = reader.ReadToken();
	// result should be false if no token read
	test (!result, msg);
}

void testEmptyStream ()
{
	testEmptyStreamFn (wxT(""), wxT("Empty string"));
	testEmptyStreamFn (wxT("  "), wxT("Empty string: spaces"));
	testEmptyStreamFn (wxT("\n\r\n"), wxT("Empty string: newlines"));
	testEmptyStreamFn (wxT("\t\t"), wxT("Empty string: tabs"));
	testEmptyStreamFn (wxT("\t   \t\n  \r \t"), wxT("Empty string, mixture"));
	testEmptyStreamFn (wxT(",.&^%*(){}[];:<>'\\|"), wxT("Empty string, punctuation"));
}

void testSingleTokenFn (wxString str, wxString expected, wxString msg)
{
	TokenSet tokenset;
	tokenset.GetIndexFor (expected); // put string into token set

	wxStringInputStream in (str);
	WordReader reader (in);
	bool result = reader.ReadToken();
	// result should be true if one token read
	test (result, msg + wxT(": result"));
	// GetToken should retrieve the expected token
	test (reader.GetToken(&tokenset) == tokenset.GetIndexFor (expected), 
			msg + wxT(": token"));
	// and should be able to retrieve its own name
	test (tokenset.GetStringFor (tokenset.GetIndexFor (expected)) == expected,
			msg + wxT(": token name"));
}

void testSingleToken ()
{
	testSingleTokenFn (wxT("a"), wxT("a"), wxT("Single letter"));
	testSingleTokenFn (wxT("abcdef"), wxT("abcdef"), wxT("Multiple letters"));
	testSingleTokenFn (wxT("AbCdEf"), wxT("abcdef"), wxT("Multiple, mixed case"));
	testSingleTokenFn (wxT("  AbC "), wxT("abc"), wxT("With space"));
	testSingleTokenFn (wxT(" \n\t AbC\nd"), wxT("abc"), wxT("Other characters"));
}

void testMultipleToken ()
{
	TokenSet tokenset;
	std::size_t token1 = tokenset.GetIndexFor (wxT("abc"));
	std::size_t token2 = tokenset.GetIndexFor (wxT("def"));
	std::size_t token3 = tokenset.GetIndexFor (wxT("xyz"));
	wxStringInputStream in (wxT("AbC,\ndef\t\t\n  xyz"));
	WordReader reader (in);
	test (!reader.IsFinished (), wxT("Is finished - 1"));
	test (reader.ReadToken (), wxT("Read one token"));
	test (reader.GetTokenStart () == 0, wxT("First token start"));
	test (reader.GetTokenEnd () == 3, wxT("First token end"));
	test (!reader.IsFinished (), wxT("Is finished - 2"));
	test (reader.GetToken (&tokenset) == token1, wxT("First token"));
	test (reader.ReadToken (), wxT("Read second token"));
	test (reader.GetTokenStart () == 5, wxT("Second token start"));
	test (reader.GetTokenEnd () == 8, wxT("Second token end"));
	test (!reader.IsFinished (), wxT("Is finished - 3"));
	test (reader.GetToken (&tokenset) == token2, wxT("Second token"));
	test (reader.ReadToken (), wxT("Read third token"));
	test (reader.GetTokenStart () == 13, wxT("Third token start"));
	test (reader.GetTokenEnd () == 16, wxT("Third token end"));
	test (reader.IsFinished (), wxT("Is finished - 4"));
	test (reader.GetToken (&tokenset) == token3, wxT("Third token"));
	test (!reader.ReadToken (), wxT("Finished input"));
	test (reader.IsFinished (), wxT("Is finished"));
	// check names stored correctly
	test (tokenset.GetStringFor (token1) == wxT("abc"), wxT("Name 1"));
	test (tokenset.GetStringFor (token2) == wxT("def"), wxT("Name 2"));
	test (tokenset.GetStringFor (token3) == wxT("xyz"), wxT("Name 3"));
}

void createDocumentList (DocumentList * doclist)
{
	// create some test documents
	std::ofstream file1 ("testferret-test-file-1");
	file1 << "one two three four" << std::endl;
	std::ofstream file2 ("testferret-test-file-2");
	file2 << "two three four five six" << std::endl;

	// create the document list
	doclist->AddDocument (new Document (wxT("testferret-test-file-1")));
	doclist->AddDocument (new Document (wxT("testferret-test-file-2")));
}

void deleteDocumentList (DocumentList * doclist)
{
}

void testTupleMap ()
{
	DocumentList * doclist = new DocumentList ();
	createDocumentList (doclist);
	doclist->RunFerret ();

	// each document should have the right number of triples
	test (doclist->CountTrigrams (0) == 2, wxT("Number of independent triples in doc 0"));
	test (doclist->CountTrigrams (1) == 3, wxT("Number of independent triples in doc 1"));
	// there is only one matching triple in these two documents
	test (doclist->CountMatches (0, 1) == 1, wxT("Number of matching triples in 0 and 1"));
}

int main (int argc, char * argv [])
{
	std::cout << "Testing Ferret: ";
	testEmptyStream ();
	testSingleToken ();
	testMultipleToken ();
	testTupleMap ();

	std::cout << std::endl << "Done." << std::endl;
}
