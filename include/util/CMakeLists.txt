add_library(lutil)

aux_source_directory(${INCLUDE_DIR}/util UTIL)

target_sources(lutil PRIVATE ${UTIL})