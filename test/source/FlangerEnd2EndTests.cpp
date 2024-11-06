#include <gtest/gtest.h>
#include <flanger/Flanger.h>
#include <wolfsound/test/wolfsound_ProcessorFileIoTest.hpp>

namespace audio_plugin {
TEST(FlangerTest, FileEnd2End) {
  const auto* testInfo =
      ::testing::UnitTest::GetInstance()->current_test_info();

  wolfsound::ProcessorFileIoTest<Flanger<float>> test{
      {.inputAudioFile = "saw200.0Hz5.0s.wav",
       .name = std::string{testInfo->test_case_name()} + "_" + testInfo->name(),
       .audioInputFilesDirectoryPath = AUDIO_INPUT_FILES_DIRECTORY}};
  test.run();
}
}  // namespace audio_plugin
