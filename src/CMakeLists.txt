# Make an automatic library - will be static or dynamic based on user setting
add_library(cynolexer STATIC lexer.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/lexer.hpp" 
     "${PROJECT_SOURCE_DIR}/include/cynophobia/shared.hpp")

# We need this directory, and users of our library will need it too
target_include_directories(cynolexer PUBLIC ../include) 

# All users of this library will need at least C++11
target_compile_features(cynolexer PUBLIC cxx_std_11)

