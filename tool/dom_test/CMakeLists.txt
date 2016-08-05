add_custom_command(
  OUTPUT ${dom_test_dir}/index.js
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tool/dom_test ${dom_test_dir}
)

add_custom_command(
  OUTPUT ${dom_test_dir}/node_modules
  COMMAND npm install --silent
  WORKING_DIRECTORY ${dom_test_dir}
  COMMENT "Resolve npm deps for dom_test."
)

add_custom_target(tool.dom_test
  DEPENDS
    ${dom_test_dir}/index.js
    ${dom_test_dir}/node_modules
)

