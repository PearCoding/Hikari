SET(GEN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/spec.json)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/hikari.h
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/header_generator.py ${GEN_SRC} ${CMAKE_BINARY_DIR}/hikari.h
    DEPENDS ${GEN_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/header_generator.py
    COMMENT "Generating header files"
)

add_custom_target(header_files DEPENDS ${CMAKE_BINARY_DIR}/hikari.h ${GEN_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/header_generator.py)
