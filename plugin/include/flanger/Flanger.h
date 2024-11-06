#pragma once

#include <juce_dsp/juce_dsp.h>
#include <ranges>
#include "flanger/FractionalDelayLine.h"
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_assert.hpp>

namespace audio_plugin {
template <typename SampleType>
class Flanger {
public:
  struct Parameters {
    using Ptr = std::shared_ptr<Parameters>;

    wolfsound::Frequency lfoFrequency{0.1f};
  };

  Flanger() = default;
  explicit Flanger(Parameters::Ptr parametersToUse)
      : parameters_{parametersToUse} {}

  void prepare(const juce::dsp::ProcessSpec& spec) {
    constexpr auto MAX_DELAY_SECONDS = 0.002;
    maxDelay_ =
        static_cast<int>(std::ceil(spec.sampleRate * MAX_DELAY_SECONDS));
    middleDelay_ = maxDelay_ / SampleType(2);
    lfo_.prepare(spec);

    // reset the values to defaults
    setParameters(Parameters{}, true);
    reset();
  }

  void setParameters(const Parameters& newParameters, bool force = false) {
    if (newParameters.lfoFrequency !=
        wolfsound::Frequency{lfo_.getFrequency()}) {
      lfo_.setFrequency(newParameters.lfoFrequency.value(), force);
    }
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

    setParameters(*parameters_);

    // Process samples one by one, at least initially.
    using namespace std::views;
    constexpr auto CHANNEL = 0u;
    for (const auto sample : iota(0, static_cast<int>(sampleCount))) {
      const auto processedSample =
          processSample(inputBlock.getSample(CHANNEL, sample));
      outputBlock.setSample(CHANNEL, sample, processedSample);
    }
  }

  SampleType processSample(SampleType sample) {
    const auto& x = sample;
    const auto xh = x + feedback_ * delayLine_.popSample(middleDelay_);

    const auto lfoUnipolarValue = (lfo_.processSample(0) + 1) / 2;
    const auto currentDelay = lfoUnipolarValue * maxDelay_;

    const auto y =
        blend_ * xh + feedforward_ * delayLine_.popSample(currentDelay);

    delayLine_.pushSample(xh);

    return y;
  }

  void reset() {
    delayLine_.reset();
    lfo_.reset();
  }

private:
  SampleType feedforward_ = SampleType(0.7);
  SampleType feedback_ = SampleType(0.7);
  SampleType blend_ = SampleType(0.7);
  FractionalDelayLine delayLine_;
  juce::dsp::Oscillator<SampleType> lfo_{
      [](auto phase) { return std::sin(phase); }, 128u};
  int maxDelay_{};
  SampleType middleDelay_{};

  Parameters::Ptr parameters_{std::make_shared<Parameters>()};
};
}  // namespace audio_plugin
