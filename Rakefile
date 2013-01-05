desc "build ferret"
task :build do 
  Dir.chdir("src") do
    sh("make ferret")
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
Version=1.0
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
    `fpm -s dir -t deb --description 'Ferret is a copy-detection tool' --url 'http://github.com/petercrlane/ferret' -m 'Peter Lane<peter.lane@bcs.org.uk>' -n uhferret -v 5.2 /usr/local/bin/uhferret /usr/share/applications/uhferret.desktop /usr/share/icons/uhferret.ico`
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
