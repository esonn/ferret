#include "mainferret.h"

bool isNamedOption (wxString test_string, wxString shortcut, wxString longform)
{
	return (test_string == shortcut) || (test_string == longform);
}

bool isHelpOption (wxString test_string)
{
	return isNamedOption (test_string, "-h", "--help");
}

bool isDataTableOption (wxString test_string)
{
	return isNamedOption (test_string, "-d", "--data-table");
}

bool isListTrigramsOption (wxString test_string)
{
	return isNamedOption (test_string, "-l", "--list-trigrams");
}

bool isAllComparisonsOption (wxString test_string)
{
	return isNamedOption (test_string, "-a", "--all-comparisons");
}

bool isRemoveCommonTrigramsOption (wxString test_string)
{
	return isNamedOption (test_string, "-r", "--remove-common");
}

bool isPdfOption (wxString test_string)
{
	return isNamedOption (test_string, "-p", "--pdf-report");
}

bool isXmlOption (wxString test_string)
{
	return isNamedOption (test_string, "-x", "--xml-report");
}

bool isDefinitionOption (wxString test_string)
{
	return isNamedOption (test_string, "-f", "--definition-file");
}

bool isStoredDataOption (wxString test_string)
{
	return isNamedOption (test_string, "-u", "--use-stored-data");
}

bool isCommandOption (wxString test_string)
{
	return isHelpOption (test_string) 
		|| isDataTableOption (test_string) 
		|| isListTrigramsOption (test_string) 
		|| isAllComparisonsOption (test_string) 
    || isRemoveCommonTrigramsOption (test_string)
		|| isPdfOption (test_string)
		|| isXmlOption (test_string)
		|| isDefinitionOption (test_string)
		|| isStoredDataOption (test_string);
}

void aboutMessage ()
{
	std::cout 
		<< "Ferret 5.4: start with no arguments for graphical version" << std::endl
		<< "Usage: ferret [-h] [-d] [-l] [-a] [-r] [-p] [-x] [-f] [-u]" << std::endl
		<< "  -h, --help           	displays help on command-line parameters" << std::endl
		<< "  -d, --data-table     	produce similarity table (default)" << std::endl
		<< "  -l, --list-trigrams  	produce trigram list report" << std::endl
		<< "  -a, --all-comparisons	produce list of all comparisons" << std::endl
    << "  -r, --remove-common   removes common trigrams" << std::endl
		//<< "  -p, --pdf-report     	source-1 source-2 results-file : create pdf report" << std::endl
		<< "  -x, --xml-report     	source-1 source-2 results-file : create xml report" << std::endl
		<< "  -f, --definition-file	use file with document list" << std::endl
		<< "  -u, --use-stored-data	store/retrieve data structure" << std::endl;
}

void produceComparisonReport (
		wxString filename1, 
		wxString filename2, 
		wxString target_name,
		Report report_type,
    bool remove_common_trigrams
		)
{
	DocumentList docs;
	if (wxFileName::IsFileReadable (filename1))
		docs.AddDocument (filename1);
	if (wxFileName::IsFileReadable (filename2))
		docs.AddDocument (filename2);

	if (docs.Size () < 2) // check we added at least 2 readable files
	{
		aboutMessage ();
		return;
	}

	docs.RunFerret ();

	if (report_type == PDF_REPORT)
	{
		PdfReport pdfreport (docs, remove_common_trigrams, false);
		pdfreport.WritePdfReport (target_name, 0, 1);
	}
	else // if (report_type == XML_REPORT)
	{
		XmlReport xmlreport (docs, remove_common_trigrams, false);
		xmlreport.WriteXmlReport (target_name, 0, 1);
	}
}

void writeTrigramList (DocumentList & docs)
{
	TupleSet & tuple_set = docs.GetTupleSet ();

	for (tuple_set.Begin (); tuple_set.HasMore (); tuple_set.GetNext ())
	{
		const std::vector<int> docIndices = tuple_set.GetDocumentsForCurrentTuple ();
		// output information for this trigram
		std::cout 
			<< docs.MakeTrigramString (tuple_set.GetToken (0), tuple_set.GetToken (1), tuple_set.GetToken (2))
			<< "           FILES:[ ";
		for (int i = 0, n = docIndices.size (); i < n; ++i)
		{
			std::cout << docIndices[i] << " ";
		}
		std::cout << "]" << std::endl;
	}
}

void writeAllComparisons (DocumentList & docs, bool remove_common_trigrams)
{
	// output the headings
	for (int i = 0, n = docs.Size (); i < n; ++i)
	{
		std::cout << ", " << docs[i]->GetPathname();
	}
	std::cout << std::endl;
	// output the data table
	for (int i = 0, n = docs.Size (); i < n; ++i)
	{
		std::cout << docs[i]->GetPathname ();
		for (int j = 0, m = docs.Size (); j < m; ++j)
		{
			std::cout << ", ";
			if (i == j)
				std::cout << "1.0";
			else if (i < j) // resemblance is symmetric, and assumes i < j
				std::cout << docs.ComputeResemblance (i, j, remove_common_trigrams);
			else
				std::cout << docs.ComputeResemblance (j, i, remove_common_trigrams);
		}
		std::cout << std::endl;
	}
}

