add_executable(cynocompiler cynocompiler.cpp)
target_compile_features(cynocompiler PRIVATE cxx_std_11)

target_link_libraries(cynocompiler PRIVATE cynolexer)

add_custom_command(
    TARGET cynocompiler PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/driver.sh
        ${CMAKE_CURRENT_BINARY_DIR}/driver.sh
)

set(DRIVER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/driver.sh")
set(DRIVER_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(
    OUTPUT ${DRIVER_DESTINATION}driver.sh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DRIVER_SOURCE} ${DRIVER_DESTINATION}driver.sh
    COMMAND chmod u+x ${DRIVER_DESTINATION}driver.sh
    DEPENDS ${DRIVER_SOURCE}  # The command depends on the source file 
)

add_custom_target(
    update_driver
    DEPENDS ${DRIVER_DESTINATION}driver.sh
)
   