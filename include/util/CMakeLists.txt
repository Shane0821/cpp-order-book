add_library(lutil)

aux_source_directory(${INCLUDE_DIR}/util UTIL)

target_sources(lutil PRIVATE ${UTIL})

target_link_libraries(lutil pthread fmt::fmt)