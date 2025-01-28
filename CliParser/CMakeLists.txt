project(CliParser)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /EHsc>)

target_sources(${PROJECT_NAME} PRIVATE
)