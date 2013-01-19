#include "xmlreport.h"

// Constructor just passes doclist to its parent class
XmlReport::XmlReport (DocumentList & doclist, bool unique)
	: OutputReport (doclist, unique)
{
}

// Write an xml report for given two documents to given path
void XmlReport::WriteXmlReport (wxString save_report_path, int doc1, int doc2)
{
	if (_file.Open(save_report_path, wxFile::write))
	{
		_file.Write ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		_file.Write ("<?xml-stylesheet type=\"text/xsl\" href=\"uhferret.xsl\" ?>\n");
		_file.Write ("<uhferret>\n");
		// -- write document comparison summary
		_file.Write (wxString::Format ("<common-trigrams>%d</common-trigrams>\n",
					_doclist.CountMatches (doc1, doc2, _unique)));
		_file.Write (wxString::Format ("<similarity>%f</similarity>\n",
					_doclist.ComputeResemblance (doc1, doc2, _unique)));
    if (_unique)
    {
      _file.Write ("(Pairwise similarity ignores trigrams in common with other documents)");
    }
		// -- write out document 1
		WriteDocumentHeader (doc1, doc2);
		WriteDocument (doc1, doc2);
		// -- write out document 2
		WriteDocumentHeader (doc2, doc1);
		WriteDocument (doc2, doc1);

		_file.Write("</uhferret>\n");

		_file.Close ();
	}
}

// output header information for source document
void XmlReport::WriteDocumentHeader (int source_doc, int target_doc)
{
	_file.Write ("<document>\n");
	_file.Write ("<source>" + 
			_doclist[source_doc]->GetOriginalPathname() + 
			"</source>\n");
	_file.Write (wxString::Format ("<num-trigrams>%d</num-trigrams>\n",
			_doclist.CountTrigrams (source_doc)));
	_file.Write (wxString::Format ("<containment>%f</containment>\n",
				_doclist.ComputeContainment (source_doc, target_doc)));
	_file.Write ("<text>\n");

}

// write trailer information, to complete document
void XmlReport::WriteDocumentFooter ()
{
	_file.Write ("</text>\n");
	_file.Write ("</document>\n");
}

void XmlReport::EndBlock ()
{
	_file.Write ("]]></block>");
}

void XmlReport::StartCopiedBlock ()
{
	_file.Write ("<block text=\"copied\"><![CDATA[");
}

void XmlReport::StartNormalBlock ()
{
	_file.Write ("<block text=\"normal\"><![CDATA[");
}

void XmlReport::WriteString (wxString str)
{
	_file.Write (str);
}

