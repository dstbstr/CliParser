cmake_minimum_required (VERSION 3.16)
project("Cli Parser Parent")

set(ShouldTest CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
if(${ShouldTest})
    include(FetchContent)

    #################################
    # GTest
    #################################
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.15.2
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(CliParser)
if(${ShouldTest})
    add_subdirectory(CliParserTest)
endif()

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
