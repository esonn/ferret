VERSION = 5.3 # number for next planned release

desc "display planned features"
task :todo do
  puts "Planned/possible additional functionality for Ferret"
  puts "- ignore comments in code files (option in gui and command-line)"
  puts "- option to check with user about type of unknown files"
  puts "- engagement chart (count of trigrams also in provided code)"
  puts "- display of groups of copied code"
  puts "- unicode, including Chinese etc processing (Bao et al 2006)"
  puts "  (the issue here is more display/output of non-ASCII characters)"
  puts "- show mean/mode of scores below result tables"
end

desc "build ferret"
task :build do 
  Dir.chdir("src") do
    sh "make ferret"
    if File.exist?("ferret")
      `strip ferret`
      `mv ferret ../uhferret`
      `chmod g-x ../uhferret`
    end
  end
end

desc "find occurrences of text, call with: 'rake find[text]'"
task :find, :text do |v, args|
  begin
    sh("grep #{args[:text]} src/*.cpp")
  rescue
  end
  begin
    sh("grep #{args[:text]} src/*.h")
  rescue
  end
end

directory "release"

desc "use fpm to create release packages"
task :make_deb => :build do 
  Dir.chdir("release") do
    # create the desktop file
    File.open("uhferret.desktop", "w") do |file|
      file.puts <<-END
[Desktop Entry]
Type=Application
Version=#{VERSION}
Name=Ferret
GenericName=Ferret: Copy-detection program
Icon=/usr/share/icons/uhferret.ico
Exec=/usr/local/bin/uhferret
Categories=Office;
END
    end
    # copy files into position
    `sudo cp ../uhferret /usr/local/bin/uhferret`
    `sudo cp uhferret.desktop /usr/share/applications/uhferret.desktop`
    `sudo cp uhferret.ico /usr/share/icons/uhferret.ico`
    # remove any old deb files
    begin
      `rm *.deb`
    rescue # ignore error when no .deb files present
    end
    # construct the deb file
    `sudo fpm -s dir -t deb --description 'Ferret is a copy-detection tool' --url 'http://peterlane.info/ferret.html' -m 'Peter Lane<peter.lane@bcs.org.uk>' -n uhferret -v #{VERSION} /usr/local/bin/uhferret /usr/share/applications/uhferret.desktop /usr/share/icons/uhferret.ico`
    puts "Deb package:"
    puts `md5sum *.deb`
  end
end

desc "construct a tgz of compiled version with install instructions"
task :make_tgz => :build do
  Dir.chdir("release") do    
    `mkdir ferret-linux`
    `cp ../uhferret ferret-linux/uhferret`
    File.open("ferret-linux/install.txt", "w") do |file|
      file.puts <<END
Ferret Installation: Generic Linux
==================================

This folder contains the executable 'uhferret'.  You can run 
ferret by double-clicking on the executable.  Alternatively, 
copy it to /usr/local/bin or /usr/bin.  You may find it 
convenient to add a link for uhferret to your application 
menu or toolbar.
END
    end
    `tar zcvf ferret-linux.tgz ferret-linux/`
    puts `md5sum ferret-linux.tgz`
    `rm -rf ferret-linux`
  end
end

desc 'create Ferret manual'
task :manual do
  Dir.chdir("manual") do
    sh "asciidoc-bib -s chicago-author-date manual.txt"
    sh "a2x -v -fpdf -darticle --dblatex-opts \"-P latex.output.revhistory=0\" manual-ref.txt"
    sh "mv manual-ref.pdf manual.pdf"
  end
end
