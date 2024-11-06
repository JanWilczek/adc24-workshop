cmake_minimum_required(VERSION 3.22)

project(AudioPluginTest)

# Signals to CMake that we want to run tests from this directory.
enable_testing()

set(SOURCES source/AudioProcessorTest.cpp source/DelayLineTests.cpp source/FlangerEnd2EndTests.cpp)

# Creates the test console application.
add_executable(${PROJECT_NAME} ${SOURCES})

# Sets the necessary include directories: ours, JUCE's, and googletest's.
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../plugin/include ${JUCE_SOURCE_DIR}/modules
                          ${GOOGLETEST_SOURCE_DIR}/googletest/include
)

set_source_files_properties(${SOURCES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

target_compile_definitions(${PROJECT_NAME} PRIVATE AUDIO_INPUT_FILES_DIRECTORY="${CMAKE_SOURCE_DIR}/data")

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(${PROJECT_NAME} PRIVATE Flanger wolfsound::wolfsound_dsp_utils)
target_link_libraries_system(${PROJECT_NAME} PRIVATE GTest::gtest_main)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Adds googletest-specific CMake commands at our disposal.
include(GoogleTest)
# Add all tests defined with googletest to the CMake metadata so that these tests
# are run upon a call to ctest in the test projects' binary directory.
if(CMAKE_GENERATOR STREQUAL Xcode)
  # On macOS arm64, all binaries have to be signed before running.
  # In local development, the linker adds an ad-hoc placeholder signature.
  # In Xcode however, the ad-hoc signature is delayed until after the “Run Script” build phase,
  # so the POST_BUILD command added by gtest_discover_tests cannot run.
  # Thus, we need to delay test discovery until run time.
  # Source: https://discourse.cmake.org/t/googletest-crash-when-using-cmake-xcode-arm64/5766/8
  gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
  gtest_discover_tests(${PROJECT_NAME})
endif()
