task :build do
  `cobra -c src/*.cobra -out:ferret.exe`
end
