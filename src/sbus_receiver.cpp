#include "sbus_receiver.h"

bfs::SbusRx sbus_rx(&Serial1);  // Use Serial1 for SBUS input
bfs::SbusData sbus_data;

namespace {
constexpr int SBUS_MIN = 172;
constexpr int SBUS_MAX = 1811;
constexpr int SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2;
}  // namespace

bool ReadSbus() {
    if (sbus_rx.Read()) {
        sbus_data = sbus_rx.data();
        return true;
    }
    return false;
}

// Normalize channel value to -1.0 to 1.0 range
float NormalizeChannel(int16_t channel) {
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH) {
        return 0.0f;  // Safety
    }

    int raw_val = sbus_data.ch[channel];

    return static_cast<float>(raw_val - SBUS_CENTER) / static_cast<float>((SBUS_MAX - SBUS_MIN) / 2.0f);
}
