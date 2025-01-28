project(CliParserTest)

add_executable(${PROJECT_NAME} )

target_link_libraries(${PROJECT_NAME} gtest)
target_link_libraries(${PROJECT_NAME} gmock)
target_link_libraries(${PROJECT_NAME} CliParser)

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /EHsc>)

target_sources(${PROJECT_NAME} PRIVATE 
src/main.cpp
)
