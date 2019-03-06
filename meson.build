project('rascal', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++1z'])

boost_dep = dependency('boost')
sfml_dep = dependency('sfml')

c = run_command('grabber.sh')
sources = c.stdout().strip().split('\n')

executable('rascal',
  sources,
  dependencies : [boost_dep, sfml_dep],
  install : true,
  cpp_args : '-Wno-unused-parameter')
