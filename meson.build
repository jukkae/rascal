project(
  'rascal',
  'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++1z']
)

boost_dep = dependency('boost')
sfml_dep = dependency('sfml')
core_foundation_dep = dependency('CoreFoundation')

c = run_command('grabber.sh')
sources = c.stdout().strip().split('\n')

executable(
  'rascal',
  sources,
  dependencies : [boost_dep, sfml_dep, core_foundation_dep],
  install : true,
  cpp_args : ['-Wno-unused-parameter', '-I/usr/local/sys/usr/include', '-Wno-sign-compare'],
  link_args : ['-L/usr/local/lib', '-lc++', '-lboost_serialization', '-lsfml-window', '-lsfml-graphics', '-lsfml-system', '-lsfml-audio']
)

run_command('cp', 'build/rascal', 'rascal')
