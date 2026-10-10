#include <Arduino.h>

#define LAB_11 1
#define LAB_21 2
#define LAB_22 3
#define LAB_31_NTC 4
#define LAB_31_LDR 5
#define LAB_31_JOYSTICK 6
#define LAB_32_NTC 7
#define LAB_32_LDR 8
#define LAB_32_JOYSTICK 9
#define LAB_31_MICROPHONE 10
#define LAB_32_MICROPHONE 11
#define LAB_31_ENCODER 12
#define LAB_32_ENCODER 13
#define LAB_41 14
#define LAB_42 15
#ifndef LAB_SELECT
#define LAB_SELECT LAB_41
#endif

#if LAB_SELECT == LAB_11
#include "lab_1/app_lab_1.1.h"
#elif LAB_SELECT == LAB_21
#include "lab_2/app_lab_2.1.h"
#elif LAB_SELECT == LAB_22
#include "lab_2/app_lab_2.2.h"
#elif LAB_SELECT == LAB_31_NTC
#include "lab_3/app_lab_3.1_ntc.h"
#elif LAB_SELECT == LAB_31_LDR
#include "lab_3/app_lab_3.1_ldr.h"
#elif LAB_SELECT == LAB_31_JOYSTICK
#include "lab_3/app_lab_3.1_joystick.h"
#elif LAB_SELECT == LAB_32_NTC
#include "lab_3/app_lab_3.2_ntc.h"
#elif LAB_SELECT == LAB_32_LDR
#include "lab_3/app_lab_3.2_ldr.h"
#elif LAB_SELECT == LAB_32_JOYSTICK
#include "lab_3/app_lab_3.2_joystick.h"
#elif LAB_SELECT == LAB_31_MICROPHONE
#include "lab_3/app_lab_3.1_microphone.h"
#elif LAB_SELECT == LAB_32_MICROPHONE
#include "lab_3/app_lab_3.2_microphone.h"
#elif LAB_SELECT == LAB_31_ENCODER
#include "lab_3/app_lab_3.1_encoder.h"
#elif LAB_SELECT == LAB_32_ENCODER
#include "lab_3/app_lab_3.2_encoder.h"
#elif LAB_SELECT == LAB_41
#include "lab_4/app_lab_4.1.h"
#elif LAB_SELECT == LAB_42
#include "lab_4/app_lab_4.2.h"
#else
#error Laborator necunoscut
#endif

void setup() {
#if LAB_SELECT == LAB_11
  lab11Setup();
#elif LAB_SELECT == LAB_21
  lab21Setup();
#elif LAB_SELECT == LAB_22
  lab22Setup();
#elif LAB_SELECT == LAB_31_NTC
  lab31NtcSetup();
#elif LAB_SELECT == LAB_31_LDR
  lab31LdrSetup();
#elif LAB_SELECT == LAB_31_JOYSTICK
  lab31JoystickSetup();
#elif LAB_SELECT == LAB_32_NTC
  lab32NtcSetup();
#elif LAB_SELECT == LAB_32_LDR
  lab32LdrSetup();
#elif LAB_SELECT == LAB_32_JOYSTICK
  lab32JoystickSetup();
#elif LAB_SELECT == LAB_31_MICROPHONE
  lab31MicrophoneSetup();
#elif LAB_SELECT == LAB_32_MICROPHONE
  lab32MicrophoneSetup();
#elif LAB_SELECT == LAB_31_ENCODER
  lab31EncoderSetup();
#elif LAB_SELECT == LAB_32_ENCODER
  lab32EncoderSetup();
#elif LAB_SELECT == LAB_41
  lab41Setup();
#elif LAB_SELECT == LAB_42
  lab42Setup();
#endif
}

void loop() {
#if LAB_SELECT == LAB_11
  lab11Loop();
#elif LAB_SELECT == LAB_21
  lab21Loop();
#elif LAB_SELECT == LAB_22
  lab22Loop();
#elif LAB_SELECT == LAB_31_NTC
  lab31NtcLoop();
#elif LAB_SELECT == LAB_31_LDR
  lab31LdrLoop();
#elif LAB_SELECT == LAB_31_JOYSTICK
  lab31JoystickLoop();
#elif LAB_SELECT == LAB_32_NTC
  lab32NtcLoop();
#elif LAB_SELECT == LAB_32_LDR
  lab32LdrLoop();
#elif LAB_SELECT == LAB_32_JOYSTICK
  lab32JoystickLoop();
#elif LAB_SELECT == LAB_31_MICROPHONE
  lab31MicrophoneLoop();
#elif LAB_SELECT == LAB_32_MICROPHONE
  lab32MicrophoneLoop();
#elif LAB_SELECT == LAB_31_ENCODER
  lab31EncoderLoop();
#elif LAB_SELECT == LAB_32_ENCODER
  lab32EncoderLoop();
#elif LAB_SELECT == LAB_41
  lab41Loop();
#elif LAB_SELECT == LAB_42
  lab42Loop();
#endif
}
