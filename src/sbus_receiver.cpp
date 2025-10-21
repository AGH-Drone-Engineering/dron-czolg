#include "sbus_receiver.h"

Sbus_reader::Sbus_reader() : sbus_rx(&SBUS_INPUT) {}

void Sbus_reader::init()
{
    sbus_rx.Begin();
    Serial.println("SBUS Reader Initialized");
}

float Sbus_reader::NormalizeChannel(int16_t channel)
{
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH)
    {
        Serial.print("Error: NormalizeChannel called with invalid channel index: ");
        Serial.println(channel);
        Serial.print("Invalid SBUS channel: ");
        Serial.println(channel);
        return 0.0f; // Safety
        return 0.0f; // Safety
    }

    int raw_val = sbus_data.ch[channel];
    if (raw_val < SBUS_MIN)
        raw_val = SBUS_MIN;
    if (raw_val > SBUS_MAX)
        raw_val = SBUS_MAX;
    if (raw_val < SBUS_MIN)
        raw_val = SBUS_MIN;
    if (raw_val > SBUS_MAX)
        raw_val = SBUS_MAX;

    return (static_cast<float>(raw_val) - static_cast<float>(SBUS_CENTER)) / SBUS_HALF_RANGE;
}

int Sbus_reader::Read_Sbus()
{
    if (sbus_rx.Read())
    {
        sbus_data = sbus_rx.data();

        if (sbus_data.failsafe)
            return 2;
        if (sbus_data.lost_frame)
            return 1;

        data[0] = NormalizeChannel(RC_THROTTLE_CH);
        data[1] = NormalizeChannel(RC_STEER_CH);
        data[2] = NormalizeChannel(RC_PITCH_CH);
        data[3] = NormalizeChannel(RC_ROLL_CH);
        data[4] = sbus_data.ch[RC_MODE_CH];

        return 0;
    }

    return 1;
}

void Sbus_reader::print_data()
{
    Serial.print("Throttle: ");
    Serial.print(data[0], 2);
    Serial.print("\tSteer: ");
    Serial.print(data[1], 2);
    Serial.print("\tPitch: ");
    Serial.print(data[2], 2);
    Serial.print("\tRoll: ");
    Serial.println(data[3], 2);
    Serial.print("\tMode: ");
    Serial.println(data[4], 2);
}

float Sbus_reader::get_throttle() { return data[0]; }
float Sbus_reader::get_steer() { return data[1]; }
float Sbus_reader::get_pitch() { return data[2]; }
float Sbus_reader::get_roll() { return data[3]; }
float Sbus_reader::get_mode() { return data[4]; }
float *Sbus_reader::get_data() { return data; }
int Sbus_reader::get_status() { return status; }
int Sbus_reader::Read_Sbus()
{
    if (sbus_rx.Read())
    {
        sbus_data = sbus_rx.data();

        if (sbus_data.failsafe)
            return 2;
        if (sbus_data.lost_frame)
            return 1;

        data[0] = NormalizeChannel(RC_THROTTLE_CH);
        data[1] = NormalizeChannel(RC_STEER_CH);
        data[2] = NormalizeChannel(RC_PITCH_CH);
        data[3] = NormalizeChannel(RC_ROLL_CH);
        data[4] = sbus_data.ch[RC_MODE_CH];

        return 0;
    }

    return 1;
}

void Sbus_reader::print_data()
{
    Serial.print("Throttle: ");
    Serial.print(data[0], 2);
    Serial.print("\tSteer: ");
    Serial.print(data[1], 2);
    Serial.print("\tPitch: ");
    Serial.print(data[2], 2);
    Serial.print("\tRoll: ");
    Serial.println(data[3], 2);
    Serial.print("\tMode: ");
    Serial.println(data[4], 2);
}

float Sbus_reader::get_throttle() { return data[0]; }
float Sbus_reader::get_steer() { return data[1]; }
float Sbus_reader::get_pitch() { return data[2]; }
float Sbus_reader::get_roll() { return data[3]; }
float Sbus_reader::get_mode() { return data[4]; }
float *Sbus_reader::get_data() { return data; }
int Sbus_reader::get_status() { return status; }