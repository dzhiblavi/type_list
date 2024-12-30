add_library(type_list INTERFACE)

target_include_directories(
  type_list INTERFACE $<BUILD_INTERFACE:${TYPE_LIST_SOURCE_DIR}/include/>)
target_include_directories(
  type_list SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)
