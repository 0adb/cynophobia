add_executable(cynocompiler cynocompiler.cpp)
target_compile_features(cynocompiler PRIVATE cxx_std_11)

target_link_libraries(cynocompiler PRIVATE cynolexer fmt::fmt)

add_custom_command(
    TARGET cynocompiler POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/driver.sh
        ${CMAKE_CURRENT_BINARY_DIR}/driver.sh)
