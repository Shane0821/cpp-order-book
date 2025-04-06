add_library(lbook)

aux_source_directory(${SRC_DIR}/book BOOK)

target_sources(lbook PRIVATE ${BOOK})

target_link_libraries(lbook lutil)