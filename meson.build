project(
  'nesvary',
  'cpp',
  version : '0.0.1',
  default_options : ['warning_level=3']
  )

project_description = 'will unlikely become a full nes emulator'

project_headers = [
  'include/ricoh2a03.hpp',
  'include/nesvary.h'
  ]

project_source_files = [
  'src/ricoh2a03.cpp',
  'src/nesvary.cpp'
  ]

project_test_files = [
  'tests/src/ricoh2a03_test.cpp',
  'tests/src/general_tests.cpp'
  ]

project_dep = [
  ]

test_dep = [
  ]
#=====================================
# Targets

public_headers = include_directories('include')

build_args = [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version(),
  ]

project_executable = executable(
  meson.project_name(),
  project_source_files,
  dependencies: project_dep,
  install: true,
  cpp_args: build_args,
  include_directories: public_headers
  )


project_shared_library = shared_library(
  meson.project_name(),
  project_source_files,
  dependencies: project_dep,
  install : false,
  cpp_args : build_args,
  include_directories : public_headers,
)
#==============================
# Project

project_dep = declare_dependency(
  include_directories: public_headers,
  link_with : project_shared_library
  )


#=================
# Unit Tests

if not meson.is_subproject()
    add_languages('cpp')
    subdir('tests')

    test('all_tests',
      executable(
        'run_tests',
        project_test_files,
        dependencies : [project_dep, test_dep],
        install: false,
        cpp_args: build_args,
        include_directories: public_headers
        )
      )
endif
