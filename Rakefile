if File.exist?(File.expand_path('./cmake_utils/Rakefile_common.rb', File.dirname(__FILE__))) then
  require_relative "./cmake_utils/Rakefile_common.rb"
else
  require_relative "../Rakefile_common.rb"
end

CLEAN.include   ["./**/*.o", "./**/*.obj", "./bin/**/example*", "./build"]
CLEAN.clear_exclude.exclude { |fn| fn.pathmap("%f").downcase == "core" }
CLOBBER.include []

desc "default task --> build"
task :default => :build

TESTS = [
  "example1",
  "example2",
  "example3",
  "example4",
  "example5",
  "example6",
  "example7",
  "example8",
  "example9",
  "example10",
  "example11",
  "example12",
  "example13",
  "example14"
]

TESTS_LUA = [
  "example_lua1",
  "example_lua2",
  "example_lua3"
]

"run tests on linux/osx"
task :run do
  TESTS.each do |cmd|
    exe = "./bin/#{cmd}"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "src_lua_interface/tests"
  TESTS_LUA.each do |cmd|
    exe = "../../bin/#{cmd}"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../.."
end

desc "run tests (Release) on windows"
task :run_win do
  BIN = 'bin\\'
  TESTS.each do |cmd|
    exe = "#{BIN}/#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "src_lua_interface/tests"
  TESTS_LUA.each do |cmd|
    exe = "..\\..\\#{BIN}#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../../src_json_interface/tests"
  TESTS_JSON.each do |cmd|
    exe = "..\\..\\#{BIN}#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../.."
end

desc "compile for Visual Studio [default year=2017, bits=x64]"
task :build_win do
  # check architecture
  case `where cl.exe`.chop
  when /x64\\cl\.exe/
    VS_ARCH = 'x64'
  when /amd64\\cl\.exe/
    VS_ARCH = 'x64'
  when /bin\\cl\.exe/
    VS_ARCH = 'x86'
  else
    raise RuntimeError, "Cannot determine architecture for Visual Studio".red
  end

  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  FileUtils.rm_rf   "build"
  FileUtils.mkdir_p "build"
  FileUtils.cd      "build"

  puts "run CMAKE for GenericContainer".yellow
  sh "cmake -G Ninja -DBITS:VAR=#{VS_ARCH} " + cmd_cmake_build() + ' ..'

  puts "compile with CMAKE for GenericContainer".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL
  else
    sh 'cmake  --build . --config Release  --target install '+PARALLEL
  end

  FileUtils.cd '..'
end

desc 'compile for OSX/LINUX/MINGW'
task :build_osx_linux_mingw do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  FileUtils.rm_rf   "build"
  FileUtils.mkdir_p "build"
  FileUtils.cd      "build"

  puts "run CMAKE for GenericContainer".yellow
  sh "cmake -G Ninja " + cmd_cmake_build() + ' ..'

  puts "compile with CMAKE for GenericContainer".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL
  else
    sh 'cmake --build . --config Release --target install '+PARALLEL
  end

  FileUtils.cd '..'
end

desc 'clean for OSX/LINUX/MINGW'
task :clean_osx_linux_mingw do
  FileUtils.rm_rf 'build'
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end

desc 'compile for LINUX'
task :build_linux => :build_osx_linux_mingw

desc 'compile for OSX'
task :build_osx => :build_osx_linux_mingw

desc 'compile for MINGW'
task :build_mingw => :build_osx_linux_mingw

desc 'clean for LINUX'
task :clean_linux => :clean_osx_linux_mingw

desc 'clean for OSX'
task :clean_osx => :clean_osx_linux_mingw

desc 'clean for MINGW'
task :clean_mingw => :clean_osx_linux_mingw

desc 'clean for WINDOWS'
task :clean_win => :clean_osx_linux_mingw

desc 'pack for OSX/LINUX/MINGW/WINDOWS'
task :cpack do
  FileUtils.cd "build"
  puts "run CPACK for GenericContainer".yellow
  sh 'cpack -C CPackConfig.cmake'
  sh 'cpack -C CPackSourceConfig.cmake'
  FileUtils.cd ".."
end
