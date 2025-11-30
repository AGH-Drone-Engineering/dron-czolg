#pragma once
#include <stdint.h>
namespace bfs
{
    class SbusData
    {
    public:
        static const int NUM_CH = 16;
        int16_t ch[16] = {0};
    };
    class SbusRx
    {
    public:
        SbusData data;
        void Begin() {}
        bool Read() { return true; }
    };
}