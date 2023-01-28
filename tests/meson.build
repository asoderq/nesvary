gtest_dir = 'googletest/googletest'
gmock_dir = 'googletest/googlemock'
gtest_incdir = include_directories(join_paths(gtest_dir, 'include'), is_system: true)
gmock_incdir = include_directories(join_paths(gmock_dir, 'include'), is_system: true)

libgtest = static_library(
  'gtest',
  cpp_args : ['-w'],
  include_directories : [include_directories(gtest_dir), gtest_incdir],
  sources : [
    join_paths(gtest_dir, 'src', 'gtest-all.cc'),
    join_paths(gtest_dir, 'src', 'gtest_main.cc')
  ]
)

libgmock = static_library(
  'gmock',
  cpp_args : ['-w'],
  include_directories : [include_directories(gmock_dir), gmock_incdir] + 
                        [include_directories(gtest_dir), gtest_incdir],
  sources : [
    join_paths(gmock_dir, 'src', 'gmock-all.cc'),
    join_paths(gmock_dir, 'src', 'gmock_main.cc')
  ],
  link_with: libgtest
)

test_dep = declare_dependency(
  dependencies : dependency('threads'),
  include_directories : [gtest_incdir, gmock_incdir],
  link_with : [libgmock]
)

