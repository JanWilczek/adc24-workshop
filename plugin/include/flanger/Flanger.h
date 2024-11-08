#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <ranges>
#include "flanger/FractionalDelayLine.h"
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_assert.hpp>

namespace audio_plugin {
template <typename SampleType>
class Flanger {
public:
  void prepare(const juce::dsp::ProcessSpec& spec) {
    juce::ignoreUnused(spec);

    reset();
  }

  template <class ProcessContext>
  void process(const ProcessContext& processContext) {
    const auto& inputBlock = processContext.getInputBlock();
    const auto& outputBlock = processContext.getOutputBlock();
    const auto channelCount = outputBlock.getNumChannels();
    const auto sampleCount = outputBlock.getNumSamples();

    // These sanity checks save you a headache later on in the development.
    jassert(inputBlock.getNumChannels() == channelCount);
    jassert(inputBlock.getNumSamples() == sampleCount);

    // Implementing mono first? Assert it!
    constexpr auto SUPPORTED_CHANNELS = 1u;
    jassert(channelCount == SUPPORTED_CHANNELS);

    // Always check if the processor is not bypassed
    if (processContext.isBypassed) {
      outputBlock.copyFrom(inputBlock);
      return;
    }

    // Process samples one by one, at least initially.
    using namespace std::views;
    constexpr auto CHANNEL = 0u;
    for (const auto sample : iota(0, static_cast<int>(sampleCount))) {
      const auto processedSample =
          processSample(inputBlock.getSample(CHANNEL, sample));
      outputBlock.setSample(CHANNEL, sample, processedSample);
    }
  }

  SampleType processSample(SampleType sample) { return sample; }

  void reset() {}

private:
};
}  // namespace audio_plugin
