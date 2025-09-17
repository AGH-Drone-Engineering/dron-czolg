#include "sbus.h"

/* SBUS object, reading SBUS */
bfs::SbusRx sbus_rx(&Serial1); // Use Serial1 for SBUS input
/* SBUS data struct */
bfs::SbusData data;

void setup()
{
    // USB serial for debug printing
    Serial.begin(115200);
    while (!Serial)
    {
    }

    // Start SBUS RX
    sbus_rx.Begin();

    Serial.println("SBUS Reader Started");
}

void loop()
{
    // Try to read a new SBUS frame
    if (sbus_rx.Read())
    {
        // Grab the received data
        data = sbus_rx.data();

        // Print all 16 channels
        Serial.print("Channels: ");
        for (int i = 0; i < data.NUM_CH; i++)
        {
            Serial.print("CH");
            Serial.print(i + 1);
            Serial.print("=");
            Serial.print(data.ch[i]);
            Serial.print("\t");
        }

        // Print failsafe / lost frame flags
        Serial.print("LostFrame=");
        Serial.print(data.lost_frame);
        Serial.print("\tFailsafe=");
        Serial.println(data.failsafe);
    }
}
