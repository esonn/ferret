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

