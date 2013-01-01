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
    sh("sudo cp ../uhferret /usr/local/bin/uhferret")
    begin
      sh("rm *.deb")
    rescue # ignore error when no .deb files present
    end
    sh("fpm -s dir -t deb --description 'Ferret is a copy-detection tool' --url 'http://github.com/petercrlane/ferret' -m 'Peter Lane<peter.lane@bcs.org.uk' -n uhferret -v 5.0 /usr/local/bin/uhferret")
  end
end
