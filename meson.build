project(
  'rascal',
  'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++2a']
)

add_project_arguments(['-stdlib=libc++'], language : 'cpp')
add_project_link_arguments(['-stdlib=libc++',], language : 'cpp')

boost_dep = dependency('boost')
# sfml_dep = dependency('sfml')
# freetype_dep = dependency('appleframeworks', modules : 'freetype')
# core_foundation_dep = dependency('appleframeworks', modules : 'CoreFoundation')

grab = run_command('tools/grabber.sh')
sources = grab.stdout().strip().split('\n')

build_rascal = build_target(
  'rascal',
  sources,
  # dependencies : [boost_dep, sfml_dep, freetype_dep, core_foundation_dep],
  install : true,
  cpp_args : ['-Wno-unused-parameter', '-I/usr/local/sys/usr/include', '-Wno-sign-compare', '-Werror'],
  link_args : ['-L/usr/local/lib', '-lboost_serialization', '-lsfml-window', '-lsfml-graphics', '-lsfml-system', '-lsfml-audio'],
  target_type : 'executable'
)

# add , '-mmacosx-version-min=10.12' to cpp_args when getting back to compatibility
build_rascal_for_bundle = build_target(
  'rascal_for_bundle',
  sources,
  # dependencies : [boost_dep, sfml_dep, freetype_dep, core_foundation_dep],
  install : true,
  install_rpath : '@executable_path/../Resources/Frameworks',
  cpp_args : ['-Wno-unused-parameter', '-I/usr/local/sys/usr/include', '-Wno-sign-compare', '-Werror', '-DBUNDLE_BUILD'],
  link_args : ['-lboost_serialization', '-lsfml-window', '-lsfml-graphics', '-lsfml-system', '-lsfml-audio'],
  target_type : 'executable'
)

run_target(
  'build',
  depends : build_rascal,
  command : ['cp', 'build/rascal', 'rascal'],
)

run_target(
  'bundle',
  depends : build_rascal_for_bundle,
  command : 'tools/make_bundle.sh'
)
