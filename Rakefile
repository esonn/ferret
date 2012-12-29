desc "build ferret"
task :build do 
  Dir.chdir("src") do
    sh("make ferret")
    if File.exist?("src/ferret")
      sh("mv src/ferret .")
    end
  end
end

