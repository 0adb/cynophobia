# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(cynotester lexertest.cpp)
 
target_compile_features(cynotester PRIVATE cxx_std_11)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(cynotester PRIVATE cynolexer Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME cynotest COMMAND cynotester) # Command can be a target
 
 