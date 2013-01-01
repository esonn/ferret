# ferret #

Ferret is a copy-detection tool, locating duplicate text or code in 
multiple text documents or source files.  Although mostly intended 
for detecting copying (*collusion*) within a given set of files, a 
limited web search capability helps check copying (*plagiarism*) 
against external web sources.

Ferret is useful for:

- document analysis, tracking changes to documents
- software developers, looking for duplicate code to refactor
- software evolution, studying how code has changed over time
- teachers, scanning for collusion in student work

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

