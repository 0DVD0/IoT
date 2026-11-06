#pragma once
#include <stdint.h>
enum class TrafficColor : uint8_t { Red, Yellow, Green };
struct TrafficLight { uint8_t redPin, yellowPin, greenPin; };
void trafficLightInit(const TrafficLight &light);
// Caller coordinates both lights; inactive outputs are switched off first.
void trafficLightWrite(const TrafficLight &light, TrafficColor color);
