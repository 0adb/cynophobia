add_executable(cynocompiler cynocompiler.cpp)
target_compile_features(cynocompiler PRIVATE cxx_std_11)

target_link_libraries(cynocompiler PRIVATE cynolexer)

set(DRIVER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/driver.sh")
set(DRIVER_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")


add_custom_command(
    OUTPUT ${DRIVER_DESTINATION}driver.sh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DRIVER_SOURCE} ${DRIVER_DESTINATION}driver.sh
    DEPENDS ${DRIVER_SOURCE}  # The command depends on the source file
    COMMENT "Copying driver source to destination"
)
 
add_custom_target(
    update_driver
    DEPENDS ${DRIVER_DESTINATION}driver.sh
)

add_custom_command(
    TARGET update_driver
    COMMAND chmod u+x ${DRIVER_DESTINATION}driver.sh
    DEPENDS ${DRIVER_DESTINATION}driver.sh 
)

target_compile_options(cynocompiler PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)