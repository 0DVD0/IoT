#pragma once
#include <stdint.h>
#include <stddef.h>
namespace DistanceProtocol {
constexpr uint8_t address = 0x12, command = 0x01, head = 0xAA;
constexpr size_t frameSize = 9;
enum class Status : uint8_t { Valid, NoSample, EchoTimeout, OutOfRange };
enum class Error : uint8_t { None, Length, Header, Checksum, Payload };
struct Sample { uint8_t sensor; Status status; uint16_t millimeters, sequence; };
void encode(const Sample &sample, uint8_t *frame);
Error decode(const uint8_t *frame, size_t length, Sample &sample);
const char *errorName(Error error);
const char *statusName(Status status);
}
