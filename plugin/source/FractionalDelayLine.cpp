#include <algorithm>
#include <array>
#include <vector>
#include <cmath>
#include "flanger/FractionalDelayLine.h"
#include <wolfsound/common/wolfsound_assert.hpp>

using namespace std;

namespace audio_plugin {
float FractionalDelayLine::popSample() const {
  return popSample(delay_);
}

void FractionalDelayLine::pushSample(float inputSample) {
  buffer_[static_cast<size_t>(writeHead_)] = inputSample;
  writeHead_++;
  writeHead_ %= std::ssize(buffer_);
}

void FractionalDelayLine::setDelay(float newDelay) {
  delay_ = newDelay;
}

float FractionalDelayLine::popSample(float delay) const {
  constexpr auto MAX_DELAY_FLOAT = static_cast<float>(MAX_DELAY);

  WS_PRECONDITION(delay >= 0.f);
  WS_PRECONDITION(delay < MAX_DELAY_FLOAT);

  auto readHead = static_cast<float>(writeHead_) - 1 - delay;
  if (readHead < 0.f) {
    readHead += MAX_DELAY_FLOAT;
  }

  auto truncatedReadHead =
      std::floor(std::clamp(readHead, 0.f, MAX_DELAY_FLOAT - 1.f));
  auto truncatedReadHeadPlusOne = truncatedReadHead + 1.f;

  const auto truncatedReadHeadWeight =
      std::abs(readHead - truncatedReadHeadPlusOne);
  const auto truncatedReadHeadPlusOneWeight =
      std::abs(readHead - truncatedReadHead);
  if (truncatedReadHeadPlusOne >= MAX_DELAY_FLOAT) {
    truncatedReadHeadPlusOne -= MAX_DELAY_FLOAT;
  }

  WS_ASSERT(0 <= truncatedReadHead, "invalid implementation");
  WS_ASSERT(0 <= truncatedReadHeadPlusOne, "invalid implementation");
  WS_ASSERT(truncatedReadHead < MAX_DELAY_FLOAT, "invalid implementation");
  WS_ASSERT(truncatedReadHeadPlusOne < MAX_DELAY_FLOAT,
            "invalid implementation");

  const auto outputSample =
      truncatedReadHeadWeight *
          buffer_[static_cast<size_t>(truncatedReadHead)] +
      truncatedReadHeadPlusOneWeight *
          buffer_[static_cast<size_t>(truncatedReadHeadPlusOne)];
  return outputSample;
}

void FractionalDelayLine::reset() {
  std::ranges::fill(buffer_, 0.f);
}
}  // namespace audio_plugin
