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
  writeHead_ %= static_cast<int>(std::ssize(buffer_));
}

void FractionalDelayLine::setDelay(float newDelay) {
  WS_PRECONDITION(newDelay < static_cast<float>(DELAY_LINE_LENGTH));
  delay_ = clampDelay(newDelay);
}

float FractionalDelayLine::popSample(float delay) const {
  constexpr auto DELAY_LINE_LENGTH_FLOAT = static_cast<float>(DELAY_LINE_LENGTH);

  WS_PRECONDITION(delay >= 0.f);
  WS_PRECONDITION(delay < DELAY_LINE_LENGTH_FLOAT);

  auto readHead = static_cast<float>(writeHead_) - 1 - delay;
  if (readHead < 0.f) {
    readHead += DELAY_LINE_LENGTH_FLOAT;
  }

  auto truncatedReadHead = std::floor(clampDelay(readHead));
  auto truncatedReadHeadPlusOne = truncatedReadHead + 1.f;

  const auto truncatedReadHeadWeight =
      std::abs(readHead - truncatedReadHeadPlusOne);
  const auto truncatedReadHeadPlusOneWeight =
      std::abs(readHead - truncatedReadHead);
  if (truncatedReadHeadPlusOne >= DELAY_LINE_LENGTH_FLOAT) {
    truncatedReadHeadPlusOne -= DELAY_LINE_LENGTH_FLOAT;
  }

  WS_ASSERT(0 <= truncatedReadHead, "invalid implementation");
  WS_ASSERT(0 <= truncatedReadHeadPlusOne, "invalid implementation");
  WS_ASSERT(truncatedReadHead < DELAY_LINE_LENGTH_FLOAT, "invalid implementation");
  WS_ASSERT(truncatedReadHeadPlusOne < DELAY_LINE_LENGTH_FLOAT,
            "invalid implementation");

  const auto outputSample =
      (truncatedReadHeadWeight *
          buffer_[static_cast<size_t>(truncatedReadHead)]) +
      (truncatedReadHeadPlusOneWeight *
          buffer_[static_cast<size_t>(truncatedReadHeadPlusOne)]);
  return outputSample;
}

void FractionalDelayLine::reset() {
  std::ranges::fill(buffer_, 0.f);
}

float FractionalDelayLine::clampDelay(float delay) const noexcept {
  return std::clamp(delay, 0.f, static_cast<float>(DELAY_LINE_LENGTH - 1u));
}
}  // namespace audio_plugin
