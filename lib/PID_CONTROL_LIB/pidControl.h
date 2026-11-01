#pragma once
class PidControl {
 public:
  bool tune(float p, float i, float d);
  void reset();
  float update(float target, float measured, float dt);
 private:
  float kp=0.05f, ki=0.02f, kd=0.001f;
  float integral=0, previous=0;
  bool initialized=false;
};
