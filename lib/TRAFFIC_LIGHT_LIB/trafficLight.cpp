#include "trafficLight.h"
#include <dd_led.h>
void trafficLightInit(const TrafficLight &s) {
  ledInit(s.redPin); ledInit(s.yellowPin); ledInit(s.greenPin);
  trafficLightWrite(s, TrafficColor::Red);
}
void trafficLightWrite(const TrafficLight &s, TrafficColor c) {
  ledOff(s.greenPin); ledOff(s.yellowPin); ledOff(s.redPin);
  ledOn(c == TrafficColor::Green ? s.greenPin : c == TrafficColor::Yellow ? s.yellowPin : s.redPin);
}
