require_relative "./cmake_utils/Rakefile_common.rb"

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

desc "compile for Visual Studio [default year=2017, bits=x64]"
task :build_win, [:year, :bits] do |t, args|
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  args.with_defaults( :year => "2017", :bits => "x64" )

  FileUtils.rm_rf   "build"
  FileUtils.mkdir_p "build"
  FileUtils.cd      "build"

  cmd_cmake = cmake_generation_command(args.bits,args.year) + cmd_cmake_build()

  puts "run CMAKE for GenericContainer".yellow
  sh cmd_cmake + ' ..'
  puts "compile with CMAKE for GenericContainer".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL+QUIET
  else
    sh 'cmake  --build . --config Release  --target install '+PARALLEL+QUIET
  end

  FileUtils.cd '..'
end

desc 'compile for OSX/LINUX/MINGW'
task :build_osx_linux_mingw do
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  dir = "build"

  FileUtils.rm_rf   dir
  FileUtils.mkdir_p dir
  FileUtils.cd      dir

  cmd_cmake = "cmake " + cmd_cmake_build()

  puts "run CMAKE for GenericContainer".yellow
  sh cmd_cmake + ' ..'
  puts "compile with CMAKE for GenericContainer".yellow
  if COMPILE_DEBUG then
    sh 'cmake --build . --config Debug --target install '+PARALLEL+QUIET
  else
    sh 'cmake --build . --config Release --target install '+PARALLEL+QUIET
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
