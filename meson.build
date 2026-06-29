project('satellite-plugin-sdk', 'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
    'default_library=static',
  ],
)

install_sdk = get_option('install_sdk') and not meson.is_subproject()

nlohmann_json_sp = subproject('nlohmann_json')
json_dep = nlohmann_json_sp.get_variable('nlohmann_json_dep')

inc = include_directories('include')

schemas_dir = meson.current_source_dir() / 'schemas'
satellite_common_lib = static_library(
  'satellite_common',
  files(
    'src/json_io.cpp',
    'src/manifest.cpp',
    'src/result_envelope.cpp',
    'src/schema_validate.cpp',
    'src/cli_options.cpp',
  ),
  cpp_args: ['-DSATELLITE_SDK_SCHEMAS_DIR="@0@"'.format(schemas_dir)],
  include_directories: inc,
  dependencies: [json_dep],
  install: install_sdk,
)

satellite_common_dep = declare_dependency(
  link_with: satellite_common_lib,
  include_directories: inc,
  dependencies: [json_dep],
)

if install_sdk
  install_headers(
    'include/satellite/json_io.hpp',
    'include/satellite/exit_codes.hpp',
    'include/satellite/manifest.hpp',
    'include/satellite/result_envelope.hpp',
    'include/satellite/schema_validate.hpp',
    'include/satellite/cli_options.hpp',
    subdir: 'satellite',
  )

  install_headers(
    'include/mp/common/json_io.hpp',
    'include/mp/common/exit_codes.hpp',
    subdir: 'mp/common',
  )

  install_data(
    'schemas/plugin_manifest.schema.json',
    'schemas/task_envelope.schema.json',
    'schemas/task_result.schema.json',
    install_dir: get_option('datadir') / 'satellite-plugin-sdk/schemas',
  )
endif
