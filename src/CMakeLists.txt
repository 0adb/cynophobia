# Shared utilities
add_library(cynoshared STATIC shared.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/shared.hpp")
target_include_directories(cynoshared PUBLIC ../include) 

# Lexer library
add_library(cynolexer STATIC lexer.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/lexer.hpp")
 
target_include_directories(cynolexer PUBLIC ../include) 
target_link_libraries(cynolexer cynoshared)

target_compile_features(cynolexer PUBLIC cxx_std_11)

