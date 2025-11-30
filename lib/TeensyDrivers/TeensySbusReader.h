#include <Arduino.h>
#include "ISbusReader.h"
#include "SBUS.h" // Prawdziwa biblioteka

class TeensySbusReader : public ISbusReader
{
    bfs::SbusRx sbus;

public:
    TeensySbusReader(HardwareSerial &serial) : sbus(&serial) { sbus.Begin(); }
    bool readChannels() override { return sbus.Read(); }
    int16_t getChannel(int ch) const override { return sbus.data().ch[ch]; }
};