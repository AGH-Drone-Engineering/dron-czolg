#pragma once
#include <stdint.h>

namespace bfs
{
    // Zmienne globalne do mockowania (ustawiane w testach)
    extern bool mock_sbus_read_return;
    extern bool mock_sbus_lost_frame;
    extern bool mock_sbus_failsafe;
    extern int16_t mock_sbus_ch[16]; // Dla ch[]

    class SbusData
    {
    public:
        static const int NUM_CH = 16;
        int16_t ch[16];
        bool lost_frame = false;
        bool failsafe = false;

        // Konstruktor domyślny
        SbusData()
        {
            for (int i = 0; i < NUM_CH; ++i)
                ch[i] = mock_sbus_ch[i];
            lost_frame = mock_sbus_lost_frame;
            failsafe = mock_sbus_failsafe;
        }
    };

    class SbusRx
    {
    private:
        SbusData data_; // Prywatny członek, jak w prawdziwej bibliotece

    public:
        void Begin() {}
        bool Read() { return mock_sbus_read_return; }
        inline const SbusData &data() const
        {
            // Aktualizuj data_ na podstawie mocków przy każdym wywołaniu
            for (int i = 0; i < SbusData::NUM_CH; ++i)
                const_cast<SbusData &>(data_).ch[i] = mock_sbus_ch[i];
            const_cast<SbusData &>(data_).lost_frame = mock_sbus_lost_frame;
            const_cast<SbusData &>(data_).failsafe = mock_sbus_failsafe;
            return data_;
        }
    };
}

namespace bfs
{
    bool mock_sbus_read_return = true;
    bool mock_sbus_lost_frame = false;
    bool mock_sbus_failsafe = false;
    int16_t mock_sbus_ch[16] = {0}; // Domyślnie 0
}