#if !defined main_ferret_h
#define main_ferret_h

/** written by Peter Lane, 2006-2008
  * (c) School of Computer Science, University of Hertfordshire
  */

#define MAX_TABLE_SIZE 5000 // Maximum number of pairs to write into HTML output

#include <iostream>
#include <wx/cmdline.h>
#include "ferretapp.h"
#include "pdfreport.h"
#include "selectfiles.h"

/** This file defines the OnInit virtual method to go with FerretApp.
  * OnInit is the method called when the program starts, and handles 
  * the switch between graphical and command-line operation.
  */

class HelpFrame; // forward declaration

enum Report { LIST_TRIGRAMS, ALL_COMPARISONS, DATA_TABLE, PDF_REPORT, XML_REPORT };

#endif

