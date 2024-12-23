# Shared utilities
add_library(cynoshared STATIC shared.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/shared.hpp")
target_include_directories(cynoshared PUBLIC ../include) 

target_compile_options(cynoshared PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak>
)

# Lexer library
add_library(cynolexer STATIC lexer.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/lexer.hpp"
     "${PROJECT_SOURCE_DIR}/include/cynophobia/charstream.hpp")

# Parser library
add_library(cynoparser STATIC parser.cpp  
     "${PROJECT_SOURCE_DIR}/include/cynophobia/parser.hpp")


target_include_directories(cynolexer PUBLIC ../include) 
target_link_libraries(cynolexer cynoshared)

target_include_directories(cynoparser PUBLIC ../include) 
target_link_libraries(cynoparser cynoshared)

target_compile_features(cynolexer PUBLIC cxx_std_11)

target_compile_options(cynolexer PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak>
)

target_compile_features(cynoparser PUBLIC cxx_std_11)

target_compile_options(cynoparser PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak>
)

target_link_options(cynolexer PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak>
)

target_link_options(cynoparser PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -fsanitize=leak>
)
 