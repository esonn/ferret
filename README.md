# ferret #

Ferret is a copy-detection tool, locating duplicate text or code in 
multiple text documents or source files.  Although mostly intended 
for detecting copying ( _collusion_ ) within a given set of files, a 
limited web search capability helps check copying ( _plagiarism_ ) 
against external web sources.

*Ferret is useful for:*

- document analysis, tracking changes to documents
- software developers, looking for duplicate code to refactor
- software evolution, studying how code has changed over time
- teachers, scanning for collusion in student work
- tracking the amount of new material in the current version of a text or program 

*Features*

- select files in text, word processor, or pdf formats
- automatic conversion to text format
- limited internet search for similar documents, to detect plagiarism 
- compares either natural language or computer language documents
- quick loading and comparison of documents, up to computer's memory capacity
- display of _all_ document comparisons, ranked by a similarity score
- detailed display of _individual_ document comparisons, highlighting any copied text
- save comparisons to pdf or xml formats, for printing.

*Similarity Measure*

Ferret computes a similarity measure based on the trigrams found within each of
the two documents under comparison; this measure is a number from 0 (no
copying) to 1 (everything has been copied). This measure should _not_ be taken
as an absolute measure of the amount of copying. Instead, the measure is
intended to indicate the _relative_ amount of copying that the current pair has
compared with the rest of the group. Pairs which appear on top of the table of 
all similarity comparisons should be examined for possible copying.

## Installation ##

This software is still being tested and repackaged.  Installers will be made available 
for versions of Linux and Windows.

## Status ##

*Working*

- compiles using wxWidgets 2.9.4 and wxPdfDocument 0.9.3
- displays similarities of files pairwise
- pdf output of results table
- file conversion from doc/pdf to txt

*Error Messages*

- error messages on comparing files for display or output

*Untested* (probably not working)

- downloading comparisons from internet

## Implementation ##

This project defines a version of ferret runnable as a standalone 
application.  Ferret is written in C++ and uses the 
[wxWidgets](http://wxwidgets.org) and [wxPdfDocument](http://wxcode.org)
libraries. Text conversion from word processor or pdf formats is through 
calling [abiword](http://www.abisource.com) or [pdftotext](http://www.xpdf.com).

For a version designed for scripting, use in a web application, or general
hacking, see [uhferret-gem](https://github.com/petercrlane/uhferret-gem).

## License ##

Ferret is released under the [GPL](http://www.gnu.org/licenses/gpl.html).

