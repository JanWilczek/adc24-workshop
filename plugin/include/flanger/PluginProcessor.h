#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "flanger/Flanger.h"

namespace audio_plugin {
class AudioPluginAudioProcessor : public juce::AudioProcessor {
public:
  using SampleType = float;

  explicit AudioPluginAudioProcessor(
      juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout);
  ~AudioPluginAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<SampleType>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

private:
  using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
  using ParameterGroup = juce::AudioProcessorParameterGroup;

  struct Parameters {
    explicit Parameters(ParameterLayout&);
    juce::AudioParameterFloat& lfoFrequency;  // NOLINT
  };

  Parameters parameters_;
  juce::AudioProcessorValueTreeState apvts_;
  juce::dsp::ProcessorDuplicator<Flanger<SampleType>,
                                 Flanger<SampleType>::Parameters>
      flanger_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
}  // namespace audio_plugin
