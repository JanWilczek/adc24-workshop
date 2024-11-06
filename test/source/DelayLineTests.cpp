#include <gtest/gtest.h>
#include <juce_dsp/juce_dsp.h>
#include "flanger/FractionalDelayLine.h"
#include <ranges>

namespace audio_plugin {
TEST(DelayLine, PopOrderCorrespondsToPushOrder) {
  juce::dsp::DelayLine<float> delayLine;
  delayLine.prepare(juce::dsp::ProcessSpec{
      .sampleRate = 44100.0,
      .maximumBlockSize = 441,
      .numChannels = 1u,
  });
  delayLine.setMaximumDelayInSamples(10);

  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 2, false));

  delayLine.pushSample(0, 1.f);
  delayLine.pushSample(0, 2.f);
  delayLine.pushSample(0, 3.f);
  delayLine.pushSample(0, 4.f);
  delayLine.pushSample(0, 5.f);

  ASSERT_FLOAT_EQ(3.f, delayLine.popSample(0, 12, false));
  ASSERT_FLOAT_EQ(3.f, delayLine.popSample(0, 11, false));
  ASSERT_FLOAT_EQ(3.f, delayLine.popSample(0, 10, false));
  ASSERT_FLOAT_EQ(4.f, delayLine.popSample(0, 9, false));
  ASSERT_FLOAT_EQ(5.f, delayLine.popSample(0, 8, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 7, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 6, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 5, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 4, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 3, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 2, false));
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(0, 1, false));
}

TEST(FractionalDelayLine, PopOrderCorrespondsToPushOrder) {
  FractionalDelayLine delayLine;

  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(2));

  delayLine.pushSample(1.f);
  delayLine.pushSample(2.f);
  delayLine.pushSample(3.f);
  delayLine.pushSample(4.f);

  delayLine.setDelay(100);
  ASSERT_FLOAT_EQ(0, delayLine.popSample());

  delayLine.setDelay(1.5f);
  ASSERT_FLOAT_EQ(2.5f, delayLine.popSample());

  delayLine.setDelay(2.3f);
  ASSERT_FLOAT_EQ(1.7f, delayLine.popSample());

  delayLine.setDelay(3.3f);
  ASSERT_NEAR(0.7f, delayLine.popSample(), 0.001f);

  delayLine.setDelay(2.f);
  ASSERT_FLOAT_EQ(2.f, delayLine.popSample());

  ASSERT_FLOAT_EQ(1.f, delayLine.popSample(3));
  ASSERT_FLOAT_EQ(2.f, delayLine.popSample(2));
  ASSERT_FLOAT_EQ(3.f, delayLine.popSample(1));
  ASSERT_FLOAT_EQ(4.f, delayLine.popSample(0));
}

TEST(FractionalDelayLine, IsResilientAgainstNumericalUnderflow) {
  FractionalDelayLine delayLine;

  for (const auto i : std::views::iota(0, 50)) {
    delayLine.pushSample(static_cast<float>(i));
  }

  // This delay value caused an underflow and resulted in readHead
  // equal to 48000 (equal to delay line length).
  // Since reading beyond delay line length is undefined behavior,
  // FractionalDelayLine should avoid it at all costs.
  ASSERT_FLOAT_EQ(0.f, delayLine.popSample(49.000732421875f));
}
}  // namespace audio_plugin
