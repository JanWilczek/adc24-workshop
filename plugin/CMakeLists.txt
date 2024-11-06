cmake_minimum_required(VERSION 3.22)

# Version is needed by JUCE.
project(Flanger VERSION 0.1.0)

# Adds a plugin target (that's basically what the Projucer does).
juce_add_plugin(
  ${PROJECT_NAME}
  COMPANY_NAME
  MyCompany # change this
  IS_SYNTH
  FALSE
  NEEDS_MIDI_INPUT
  FALSE
  NEEDS_MIDI_OUTPUT
  FALSE
  PLUGIN_MANUFACTURER_CODE
  MCMP # change this
  PLUGIN_CODE
  WRFL
  FORMATS
  Standalone
  VST3
  AU
  PRODUCT_NAME
  "Workshop Flanger"
  COPY_PLUGIN_AFTER_BUILD
  TRUE
)

set(SOURCES source/PluginProcessor.cpp source/FractionalDelayLine.cpp)

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

set_source_files_properties(${SOURCES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries_system(
  ${PROJECT_NAME}
  PRIVATE
  juce::juce_audio_utils
  PUBLIC
  juce::juce_dsp
)
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE wolfsound::wolfsound_dsp_utils
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
