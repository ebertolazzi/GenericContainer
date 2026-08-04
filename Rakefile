# frozen_string_literal: true

require 'fileutils'
require 'rake/clean'
require 'rbconfig'
require 'shellwords'

PROJECT_ROOT = File.expand_path(__dir__)
BUILD_DIR    = File.join(PROJECT_ROOT, 'build')
INSTALL_DIR  = File.join(PROJECT_ROOT, 'lib')
BIN_DIR      = File.join(PROJECT_ROOT, 'bin')
YELLOW       = "\e[33m"
RESET        = "\e[0m"

# Build switches. Override from the command line, for example:
#   rake build BUILD_TYPE=Debug SHARED=1
BUILD_TYPE = ENV.fetch('BUILD_TYPE', 'Release')
BUILD_SHARED_LIBS = ENV.fetch('SHARED', '0').match?(/\A(1|on|true|yes)\z/i)

# Backend switches. Set any of these to 0/off/false/no to skip the backend.
def enabled_env?(name, default: true)
  value = ENV.fetch(name, default ? 'ON' : 'OFF')
  value.match?(/\A(1|on|true|yes)\z/i)
end

ENABLE_YAML = enabled_env?('YAML')
ENABLE_TOML = enabled_env?('TOML')
ENABLE_LUA  = enabled_env?('LUA')
ENABLE_EXAMPLES = enabled_env?('EXAMPLES', default: false)

CLEAN.clear_exclude.exclude { |fn| fn.pathmap('%f').casecmp('core').zero? }
CLEAN.include('**/*.o', '**/*.obj')
CLOBBER.include(BUILD_DIR)

abort "Unsupported platform: #{RbConfig::CONFIG['host_os']}" unless RbConfig::CONFIG['host_os'].match?(/darwin|linux|cygwin|mingw|msys|mswin|windows/i)

def cmake_bool(value)
  value ? 'ON' : 'OFF'
end


def command_string(*cmd)
  cmd.flatten.map { |part| Shellwords.escape(part.to_s) }.join(' ')
end


def sh_echo(*cmd, chdir: nil)
  rendered = command_string(*cmd)
  rendered = "cd #{Shellwords.escape(chdir)} && #{rendered}" if chdir
  puts "#{YELLOW}#{rendered}#{RESET}"
  return sh(*cmd) if chdir.nil?

  Dir.chdir(chdir) { sh(*cmd) }
end


def cmake_build_parallel_args
  processors = begin
    require 'etc'
    Etc.respond_to?(:nprocessors) ? Etc.nprocessors : nil
  rescue LoadError
    nil
  end

  processors.to_i.positive? ? ['--parallel', processors.to_s] : ['--parallel']
end


def configure_args(build_testing: false, build_examples: false)
  [
    'cmake',
    '-S', PROJECT_ROOT,
    '-B', BUILD_DIR,
    "-DCMAKE_BUILD_TYPE=#{BUILD_TYPE}",
    "-DBUILD_SHARED_LIBS=#{cmake_bool(BUILD_SHARED_LIBS)}",
    "-DBUILD_TESTING=#{cmake_bool(build_testing)}",
    "-DGENERIC_CONTAINER_BUILD_EXAMPLES=#{cmake_bool(build_examples)}",
    "-DGENERIC_CONTAINER_ENABLE_YAML=#{cmake_bool(ENABLE_YAML)}",
    "-DGENERIC_CONTAINER_ENABLE_TOML=#{cmake_bool(ENABLE_TOML)}",
    "-DGENERIC_CONTAINER_ENABLE_LUA=#{cmake_bool(ENABLE_LUA)}",
    '-DGENERIC_CONTAINER_ALLOW_NETWORK_FETCH=OFF'
  ]
end


desc 'Configure the CMake build tree without tests or examples'
task :configure do
  FileUtils.mkdir_p(BUILD_DIR)
  sh_echo(*configure_args(build_testing: false, build_examples: false))
end

desc 'Configure the CMake build tree with unit tests enabled'
task :configure_tests do
  FileUtils.mkdir_p(BUILD_DIR)
  sh_echo(*configure_args(build_testing: true, build_examples: ENABLE_EXAMPLES))
end

desc 'Build GenericContainer'
task build: :configure do
  sh_echo('cmake', '--build', BUILD_DIR, '--config', BUILD_TYPE, *cmake_build_parallel_args)
  sh_echo('cmake', '--install', BUILD_DIR, '--config', BUILD_TYPE, '--prefix', INSTALL_DIR)
end

desc 'Build and run unit tests from the tests/ directory through CTest'
task tests: :configure_tests do
  sh_echo('cmake', '--build', BUILD_DIR, '--config', BUILD_TYPE, '--target', 'gc_unit_tests', *cmake_build_parallel_args)
  sh_echo('ctest', '--test-dir', BUILD_DIR, '--build-config', BUILD_TYPE, '--output-on-failure', '-L', 'unit')
end

desc 'Build and run unit tests'
task run: :tests

desc 'Alias for tests'
task test: :tests

desc 'Build source/binary packages with CPack'
task cpack: :build do
  sh_echo('cmake', '--build', BUILD_DIR, '--config', BUILD_TYPE, '--target', 'package')
end

desc 'Remove generated build artifacts'
task :clean do
  FileUtils.rm_rf(BUILD_DIR)
  FileUtils.rm_rf(INSTALL_DIR)
  FileUtils.rm_rf(BIN_DIR)
end

task default: :build
