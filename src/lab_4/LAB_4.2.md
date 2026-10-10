# Laboratorul 4.2 — Motor DC prin L298N

## Pornire

PlatformIO -> Project Tasks -> megaatmega2560_lab42 -> Upload, apoi Monitor
la 115200 baud. Mediul defineste LAB_SELECT=15; selectia implicita a celorlalte
laboratoare nu a fost modificata. Nu necesita LCD sau Keypad.

## Conectare Mega 2560

| Mega / componenta | L298N |
|---|---|
| D5 PWM | ENA (scoateti jumperul ENA) |
| D6 | IN1 |
| D7 | IN2 |
| GND Mega si minus sursa motor | GND |
| Cele doua borne motor DC | OUT1 si OUT2 |
| Plus alimentare externa adecvata motorului | Vs, adesea marcat 12V |

Motorul nu se alimenteaza din pinul 5V al placii Mega. Folositi alimentarea
externa conform tensiunii nominale si curentului motorului, in limitele
modulului. Canalul B nu este utilizat.

Alimentarea LOGICA de 5V depinde de modulul L298N:
- Daca documentatia modulului permite alimentare logica externa: scoateti
  jumperul 5V-EN al regulatorului, apoi alimentati intrarea logica 5V cu 5V
  de la Mega, cu masa comuna.
- Daca folositi regulatorul integrat, respectati domeniul lui de alimentare
  si nu conectati iesirea lui de 5V in paralel cu 5V de la Mega/USB.
Jumperul 5V-EN al regulatorului nu este acelasi cu jumperul ENA al PWM.
Verificati inscriptiile modulului inainte de alimentare. Folositi modulul
L298N cu diodele de protectie pentru sarcina inductiva, nu circuitul integrat
neechipat. Modificarile conexiunilor se fac fara alimentare.

## Comenzi STDIO

Introduceti fiecare comanda pe o linie, urmata de Enter:

motor set 40
motor set -40
motor inc
motor dec
motor max
motor stop
motor status
help

Valorile acceptate sunt numere intregi intre -100 si 100. Valorile invalide,
texte suplimentare, fractiile si liniile de peste 63 caractere sunt respinse
fara a modifica comanda motorului. Spatiile exterioare sunt ignorate.

inc mareste MODULUL cu 10 puncte procentuale: -40 -> -50, +40 -> +50.
dec reduce modulul spre zero: -40 -> -30, +40 -> +30. Nu inverseaza sensul.
La atingerea limitelor se afiseaza un mesaj. Setarea zero si stop pastreaza
ultimul sens cerut; max aplica 100% in acel sens. La pornire sensul memorat
este inainte, iesirea este oprita.

## Niveluri software

- L298_LIB: l298Init, l298Coast, l298Drive; GPIO si PWM, dezactivarea ENA
  inaintea schimbarii IN1/IN2.
- DC_MOTOR_LIB: validare domeniu, sens memorat, inc/dec/max/stop, conversie
  procent -> PWM 0..255, stare de asteptare la schimbarea sensului.
- MOTOR_COMMAND_LIB: interpretarea textului, separat de hardware.
- MOTOR_REPORT_LIB: raportarea prin printf.
- RTOS_SERIAL_STDIO_LIB: stdin prin scanf, stdout prin printf; asteptarea
  caracterelor blocheaza task-ul cu vTaskDelay(1). Fara ecou local din MCU.
  Driverul STDIO vechi al celorlalte laboratoare nu este modificat.
- src/lab_4/app_lab_4.2.h/.cpp: lab42Setup/lab42Loop, cozi si task-uri.

## FreeRTOS

Recepție (prioritate 2) -> coada 8 comenzi -> control (prioritate 3)
-> mailbox ultima stare -> raportare (prioritate 1) -> printf/Serial.

Numai task-ul de control modifica motorul. Numai task-ul de raportare scrie
stdout dupa setup, pentru a evita amestecarea mesajelor. Recepția foloseste
scanf("%c") cu buffer limitat si normalizare CR/LF/CRLF in adaptor.

Task-ul de control asteapta o comanda maximum un tick, apoi actualizeaza si
starea de inversare. Task-ul de raportare este trezit la publicarea starii
sau la expirarea perioadei de aproximativ o secunda. Mailboxul pastreaza
ultima stare; comenzi trimise in rafala pot avea rapoarte intermediare
comasate. Numarul comenzii permite observarea acestei situatii.

motor stop elimina comenzile vechi inca aflate in coada si este trimisa in
fata. Oprirea se aplica dupa receptia completa a liniei si planificarea
task-ului; nu este un buton hardware de urgenta. Comenzile trimise ulterior
pot reporni motorul in mod normal.

## Comanda si marimile raportate

PWM = round(abs(procent) * 255 / 100). De exemplu 50% -> 128/255.
PWM reprezinta factor de umplere comandat. Nu masoara puterea electrica,
cuplul sau turatia. Frecventa PWM este cea implicita Arduino pentru D5;
nu se modifica timerele pentru aceasta aplicatie.

Setat: procentul semnat cerut. Aplicat: procentul efectiv comandat puntii.
PWM: valoarea analogWrite. Sens memorat: sensul folosit de max/inc la zero.
PAUZA INVERSARE: iesire temporar oprita, cerere de sens opus in asteptare.
COMANDAT PORNIT/OPRIT: stare software, fara feedback de la motor.

La schimbarea sensului, ENA este dezactivat timp de minimum 100 ms masurate
cu millis(), apoi se aplica noul sens. Verificarea se face la executia
task-ului, deci pauza poate fi mai lunga. Pauza nu garanteaza oprirea mecanica
si trebuie adaptata motorului/sarcinii. stop anuleaza inclusiv o inversare
in asteptare; reintroducerea sensului opus dupa stop respecta pauza ramasa.

Oprirea este prin ENA=LOW, adica free-running/coast conform datasheet-ului.
Alimentarea comandata motorului este intrerupta, dar rotorul poate continua
prin inertie. Nu este implementata franare activa. La nivel mic de PWM,
motorul real poate sa nu porneasca, din cauza frecarii si caderii pe driver.

Referinta hardware: https://www.st.com/resource/en/datasheet/l298.pdf

## Testare

Au fost compilate mediile 4.2 si 4.1. Testele host verifica parserul,
limitele, PWM, inc/dec negativ, pauza la inversare, stop in timpul pauzei
si trecerea millis() peste limita de 32 biti. Testele folosesc GPIO simulat;
nu valideaza curentii, conectarea sau miscarea mecanica.

Rulare teste host din radacina proiectului:

c++ -std=c++11 -Itest/motor/fakes -Ilib/L298_LIB -Ilib/DC_MOTOR_LIB -Ilib/MOTOR_COMMAND_LIB test/motor/motor_test.cpp lib/L298_LIB/dd_l298.cpp lib/DC_MOTOR_LIB/dcMotor.cpp lib/MOTOR_COMMAND_LIB/motorCommand.cpp -o /tmp/motor-test
/tmp/motor-test

Pentru raport: adaugati schema electrica, diagrama de mai sus, masuratori si
capturi proprii. Implementarea nu include un model Wokwi sau fisiere JSON.
