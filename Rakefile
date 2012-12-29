desc "build ferret"
task :build do 
  Dir.chdir("src") do
    sh("make ferret")
    if File.exists("src/ferret.exe")
      sh("mv src/ferret.exe .")
    end
  end
end

