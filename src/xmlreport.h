#if !defined xmlreport_h
#define xmlreport_h

/** written by Peter Lane, 2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#include <wx/wx.h>
#include <wx/sstream.h>

#include "documentlist.h"
#include "outputreport.h"
#include "tokenset.h"

class XmlReport : public OutputReport
{
	public:
		XmlReport (DocumentList & doclist, bool unique);
		void WriteXmlReport (wxString save_report_path, int doc1, int doc2);
	private:
		void WriteDocumentHeader (int source_doc, int target_doc);
		void WriteDocumentFooter ();
		void EndBlock ();
		void StartCopiedBlock (bool unique);
		void StartNormalBlock ();
		void WriteString (wxString str);
	private: 
		wxFile _file;
};

#endif

