%w(colorize rake fileutils).each do |gem|
  begin
    require gem
  rescue LoadError
    warn "Install the #{gem} gem:\n $ (sudo) gem install #{gem}".magenta
    exit 1
  end
end

require "rake/clean"

CLEAN.include   ["./**/*.o", "./**/*.obj", "./bin/**/example*", "./build"]
CLOBBER.include []
CLEAN.exclude('**/[cC][oO][rR][eE]')

case RUBY_PLATFORM
when /darwin/
  OS = :mac
when /linux/
  OS = :linux
when /cygwin|mswin|mingw|bccwin|wince|emx/
  OS = :win
end

require_relative "./Rakefile_common.rb"

desc "default task --> build"
task :default => :build

file_base = File.expand_path(File.dirname(__FILE__)).to_s

cmd_cmake_build = ""
if COMPILE_EXECUTABLE then
  cmd_cmake_build += ' -DEB_ENABLE_TESTS:VAR=ON '
else
  cmd_cmake_build += ' -DEB_ENABLE_TESTS:VAR=OFF '
end
if COMPILE_DYNAMIC then
  cmd_cmake_build += ' -DEB_BUILD_SHARED:VAR=ON '
else
  cmd_cmake_build += ' -DEB_BUILD_SHARED:VAR=OFF '
end
if COMPILE_DEBUG then
  cmd_cmake_build += ' -DCMAKE_BUILD_TYPE:VAR=Debug --loglevel=WARNING '
else
  cmd_cmake_build += ' -DCMAKE_BUILD_TYPE:VAR=Release --loglevel=WARNING '
end

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
  "JSON_test_1"
]

TESTS_LUA = [
  "example_lua1",
  "example_lua2",
  "example_lua3"
]

TESTS_JSON = [
  "JSON-test-1"
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
  FileUtils.cd "../../src_json_interface/tests"
  TESTS_JSON.each do |cmd|
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

desc "build lib"
task :build do
  puts "UTILS build".green
  case OS
  when :mac
    Rake::Task[:build_osx].invoke
  when :linux
    Rake::Task[:build_linux].invoke
  when :win
    Rake::Task[:build_win].invoke
  end
end

desc "compile for Visual Studio"
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

  if RUN_CPACK then
    puts "run CPACK for ROOTS".yellow
    sh 'cpack -C CPackConfig.cmake'
    sh 'cpack -C CPackSourceConfig.cmake'
  end

  FileUtils.cd '..'
end

desc 'compile for OSX'
task :build_osx do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  dir = "build"

  FileUtils.rm_rf   dir
  FileUtils.mkdir_p dir
  FileUtils.cd      dir

  cmd_cmake = "cmake " + cmd_cmake_build

  puts "run CMAKE for GenericContainer".yellow
  sh cmd_cmake + ' ..'
  puts "compile with CMAKE for GenericContainer".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL+QUIET
  else
    sh 'cmake --build . --config Release --target install '+PARALLEL+QUIET
  end

  if RUN_CPACK then
    puts "run CPACK for ROOTS".yellow
    sh 'cpack -C CPackConfig.cmake'
    sh 'cpack -C CPackSourceConfig.cmake'
  end

  FileUtils.cd '..'
end

desc 'compile for LINUX'
task :build_linux => :build_osx

desc "clean for OSX"
task :clean_osx do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end

desc "clean for LINUX"
task :clean_linux => :clean_osx

desc "clean for WINDOWS"
task :clean_win => :clean_osx
