#pragma once
#include <distanceProtocol.h>
// Slave callbacks only copy a previously published frame; never block.
void distanceSlaveBegin();
void distanceSlavePublish(const DistanceProtocol::Sample &sample);
void distanceMasterBegin();
// 0: transaction completed; otherwise Wire error (5 also denotes timeout).
uint8_t distanceMasterRead(DistanceProtocol::Sample &sample, DistanceProtocol::Error &error);
