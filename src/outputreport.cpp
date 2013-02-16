#include "outputreport.h"

OutputReport::OutputReport (DocumentList & doclist, bool unique, bool ignore)
	: _doclist (doclist), _unique (unique), _ignore (ignore)
{
}

// provide default, empty definitions for virtual functions
void OutputReport::ProcessTrigram (wxString trigram, int string, int end) {}
void OutputReport::WriteDocumentFooter () {}
void OutputReport::EndBlock () {}
void OutputReport::StartCopiedBlock (bool is_unique, bool is_template) {}
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
  bool insidespecialblock = false;

  bool was_unique = false;
  bool is_unique = false;
  bool was_template = false;
  bool is_template = false;

	while (document1->ReadTrigram (tokenset))
  { 
    was_unique = is_unique;
    is_unique = _doclist.IsMatchingTrigram (
        document1->GetToken (0),
        document1->GetToken (1),
        document1->GetToken (2),
        doc1,
        doc2,
        true,
        false
        );
    was_template = is_template;
    is_template = _doclist.IsTemplateTrigram (
        document1->GetToken (0),
        document1->GetToken (1),
        document1->GetToken (2)
        );

    // test if trigram is a match across the documents
    // -- flag _unique used to restrict display to unique matches
    if (_doclist.IsMatchingTrigram (
          document1->GetToken (0),
          document1->GetToken (1),
          document1->GetToken (2),
          doc1,
          doc2,
          _unique,
          _ignore
          ))
    { // processing a matching trigram
      if (!insideblock)
      {
        if (lastwritten > 0) EndBlock ();
        // write any unwritten text up to start of this block
        WriteString (txt.Mid (lastwritten, document1->GetTrigramStart()-lastwritten));
        lastwritten = document1->GetTrigramStart ();
        // start style depends on uniqueness/template of trigram 
        StartCopiedBlock (is_unique, is_template); 
        insideblock = true;
        insidespecialblock = is_unique || is_template;
      }
      else // inside a block already
      {
        // check if change from 'shared' to 'special' copying
        // -- have we moved to a unique/template copied block?
        // -- or have we moved to a shared block from a unique/template?
        // -- or we no longer have a template when we did before (and must be in a specialblock)
        // -- or we no longer have unique text when we did before (and must be in a specialblock)
        if ((!insidespecialblock && (is_unique || is_template)) ||
            (insidespecialblock && !(is_unique || is_template)) ||
            (!is_template && was_template) ||
            (!is_unique && was_unique)
           )
        {
          EndBlock ();
          insidespecialblock = is_unique || is_template;
          StartCopiedBlock (is_unique, is_template);
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
      // check if change from 'shared' to 'unique/template' copying
      // -- have we moved to a unique copied block or template?
      // -- or have we moved to a shared block?
      // -- or we have a template when we did not before
      // -- or we have unique text when we did not before
      // Then EndBlock and restart copied block
      if ( (!insidespecialblock && (is_unique || is_template)) ||
           (insidespecialblock && !(is_unique || is_template)) ||
           (is_template && !was_template) ||
           (is_unique && !was_unique)
         )
      {
        EndBlock ();
        insidespecialblock = is_unique || is_template;
        StartCopiedBlock (is_unique, is_template);
      }

      if (lastwritten < document1->GetTrigramStart (1))
      {
        if (insideblock || insidespecialblock || (lastwritten == 0)) // moving from inside block to not
        {
          if (lastwritten > 0) EndBlock ();
          insideblock = false;
          insidespecialblock = false;
          StartNormalBlock ();
        }
        WriteString (txt.Mid (lastwritten, document1->GetTrigramStart(1)-lastwritten));
        lastwritten = document1->GetTrigramStart (1);
      }
    }
  }
	if (lastwritten < txt.length ())
	{
		if (insideblock || insidespecialblock)
		{
			EndBlock ();
			insideblock = false;
      insidespecialblock = false;
			StartNormalBlock ();
		}
		WriteString (txt.Mid (lastwritten, txt.length () - lastwritten));
	}
	EndBlock ();
	WriteDocumentFooter ();
	document1->CloseInput ();
}

