#include "sbus_reader.h"

Sbus_reader::Sbus_reader() : sbus_rx(&SBUS_INPUT) {}

void Sbus_reader::init()
{
    sbus_rx.Begin();
    // Serial.println("SBUS Reader Initialized");
}

float Sbus_reader::NormalizeChannel(int16_t channel)
{
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH)
    {
        // Serial.print("Error: NormalizeChannel called with invalid channel index: ");
        // Serial.println(channel);
        return 0; // Safety
    }

    int raw_val = sbus_data.ch[channel];
    return map(raw_val, SBUS_MIN, SBUS_MAX, 48, 2047);
}

float Sbus_reader::isChannelActive(int16_t channel)
{
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH)
    {
        // Serial.print("Error: isChannelActive called with invalid channel index: ");
        // Serial.println(channel);
        return 0.0; // Safety
    }

    int raw_val = sbus_data.ch[channel];
    return raw_val > SBUS_CENTER ? 1.0 : 0.0;
}

float Sbus_reader::Read_Sbus()
{
    if (sbus_rx.Read())
    {
        sbus_data = sbus_rx.data();

        if (sbus_data.failsafe)
            return 2;
        if (sbus_data.lost_frame)
            return 1;

        data[0] = NormalizeChannel(RC_THROTTLE_CH); // Throttle
        data[1] = NormalizeChannel(RC_YAW_CH);      // Yaw
        data[2] = NormalizeChannel(RC_PITCH_CH);    // Pitch
        data[3] = NormalizeChannel(RC_ROLL_CH);     // Roll
        data[4] = isChannelActive(RC_ARM_CH);       // Arm status
        data[5] = isChannelActive(RC_MODE_CH);      // Mode

        return 0.0;
    }

    return 1.0;
}

void Sbus_reader::print_data()
{
    Serial.print("Throttle: ");
    Serial.print(data[0], 2);
    Serial.print("\nYaw: ");
    Serial.print(data[1], 2);
    Serial.print("\nPitch: ");
    Serial.print(data[2], 2);
    Serial.print("\nRoll: ");
    Serial.println(data[3], 2);
    Serial.print("\nArm Status: ");
    Serial.println(data[4], 2);
    Serial.print("\nMode: ");
    Serial.println(data[5], 2);
}

// idk why but channels are different than normally
float Sbus_reader::get_throttle() { return data[0]; }
float Sbus_reader::get_yaw() { return data[1]; }
float Sbus_reader::get_pitch() { return data[2]; }
float Sbus_reader::get_roll() { return data[3]; }
float Sbus_reader::get_arm_status() { return data[4]; }
float Sbus_reader::get_mode() { return data[5]; }
float *Sbus_reader::get_data() { return data; }
float Sbus_reader::get_status() { return status; }