void writeSimilarityTable (DocumentList & docs, bool remove_common_trigrams) 
{
	// output the data
	std::cout << "Number of documents: " << docs.Size () << std::endl;
	std::cout << "Number of distinct trigrams: " << docs.GetTotalTrigramCount () << std::endl;
  if (remove_common_trigrams)
  {
    std::cout << "Similarity measure removes trigrams common to other files" << std::endl;
  }
	for (int i=0; i<docs.Size(); ++i)
		for (int j=i+1; j<docs.Size(); ++j)
		{
			if (docs[i]->GetGroupId () != docs[j]->GetGroupId ())
			{ // only output result if not in same group
				std::cout 
					<< docs[i]->GetPathname () << " ; "
					<< docs[j]->GetPathname () << " ; "
					<< docs.CountMatches (i, j, remove_common_trigrams) << " ; "
					<< docs.CountTrigrams (i) << " ; " 
					<< docs.CountTrigrams (j) << " ; "
					<< docs.ComputeResemblance (i, j, remove_common_trigrams)
					<< std::endl;
			}
		}
}

// Note this must return false if the command-line operation has completed
// -- in wxWidgets returning false from OnInit ceases the application
// This returns true if no command-line options provided, and the application
// must remain running in the graphical mode.
bool FerretApp::OnInit ()
{
	_ferret_help = new HelpFrame ();

	if (argc == 1) // no command-line options, so run the graphical interface
	{
		SelectFiles * frame = new SelectFiles ();
		frame->Show (true);

		return true; // for graphical operation, program must keep running
	}
	else
	{
		Report report_type = DATA_TABLE;	// assume data table is required
		int filenames_start = 1; 		// index within argv of first filename
		wxString definition_file = "";	// string to hold path to definition file
		wxString stored_data = "";		// string to hold path to stored data
		wxString upload_dir = "";		// string to hold path to upload_dir, for html-table
    bool remove_common_trigrams = false; // flag to change type of similarity measure used

		// work through command options, leaving filenames_start pointing at next argument
		while (isCommandOption (argv[filenames_start]) && filenames_start < argc)
		{
			if (isHelpOption (argv[filenames_start]))
			{
				aboutMessage ();
				return false;
			}
			else if (isDataTableOption (argv[filenames_start]))
			{
				report_type = DATA_TABLE;
				filenames_start += 1;
			}
			else if (isListTrigramsOption (argv[filenames_start]))
			{
				report_type = LIST_TRIGRAMS;
				filenames_start += 1;
			}
			else if (isAllComparisonsOption (argv[filenames_start]))
			{
				report_type = ALL_COMPARISONS;
				filenames_start += 1;
			}
      else if (isRemoveCommonTrigramsOption (argv[filenames_start]))
      {
        remove_common_trigrams = true;
        filenames_start += 1;
      }
			else if (isPdfOption (argv[filenames_start]))
			{
				report_type = XML_REPORT;
				//report_type = PDF_REPORT;
				filenames_start += 1;
			}
			else if (isXmlOption (argv[filenames_start]))
			{
				report_type = XML_REPORT;
				filenames_start += 1;
			}
			else if (isDefinitionOption (argv[filenames_start]))
			{
				definition_file = argv[filenames_start+1];
				filenames_start += 2;
			}
			else if (isStoredDataOption (argv[filenames_start]))
			{
				stored_data = argv[filenames_start+1];
				filenames_start += 2;
			}
		}

		// -- carry out required action
		int num_filenames = argc - filenames_start;
		// ---- first check error conditions, basically insufficient files or bad report option
		if ( (num_filenames < 2 && definition_file.IsEmpty () && stored_data.IsEmpty ()) ||	
				(report_type == PDF_REPORT && num_filenames != 3) ||
				(report_type == XML_REPORT && num_filenames != 3))
		{	// not enough filenames, or incorrect use of PDF/XML_REPORT option
			return false;
		}
		else if (report_type == PDF_REPORT || report_type == XML_REPORT) // num_filenames must be 3
		{
			produceComparisonReport (
					argv[filenames_start], 
					argv[filenames_start+1],
				        argv[filenames_start+2],	
					report_type,
          remove_common_trigrams
					);
			return false;
		}
		else // other report options are similar, needing ferret to run on all files
		{
			DocumentList docs;
			int num_preloaded_documents = 0;
			// optionally, retrieve documentlist from store
			if (!stored_data.IsEmpty ())
			{
				docs.RetrieveDocumentList (stored_data);
				num_preloaded_documents = docs.Size ();
				// TODO: catch 'false' return to indicate failure
			}
			// add in provided input files
			// -- from definition file, if provided
			if (wxFileName::IsFileReadable (definition_file))
			{
				docs.AddDocumentsFromDefinitionFile (definition_file);
			}
			// -- and any remaining filenames on command line
			for (int i = filenames_start; i < argc; ++i)
			{
				if (wxFileName::IsFileReadable (argv[i]))
				{
					docs.AddDocument (argv[i]);
				}
			}

			if (docs.Size () < 2) // check we added at least 2 readable files
			{
				aboutMessage ();
				return false;
			}

			std::vector<Document *> to_remove; // keep a list of documents not to be processed
			// make sure every document has its text extracted
			for (int i = 0, n = docs.Size(); i < n; ++i)
			{
				wxString extract_folder = wxGetApp().GetExtractFolder ();
				if (!docs[i]->ExtractDocument (extract_folder, i))
				{
					to_remove.push_back (docs[i]);
				}
			}
			// remove unwanted or failed documents
			for (int i = 0, n = to_remove.size(); i < n; ++i)
			{
				docs.RemoveDocument (to_remove[i]);
			}

			docs.RunFerret (num_preloaded_documents);

			// report output, based on report type
			if (report_type == LIST_TRIGRAMS) 
			{
				 writeTrigramList (docs);
			}
			else if (report_type == ALL_COMPARISONS) 
			{
				writeAllComparisons (docs, remove_common_trigrams);
			}
			else if (report_type == DATA_TABLE)
			{
				writeSimilarityTable (docs, remove_common_trigrams);
			}
			// optionally save out the document table
			if (!stored_data.IsEmpty ())
			{
				docs.SaveDocumentList (stored_data);
			}
		}
		return false; // stops application, as successful
	}
}

