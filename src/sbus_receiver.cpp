#include "sbus_receiver.h"

bfs::SbusRx sbus_rx(&Serial1);  // Use Serial1 for SBUS input
bfs::SbusData sbus_data;