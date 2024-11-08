#include <gtest/gtest.h>
#include <flanger/PluginProcessor.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <wolfsound/dsp/wolfsound_testSignals.hpp>

namespace audio_plugin {
TEST(AudioProcessor, StereoTest) {
  using namespace wolfsound::literals;
  using namespace std::chrono_literals;

  // given
  AudioPluginAudioProcessor processor{};

  constexpr auto SAMPLES_PER_BLOCK = 441;
  constexpr auto SAMPLE_RATE = 44100_Hz;
  processor.prepareToPlay(static_cast<double>(SAMPLE_RATE.value()),
                          SAMPLES_PER_BLOCK);

  const auto saw =
      wolfsound::generateNonaliasingSawRampDown(440_Hz, SAMPLE_RATE, 1s);

  juce::AudioBuffer<float> buffer{2, SAMPLES_PER_BLOCK};
  buffer.copyFrom(0, 0, saw.data(), SAMPLES_PER_BLOCK);
  buffer.copyFrom(1, 0, saw.data(), SAMPLES_PER_BLOCK);

  juce::MidiBuffer empty{};

  // when
  processor.processBlock(buffer, empty);

  // then

  // ensure that some processing happened
  for (const auto i : std::views::iota(1, SAMPLES_PER_BLOCK)) {
    ASSERT_NE(buffer.getSample(0, i), saw.at(static_cast<size_t>(i)));
  }

  // ensure that both channels contain identical samples
  for (const auto i : std::views::iota(0, SAMPLES_PER_BLOCK)) {
    ASSERT_FLOAT_EQ(buffer.getSample(0, i), buffer.getSample(1, i));
  }
}
}  // namespace audio_plugin
