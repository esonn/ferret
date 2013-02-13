#include "outputreport.h"

OutputReport::OutputReport (DocumentList & doclist, bool unique, bool ignore)
	: _doclist (doclist), _unique (unique), _ignore (ignore)
{
}

// provide default, empty definitions for virtual functions
void OutputReport::ProcessTrigram (wxString trigram, int string, int end) {}
void OutputReport::WriteDocumentFooter () {}
void OutputReport::EndBlock () {}
void OutputReport::StartCopiedBlock (bool unique) {}
void OutputReport::StartNormalBlock () {}
void OutputReport::WriteString (wxString str) {}

void OutputReport::WriteDocument (int doc1, int doc2)
{
	// -- write internal text from document
	wxFFile f (_doclist[doc1]->GetPathname (), "rb");
  wxString txt;
  f.ReadAll (&txt);
	txt.Replace ("\t", "    "); // replace tabs with 4-spaces, to ensure they show up in all outputs

	// make an input stream for the read document
	TokenSet & tokenset = _doclist.GetTokenSet ();
	wxStringInputStream in (txt);

	Document * document1 = _doclist[doc1];
	document1->StartInput (in, tokenset); // make document read from string of document
	int lastwritten = 0;
	bool insideblock = false;
  bool insideuniqueblock = false;

	while (document1->ReadTrigram (tokenset))
	{ 
		if (_doclist.IsMatchingTrigram (
					document1->GetToken (0),
					document1->GetToken (1),
					document1->GetToken (2),
					doc1,
					doc2,
          _unique
          ))
		{
			if (!insideblock)
			{
				if (lastwritten > 0) EndBlock ();
        // write any unwritten text up to start of this block
        WriteString (txt.Mid (lastwritten, document1->GetTrigramStart()-lastwritten));
        lastwritten = document1->GetTrigramStart ();
        StartCopiedBlock (_doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              )); // start style depends on uniqueness of trigram
				insideblock = true;
        insideuniqueblock = _doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              );
			}
      else // inside a block already
      {
        // check if change from 'shared' to 'unique' copying
        // -- have we moved to a unique copied block?
        if (!insideuniqueblock && _doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              ))
        {
          EndBlock ();
          insideuniqueblock = true;
          StartCopiedBlock (true);
        }
        // -- have we moved to a shared block?
        else if (insideuniqueblock && !_doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              ))
        {
          EndBlock ();
          insideuniqueblock = false;
          StartCopiedBlock (false);
        }
      }
      // write the trigram
			WriteString (txt.Mid (lastwritten, document1->GetTrigramEnd()-lastwritten));
			lastwritten = document1->GetTrigramEnd ();
			ProcessTrigram (
				_doclist.MakeTrigramString(
					document1->GetToken (0),
					document1->GetToken (1),
					document1->GetToken (2)),
				document1->GetTrigramStart (),
				document1->GetTrigramEnd ()
			);
		}
		else // inside a block already
		{
      // check if change from 'shared' to 'unique' copying
      // -- have we moved to a unique copied block?
			if (!insideuniqueblock && _doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              ))
      {
        EndBlock ();
        insideuniqueblock = true;
        StartCopiedBlock (true);
      }
      // -- have we moved to a shared block?
      else if (insideuniqueblock && !_doclist.IsMatchingTrigram (
              document1->GetToken (0),
              document1->GetToken (1),
              document1->GetToken (2),
              doc1,
              doc2,
              true
              ))
      {
        EndBlock ();
        insideuniqueblock = false;
        StartCopiedBlock (false);
      }
      if (lastwritten < document1->GetTrigramStart (1))
			{
				if (insideblock || insideuniqueblock || (lastwritten == 0)) // moving from inside block to not
				{
					if (lastwritten > 0) EndBlock ();
					insideblock = false;
          insideuniqueblock = false;
					StartNormalBlock ();
				}
				WriteString (txt.Mid (lastwritten, document1->GetTrigramStart(1)-lastwritten));
				lastwritten = document1->GetTrigramStart (1);
			}
		}
	}
	if (lastwritten < txt.length ())
	{
		if (insideblock || insideuniqueblock)
		{
			EndBlock ();
			insideblock = false;
      insideuniqueblock = false;
			StartNormalBlock ();
		}
		WriteString (txt.Mid (lastwritten, txt.length () - lastwritten));
	}
	EndBlock ();
	WriteDocumentFooter ();
	document1->CloseInput ();
}

