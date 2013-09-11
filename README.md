# ferret #

Ferret is a copy-detection tool, locating duplicate text or code in 
multiple text documents or source files.  Ferret is designed to  
detect copying ( _collusion_ ) within a given set of files, and 
distinguishes copying unique to a pair of documents, across a 
group of documents, and against provided template material.

*Ferret is useful for:*

- document analysis, tracking changes to documents
- software developers, looking for duplicate code to refactor
- software evolution, studying how code has changed over time
- teachers, looking for collusion or plagiarism in student work
- tracking the amount of new material in the current version of a text or program 

*Features:*

- compares text documents containing natural language or computer language
- automatic conversion of standard word processor or pdf formats to text
- processing specialised for major programming languages
- choice of similarity measures to highlight individual or group similarity
- quick loading and comparison of documents, up to computer's memory capacity
- display of _all_ document comparisons, ranked by a similarity score
- detailed display of _individual_ document comparisons, highlighting any text
  in common with the group or template material, or uniquely with compared document
- save result table and comparisons to pdf or xml formats, for printing or further analysis.
- display of unique trigrams per document/group
- template material, whose contents can be excluded from analysis
- display of engagement (overlap with template material) per document/group

*Similarity Measure*

Ferret computes a similarity measure based on the trigrams found within each of
the two documents under comparison; this measure is a number from 0 (no
copying) to 1 (everything has been copied). This measure should _not_ be taken
as an absolute measure of the amount of copying. Instead, the measure is
intended to indicate the _relative_ amount of copying that the current pair has
compared with the rest of the group. Pairs which appear on top of the table of 
all similarity comparisons should be examined for possible copying, but the 
measure itself does not imply any reliable conclusion.

## Installation ##

*Download*

- [Ferret 5.4](http://peterlane.info/downloads/uhferret_5.4_i386.deb) for Ubuntu (32-bit)
- [Ferret 5.4](http://peterlane.info/downloads/ferret-5.4-linux.tgz) for generic Linux (32-bit)
- for Windows (in preparation)

*Install*

Either unpack or follow the instructions in the installer.  The installer will 
create a menu entry for Ferret in your applications menu, under 'Office' for Linux.

*More information*

For more information and options, see <http://peterlane.info/ferret.html>

## Install From Source ##

Download and install [wxWidgets](http://wxwidgets.org) (version 2.9+) and
[wxPdfDocument](http://wxcode.sourceforge.net/components/wxpdfdoc/).

(To obtain a static executable (with all the wx libraries contained within the 
ferret executable) use the configure flag '--disable-shared'.)

Checkout the [source code](https://github.com/petercrlane/ferret) or download 
as a zipped folder by clicking on the ZIP button.

    > cd src
    > make

to build the executable 'uhferret'.  

Ferret is known to compile on Linux (using GTK2) and Windows (using mingw) - it
is believed to compile on Mac OSX, but this is untested.

## Use ##

Start 'uhferret' to run the program.  The graphical interface leads the user through 
the three main stages:

1. select files to analyse, using your computer's file browser
2. list all pairs of files, ranked by degree of copying using a _similarity measure_
3. show a detailed comparison of each pair of files.

Ferret can also be used from the command line. The command-line options are:

    > uhferret --help
    Ferret 5.4: start with no arguments for graphical version
    Usage: ferret [-h] [-d] [-l] [-a] [-r] [-w] [-p] [-x] [-f] [-u]
      -h, --help           	displays help on command-line parameters
      -d, --data-table     	produce similarity table (default)
      -l, --list-trigrams  	produce trigram list report
      -a, --all-comparisons	produce list of all comparisons
      -r, --remove-common   removes common trigrams
      -p, --pdf-report     	source-1 source-2 results-file : create pdf report
      -x, --xml-report     	source-1 source-2 results-file : create xml report
      -f, --definition-file	use file with document list
      -u, --use-stored-data	store/retrieve data structure

More details on using Ferret can be found in the 
[manual](http://peterlane.info/downloads/ferret-manual.pdf).

### Supported File Types / Languages ###

The following list gives the recognised types of file or computer code within 
Ferret, with the recognised file extension in brackets. To request an addition to this 
list, raise an issue at <https://github.com/petercrlane/ferret> or contact the author.

- Text documents (.txt)
- Word processor formats (.doc, .docx, .rtf, .abw)
- Pdf documents (.pdf)
- Computer languages
  - ActionScript (.as, .actionscript)
  - C/C++ (.h, .c, .cpp)
  - C# (.cs)
  - Clojure (.clj)
  - Groovy (.groovy)
  - Haskell (.hs, .lhs)
  - Java (.java)
  - Lisp (.lisp, .lsp)
  - Lua (.lua)
  - PHP (.php)
  - Prolog (.pl)
  - Python (.py)
  - Racket (.rkt)
  - Ruby (.rb)
  - Scheme (.scm, .ss)
  - Visual Basic (.vb)
  - XML/HTML (.xml, .html)

## Implementation ##

This project defines a version of ferret runnable as a standalone 
application.  Ferret is written in C++ and uses the 
[wxWidgets](http://wxwidgets.org) and [wxPdfDocument](http://wxcode.org)
libraries. Text conversion from word processor or pdf formats is through 
calling [abiword](http://www.abisource.com) or [pdftotext](http://www.xpdf.com).

For a version designed for embedding in your own scripts, see 
[uhferret-gem](https://github.com/petercrlane/uhferret-gem).

## License ##

Ferret is released under the [GPL](http://www.gnu.org/licenses/gpl.html).
It may be used freely, with the only restriction that it may not be sold 
or distributed as a component of a proprietary (closed-source) product.
Please read the license for more details, or otherwise contact the author 
for clarification.

## History ##

Version 5.4:

- selection and use of template material
- engagement table
- added green highlighting in displays to show overlap with template material
- added more languages

Version 5.3:

- file-specific tokenising of major programming languages (see list above)
- no user-selection of text/code format (gui retains option to process 
  unrecognised files as txt or word-processed files)
- drag and drop files onto Select Files dialog
- include option to compute similarities after excluding trigrams common to other files
- added display of document-unique trigram count
- red-blue-black colour for confirming pairwise copying implemented on displays/pdf/xml
- grouping of files based on base directory in select files display

