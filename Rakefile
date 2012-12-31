desc "build ferret"
task :build do 
  Dir.chdir("src") do
    sh("make ferret")
    if File.exist?("ferret")
      sh("strip ferret")
      sh("mv ferret ..")
    end
  end
end

desc "find occurrences of text, call with: 'rake find[text]'"
task :find, :text do |v, args|
  sh("grep #{args[:text]} src/*.cpp")
  sh("grep #{args[:text]} src/*.h")
end

