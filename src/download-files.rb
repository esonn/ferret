# Download Files from Trigrams
# Copyright (c) 2008, Peter C. R. Lane.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
#
# ---------------------------------------------------------------------

# Ruby program to download files from internet, 
# designed to be called from Ferret, 
# but usable from command-line, for testing purposes

# Note that Yahoo is used for internet searches.  Yahoo permits 
# searches/downloads per day of the order of 1000s.  
# TODO: Put in a check for exceeding the download limit.

# Call from command line with four inputs:
# folder to download files into 
# filename of list of tuples to search on
# OPTIONAL maximum number of documents to download per tuple
# OPTIONAL maximum number of documents to place into Download folder
# OPTIONAL maximum number of trigrams to use for searching

require "net/http"
require 'rexml/document'
require 'open-uri'
require 'uri'
include REXML

# main function
class Downloader
  def initialize
    @candidate_urls = [] # hold the found URL addresses in local variable
    if ARGV.length < 2 || ARGV.length > 5
    then # error in input
      puts "Try again with correct number of inputs"
      puts "download-files.rb: download_folder tuple_filename [max_documents max
_downloads max_tuple_searches]"
      exit
    else # set the given parameters
      # folder to download documents into
      @download_folder = ARGV[0]
      # filename of list of tuples to search on
      @tuple_filename = ARGV[1]
      # maximum number of documents to finally download, defaults to 10
      @max_documents = if ARGV.length > 2 then ARGV[2].to_i else 10 end
      # maximum number of downloads to consider per tuple, defaults to 10
      @max_downloads = if ARGV.length > 4 then ARGV[3] else 10 end
      # maximum number of tuples to use for searching, defaults to 50
      @max_tuple_searches = if ARGV.length == 5 then ARGV[4].to_i else 50 end

      puts "Called with download folder: #{@download_folder}, #{@tuple_filename}"
    end
  end

  # -- utility function to return last part of string after right-most token
  #    or whole string if token not present
  def get_tail(str, token)
    if str.rindex(token)
      then str[str.rindex(token)+1, str.length]
      else str
    end
  end

  # -- utility function to check if string finishes with given character
  def ends_with(str, c) 
    str.rindex(c) == str.length-1 
  end

  # check if given URL is a document to download
  # -- only rule out directories, which end in '/'
  def valid_document? url
     not(ends_with(url, '/'))
  end

  # add given URL to candidates if it is a kind of document
  def add_url url
    @candidate_urls.push url if valid_document? url
  end

  # Get search results from Yahoo for given tuple
  # Note: spaces in tuple are replaced with '%20' symbols, else string does not get parsed correctly
  def get_search_results tuple
    requestString = "http://search.yahooapis.com/WebSearchService/V1/webSearch?appid=WebFerret&query=%22#{tuple.gsub(" ", "%20")}%22&results=#{@max_downloads}"
    Net::HTTP.get_response(URI.parse(requestString)).body
  end

  # Search for URL addresses based on given tuple, storing addresses in candidate_urls
  # note: request to Yahoo ensures only @max_downloads are made
  def search_tuple tuple
    begin
      Document.new(get_search_results(tuple)).root.each_element('//Result') do |result| 
        add_url(result.elements['Url'].get_text.value)
      end
    rescue # ignore errors
    end
  end

  # Search for each tuple in tuple_filename
  # -- take max_tuple_searches tuples at random from list
  def search_tuples
    tuples = [] # read tuples in from tuple_filename, one tuple per line
    File.open(@tuple_filename) do |file|
      while line = file.gets
        tuples << line 
      end
    end
    @max_tuple_searches.times do search_tuple(tuples[Kernel.rand(tuples.length)])
 end
  end

  # count the number of times item appears in the candidate_urls list
  def count_times item
    count = 0
    @candidate_urls.each { |i| if i == item then count = count + 1 end }
    count
  end

  # return candidate URLs - sorted into order of frequency with most frequent first
  def candidate_urls
    urls = @candidate_urls.uniq  # remove duplicates
    urls.sort! { |x,y| count_times(y) <=> count_times(x) }
  end

  # download document specified and place it into the Download folder
  def download_document url
    begin
      open(@download_folder + '/' + get_tail(url, '/'), "wb").write(open(url).read)
    rescue # ignore errors as this simply means no file will be placed into download_folder
    end
  end

  # Download each document in first part of ordered candidate URLs, saving result in download_folder
  def download_documents
    candidate_urls.first(@max_documents).each do |url| 
      download_document url
    end
  end

  # the main cycle of steps to locate and download similar documents
  def get_documents
    search_tuples
    download_documents
  end
end

# run Downloader system
Downloader.new.get_documents

