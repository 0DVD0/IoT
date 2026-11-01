#pragma once
// Regulator de incalzire. Banda este latimea TOTALA in unitatile masurate.
class HeatingHysteresis {
 public:
  HeatingHysteresis(float reference, float totalBand);
  bool configure(float reference, float totalBand);
  bool update(float actual, bool valid);
  float reference() const { return setPoint; }
  float band() const { return width; }
  float lower() const { return setPoint - width / 2.0f; }
  float upper() const { return setPoint + width / 2.0f; }
  bool isOn() const { return on; }
 private:
  float setPoint;
  float width;
  bool on;
};
