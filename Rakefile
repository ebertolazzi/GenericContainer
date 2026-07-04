%w(colorize fileutils rake/clean).each do |gem|
  begin
    require gem
  rescue LoadError
    warn "Install the #{gem} gem:\n $ (sudo) gem install #{gem}"
    exit 1
  end
end

require 'rake/clean'
# avoid to remove file "core" (in Eigen inclusion)
CLEAN.clear_exclude.exclude { |fn| fn.pathmap("%f").downcase == "core" }

CLEAN.include   ["./**/*.o", "./**/*.obj", "./bin/**/example*", "./build"]
CLEAN.clear_exclude.exclude { |fn| fn.pathmap("%f").downcase == "core" }
CLOBBER.include []

#
# Check for a configuration file on a upper directory.
# This permits to use a unique configuration file for
# large projects.
# On a local project use the default in this file.
#
if File.exist?(File.expand_path('../Rakefile_configure.rb', File.dirname(__FILE__))) then
  # found in the root of the local project
  require_relative '../Rakefile_configure.rb'
elsif File.exist?(File.expand_path('../../Rakefile_configure.rb', File.dirname(__FILE__))) then
  # found in the upper project
  require_relative '../../Rakefile_configure.rb'
elsif File.exist?(File.expand_path('../../cmake_utils/Rakefile_configure.rb', File.dirname(__FILE__))) then
  # found in the upper project under cmake_utils
  require_relative '../../cmake_utils/Rakefile_configure.rb'
else
  #-------------------------
  COMPILE_DEBUG      = false
  COMPILE_DYNAMIC    = false
  COMPILE_EXECUTABLE = true
  #-------------------------
end

#    ___  ____
#   / _ \/ ___|
#  | | | \___ \
#  | |_| |___) |
#   \___/|____/
#
case RUBY_PLATFORM
when /darwin/
  OS  = :mac
  CMD = "build.sh"
when /linux|cygwin/ # cygwin compile as a linux system
  OS  = :linux
  CMD = "build.sh"
when /msys/
  # msys2 envirorment to compile with MINGW
  OS  = :mingw
  CMD = "build.sh"
else # assume windows
  OS  = :win
  CMD = "build.ps1"
end

PROJECT_ROOT = File.expand_path(File.dirname(__FILE__))

def build_script_path
  File.join(PROJECT_ROOT, CMD)
end

def run_build_script(*args)
  script = build_script_path

  unless File.exist?(script)
    abort "Build script not found: #{script}"
  end

  if OS == :win
    sh 'powershell', '-ExecutionPolicy', 'Bypass', '-File', script, *args
  else
    sh script, *args
  end
end

#   ____  _   _ ___ _     ____
#  | __ )| | | |_ _| |   |  _ \
#  |  _ \| | | || || |   | | | |
#  | |_) | |_| || || |___| |_| |
#  |____/ \___/|___|_____|____/
#
desc "default task --> build"
task :default => :build

desc 'compile GenericContainer'
task :build do

  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'

  puts "run CMAKE for GenericContainer".yellow

  if COMPILE_DEBUG then
    cmd = "#{CMD} build Release"
  else
    cmd = "#{CMD} build Debug"
  end
  puts cmd.yellow
  run_build_script 'build', (COMPILE_DEBUG ? 'Release' : 'Debug')
end

desc 'clean for OSX/LINUX/MINGW'
task :clean_osx_linux_mingw do
  FileUtils.rm_rf 'build'
  FileUtils.rm_rf 'lib'
  FileUtils.rm_rf 'lib3rd'
end

desc 'pack for OSX/LINUX/MINGW/WINDOWS'
task :cpack do
  run_build_script 'package'
end
