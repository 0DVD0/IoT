# Lab 5.1 — temperatura ON-OFF cu histereza

Mediu PlatformIO: `megaatmega2560_lab51` (implicit). `LAB_SELECT=16` selecteaza
`lab51Setup()` si `lab51Loop()` din main.cpp. Nu foloseste FreeRTOS.
Upload si Serial Monitor se fac din PlatformIO, 115200 baud, terminator LF.

## Module

- `DHT_SENSOR_LIB`: driver temperatura DHT11/DHT22, validarea citirii.
- `HYSTERESIS_LIB`: regulator de incalzire independent de Arduino.
- `THERMOSTAT_UI_LIB`: comenzi limitate la 63 caractere, parsare si rapoarte STDIO.
- `RELAY_LIB`: driver existent reutilizat, polaritate configurabila.
- `SERIAL_STDIO_LIB`: mod neblocant raw adaugat; modul blocant existent ramane disponibil.
- `src/lab_5/app_lab_5.1.*`: configuratie pini, temporizare, coordonare.

## Conexiuni si configurare

DHT DATA -> D22, VCC -> 5V, GND -> GND. Pentru senzor fara modul, folositi
pull-up de 10 kOhm DATA–VCC; verificati daca modulul il include.
Releu IN -> D7, VCC -> 5V, GND -> GND. In aplicatie, `relayActiveLow=true`.
Verificati polaritatea modulului fizic. LED demonstrativ: 5V -> COM,
NO -> rezistor 220–330 Ohm -> anod LED; catod -> GND.
LED_BUILTIN semnalizeaza citire invalida sau veche.

Implicit `dhtType=DHT22`, ca in Wokwi. Pentru KY-015 cu DHT11 schimbati
constanta in DHT11 in app_lab_5.1.cpp si recompilati.

## Comportament

SP initial 28 C, histereza TOTALA 2 C: ON strict sub 27 C, OFF strict peste
29 C. Intre praguri si la egalitate se pastreaza starea. Initial OFF.
Citire la 2000 ms, prima dupa 2 s; raport periodic la 2000 ms.
Orice citire invalida opreste releul; ultima valoare nu este folosita pentru
control. Vechimea maxima acceptata este 6000 ms. Verificarea are loc cand
bucla ruleaza, nu este un watchdog hardware. Recuplarea este automata dupa
revenirea citirilor valide si indeplinirea pragului inferior.

Comenzi (punct pentru zecimale, Enter LF/CR/CRLF):
- `setpoint 28` (0..50 C)
- `hysteresis 2` (0.5..10 C, latime totala)
- `status`
- `help`

Setarile sunt volatile, revenind la valorile initiale dupa resetare.
Nu exista asteptare blocanta pentru tastare; citirea DHT in sine este sincrona.
LED-ul reprezinta incalzitorul, nu incalzeste automat senzorul. Pentru o bucla
fizica completa este necesar un actuator termic dimensionat si alimentat adecvat.

## Verificare

`pio run -e megaatmega2560_lab51`

Test regulator pe calculator:
```
c++ -std=c++11 -Wall -Wextra -Ilib/HYSTERESIS_LIB test/hysteresis/hysteresis_test.cpp lib/HYSTERESIS_LIB/hysteresis.cpp -o /tmp/hysteresis-test
/tmp/hysteresis-test
```
Test fizic/simulare: 26 -> 28 -> 30 -> 28 -> 26 C produce ON, ON, OFF, OFF, ON.
Verificati si comanda incompleta (rapoartele continua), CRLF, valori invalide,
linie peste 63 caractere si deconectarea DATA (OFF + alerta).
