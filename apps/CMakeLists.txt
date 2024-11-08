add_executable(cynocompiler cynocompiler.cpp)
target_compile_features(cynocompiler PRIVATE cxx_std_11)

target_link_libraries(cynocompiler PRIVATE cynolexer fmt::fmt)
