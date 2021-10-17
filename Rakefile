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

require_relative "./Rakefile_common.rb"

file_base = File.expand_path(File.dirname(__FILE__)).to_s

cmd_cmake_build = ""
if COMPILE_EXECUTABLE then
  cmd_cmake_build += ' -DBUILD_EXECUTABLE:VAR=true '
else
  cmd_cmake_build += ' -DBUILD_EXECUTABLE:VAR=false '
end
if COMPILE_DYNAMIC then
  cmd_cmake_build += ' -DBUILD_SHARED:VAR=true '
else
  cmd_cmake_build += ' -DBUILD_SHARED:VAR=false '
end
if COMPILE_DEBUG then
  cmd_cmake_build += ' -DCMAKE_BUILD_TYPE:VAR=Debug --loglevel=WARNING '
else
  cmd_cmake_build += ' -DCMAKE_BUILD_TYPE:VAR=Release --loglevel=WARNING '
end
cmd_cmake_build += " -DINSTALL_HERE:VAR=true "

TESTS = [
  "example1",
  "example2",
  "example3",
  "example4",
  "example5",
  "example6",
  "example7",
  "example8",
  "example9"
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
  TESTS.each do |cmd|
    sh "bin\\Release\\#{cmd}.exe"
  end
  FileUtils.cd "src_lua_interface/tests"
  TESTS_LUA.each do |cmd|
    exe = "..\\..\\bin\\Release\\#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../../src_lua_interface/tests"
  TESTS_JSON.each do |cmd|
    exe = "..\\..\\bin\\Release\\#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../.."
end

desc "run tests (Debug) on windows"
task :run_win_debug do
  TESTS.each do |cmd|
    sh "bin\\Debug\\#{cmd}.exe"
  end
  FileUtils.cd "src_lua_interface/tests"
  TESTS_LUA.each do |cmd|
    exe = "..\\..\\bin\\Debug\\#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../../src_lua_interface/tests"
  TESTS_JSON.each do |cmd|
    exe = "..\\..\\bin\\Debug\\#{cmd}.exe"
    sh exe if File.exist?(exe)
  end
  FileUtils.cd "../.."
end

desc "compile for Visual Studio [default year=2017, bits=x64]"
task :build_win, [:year, :bits] do |t, args|
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  args.with_defaults( :year => "2017", :bits => "x64" )

  dir = "vs_#{args.year}_#{args.bits}"

  FileUtils.rm_rf   dir
  FileUtils.mkdir_p dir
  FileUtils.cd      dir

  cmd_cmake = win_vs(args.bits,args.year) + cmd_cmake_build

  puts "run CMAKE for GenericContainer".yellow
  sh cmd_cmake + ' ..'
  puts "compile with CMAKE for UTILS".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL+QUIET
  else
    sh 'cmake  --build . --config Release  --target install '+PARALLEL+QUIET
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
  puts "compile with CMAKE for UTILS".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL+QUIET
  else
    sh 'cmake  --build . --config Release  --target install '+PARALLEL+QUIET
  end

  FileUtils.cd '..'
end

desc 'compile for LINUX'
task :build_linux => [ :build_osx ] do
end

desc "clean for OSX"
task :clean_osx do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end

desc "clean for LINUX"
task :clean_linux do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end

desc "clean for WINDOWS"
task :clean_win do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end
