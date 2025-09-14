#include "SBUS.h"

// Use Serial1 if your board has multiple UARTs (Mega, ESP32, Teensy).
// On Uno/Nano, you'll need SoftwareSerial with an inverter circuit.
SBUS sbus(Serial1);

int16_t channels[18]; // stores up to 18 channels
bool failsafe = false;
bool lostFrame = false;

void setup()
{
    Serial.begin(115200); // USB Serial (for printing to PC)
    sbus.begin();         // Start SBUS (100000 baud, 8E2)
    Serial.println("SBUS Reader Started");
}

void loop()
{
    // Try to read one SBUS frame (25 bytes)
    if (sbus.read())
    {
        sbus.getNormalizedChannels(channels, failsafe, lostFrame);

        // Print all 16 channels
        Serial.print("Channels: ");
        for (int i = 0; i < 16; i++)
        {
            Serial.print("CH");
            Serial.print(i + 1);
            Serial.print("=");
            Serial.print(channels[i]);
            Serial.print("  ");
        }

        // Print failsafe / lost frame flags
        if (failsafe)
            Serial.print("  FAILSAFE!");
        if (lostFrame)
            Serial.print("  LOST FRAME!");

        Serial.println();
    }
}
