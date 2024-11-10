#pragma once
#include <array>

namespace audio_plugin {

class FractionalDelayLine {
public:
  [[nodiscard]] float popSample() const;
  [[nodiscard]] float popSample(float delay) const;
  void pushSample(float inputSample);
  void setDelay(float newDelay);
  void reset();

private:
  static constexpr auto DELAY_LINE_LENGTH = 48000u;

  [[nodiscard]] float clampDelay(float delay) const noexcept;

  std::array<float, DELAY_LINE_LENGTH> buffer_{};

  float delay_ = 4.f;
  int writeHead_ = 0;
};
}  // namespace audio_plugin
