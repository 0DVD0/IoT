# Lab 5.2 — test manual encoder -> PWM

La cererea utilizatorului, aplicatia activa a fost schimbata din PID in
control manual, fara senzor de turatie. Nu simuleaza sau masoara RPM si nu
indeplineste singura cerinta de reglare PID a laboratorului.
Bibliotecile PID_CONTROL_LIB si TACHOMETER_LIB raman in proiect, dar nu sunt
folosite in aceasta aplicatie.

Mediu: megaatmega2560_lab52; Serial 115200; LAB_SELECT=17.
L293D: D5 -> Enable pin 1, D6 -> 1A pin 2, D7 -> 2A pin 7.
Motor intre iesirile 3 si 6. VCC1 pin 16 = 5V logica; VCC2 pin 8 = sursa
motorului; GND comun pe 4,5,12,13. Pinii 9,10,15 la GND; 11,14 liberi.
Verificati dimensionarea alimentarii motorului; PWM nu limiteaza curentul
instantaneu de pornire/blocare.
KY040: CLK=D2, DT=D3, +=5V, GND comun. SW si D18 nu sunt utilizate.

Motorul porneste cu PWM=0. Fiecare ciclu complet de cuadratura modifica PWM
cu 5 unitati, cu saturare la 0 si 255. Encoderul comanda direct motorul,
intr-un singur sens. Modificati encoderDirection in -1 daca sensul reglajului
este invers celui dorit. La PWM mic motorul poate sa nu porneasca fizic.
Driverul L298_LIB compatibil cu L293D este reutilizat si primeste valoarea
PWM exacta, fara conversie intermediara in procente.

Comenzi cu Enter:
- stop: PWM=0; o noua rotire a encoderului poate reporni motorul.
- pwm 80: valoare intreaga 0..255.
- status: raportul comenzii curente, nu confirmare de miscare.
- help.

Nu trebuie ppr sau start. Nu exista oprire dupa 2 secunde pentru lipsa
feedbackului. Motorul ramane comandat pana la modificarea PWM, stop sau reset.
Raportarea se face la pornire, schimbarea PWM si comenzi, fara repetare periodica.

Module reutilizate: ROTARY_ENCODER_LIB, L298_LIB, SERIAL_STDIO_LIB.
Aplicatia: src/lab_5/app_lab_5.2.h/.cpp, lab52Setup si lab52Loop.
Compilare: pio run -e megaatmega2560_lab52.
