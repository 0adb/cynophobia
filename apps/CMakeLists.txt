add_executable(cynocompiler cynocompiler.cpp)
target_compile_features(cynocompiler PRIVATE cxx_std_11)

target_link_libraries(cynocompiler PRIVATE cynolexer)

add_custom_command(
    TARGET cynocompiler POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/driver.sh
        $<TARGET_FILE_DIR:cynocompiler>/driver.sh
)
