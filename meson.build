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

inc = include_directories('src')

schemas_dir = meson.current_source_dir() / 'schemas'
satellite_common_lib = static_library(
  'satellite_common',
  files(
    'src/satellite/json_io.cpp',
    'src/satellite/manifest.cpp',
    'src/satellite/result_envelope.cpp',
    'src/satellite/schema_validate.cpp',
    'src/satellite/cli_options.cpp',
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
    'src/satellite/json_io.hpp',
    'src/satellite/exit_codes.hpp',
    'src/satellite/manifest.hpp',
    'src/satellite/result_envelope.hpp',
    'src/satellite/schema_validate.hpp',
    'src/satellite/cli_options.hpp',
    subdir: 'satellite',
  )


  install_data(
    'schemas/plugin_manifest.1.1.schema.json',
    'schemas/task_envelope.1.1.schema.json',
    'schemas/task_result.1.1.schema.json',
    'schemas/schema_registry.json',
    install_dir: get_option('datadir') / 'satellite-plugin-sdk/schemas',
  )

  install_data(
    'schemas/archive/1.0/plugin_manifest.schema.json',
    'schemas/archive/1.0/task_envelope.schema.json',
    'schemas/archive/1.0/task_result.schema.json',
    install_dir: get_option('datadir') / 'satellite-plugin-sdk/schemas/archive/1.0',
  )

  install_data(
    'schemas/common/geo_point.schema.json',
    'schemas/common/origin.schema.json',
    'schemas/common/binding.schema.json',
    'schemas/common/schedule.schema.json',
    'schemas/common/routing.schema.json',
    'schemas/common/time_range.schema.json',
    'schemas/common/receiver.schema.json',
    'schemas/common/mission_params.schema.json',
    'schemas/common/envelope_header.1.1.schema.json',
    install_dir: get_option('datadir') / 'satellite-plugin-sdk/schemas/common',
  )

  install_data(
    'schemas/canonical/time_window.schema.json',
    'schemas/canonical/window_set.schema.json',
    'schemas/canonical/data_product.schema.json',
    'schemas/canonical/delivery_receipt.schema.json',
    'schemas/canonical/image_meta.schema.json',
    'schemas/canonical/image_meta_set.schema.json',
    'schemas/canonical/cost_score.schema.json',
    'schemas/canonical/policy_decision.schema.json',
    install_dir: get_option('datadir') / 'satellite-plugin-sdk/schemas/canonical',
  )
endif
