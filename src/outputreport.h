#if !defined outputreport_h
#define outputreport_h

#include <wx/wx.h>
#include <wx/sstream.h>

#include "documentlist.h"
#include "tokenset.h"

/** This class provides a 'visitor' pattern, for different kinds of document display.
 *  Child classes should implement the virtual methods, 
 *  so that WriteDocument can be used to walk through a document, highlighting common parts.
 */

class OutputReport
{
	public:
		OutputReport (DocumentList & doclist, bool unique, bool ignore);
		virtual void ProcessTrigram (wxString trigram, int start, int end);
		virtual void WriteDocumentFooter ();
		virtual void EndBlock ();
		virtual void StartCopiedBlock (bool unique);
		virtual void StartNormalBlock ();
		virtual void WriteString (wxString str);
		void WriteDocument (int doc1, int doc2);
	protected:
		DocumentList & _doclist;
    bool           _unique;
    bool           _ignore;
  friend class PdfReport;
  friend class XmlReport;
};

#endif

