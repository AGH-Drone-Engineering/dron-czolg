#include "sbus_receiver.h"

bfs::SbusRx sbus_rx(&Serial1);  // Use Serial1 for SBUS input
bfs::SbusData sbus_data;

namespace {
constexpr int SBUS_MIN = 172;
constexpr int SBUS_MAX = 1811;
constexpr int SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2;
constexpr float SBUS_HALF_RANGE = (SBUS_MAX - SBUS_MIN) / 2.0f;
}  // namespace

bool ReadSbus() {
    if (sbus_rx.Read()) {
        sbus_data = sbus_rx.data();
        return true;
    }
    return false;
}

// Normalize channel value to -1.0 to 1.0 range
// There could be problem with bias, because values are asymmetric around center
float NormalizeChannel(int16_t channel) {
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH) {
        Serial.println("Invalid SBUS channel: " + String(channel));
        return 0.0f;  // Safety
    }

    int raw_val = sbus_data.ch[channel];
    if (raw_val < SBUS_MIN) raw_val = SBUS_MIN;
    if (raw_val > SBUS_MAX) raw_val = SBUS_MAX;

    return (static_cast<float>(raw_val) - static_cast<float>(SBUS_CENTER)) / SBUS_HALF_RANGE;
}
