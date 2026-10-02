# Encoder incremental: laboratoarele 3.1 si 3.2

Conectare Mega 2560, dupa etichetele modulului: CLK -> D2, DT -> D3,
SW -> D22, + -> 5V, GND -> GND. D2/D3 sunt rezervate encoderului;
decuplati LED-urile laboratorului 2 daca sunt conectate pe acesti pini.
Intrari INPUT_PULLUP; butonul SW este activ LOW, debounce 30 ms.

Upload: megaatmega2560_lab31_encoder / megaatmega2560_lab32_encoder.
Monitor: 115200 baud. Mediile selecteaza LAB_SELECT=12/13.
Configuratia implicita nu a fost modificata. Variantele LDR sunt pastrate.

Module: ROTARY_ENCODER_LIB (decodor + driver AVR pentru un encoder),
ENCODER_SIGNALS_LIB (conversie/filtrare), ENCODER_REPORT_LIB (STDIO),
ENCODER_APP_LIB (task-uri comune). Punctele setup/loop se afla in
src/lab_3/app_lab_3.1_encoder.h/.cpp si app_lab_3.2_encoder.h/.cpp.

ISR pe CHANGE la ambele canale: valideaza tranzitiile Gray, acumuleaza
patru tranzitii pentru un ciclu complet. Revenirea contactului anuleaza
tranzitia opusa; salturile pe ambele canale sunt numarate ca invalide si
reseteaza acumularea partiala. Nu se face printf sau apel FreeRTOS in ISR.
Task-ul copiaza atomic contorul de 32 biti; ISR ramane scurt.
Acest algoritm nu garanteaza eliminarea tuturor erorilor mecanice si nu
recupereaza impulsurile pierdute la viteze mari. Filtrarea ulterioara nu
corecteaza o eroare permanenta a contorului.

3.1: achizitie periodica a contorului, conversie in unghi relativ, raportare.
3.2: FIFO 8 stari -> mediana ultimelor 5 pozitii semnate -> mediere ponderata
1,2,3 -> unghi -> limitare iesire la -360..360 grade -> mailbox 1 stare.
Contorul brut nu este limitat la acest interval si nu se infasoara la 360.
Unghiul este relativ la pornire, nu pozitie absoluta. Oprirea alimentarii
pierde reperul. SW raporteaza apasari; nu reseteaza pozitia.

Calibrare: cyclesPerRevolution in lib/ENCODER_APP_LIB/encoderApp.cpp este 20
implicit, o ipoteza de verificat pe modulul real. Marcati pozitia mecanica,
rotiti lent exact un tur si folositi diferenta absoluta a contorului afisat.
Un ciclu A/B complet nu este obligatoriu egal cu un clic mecanic.
Unghi = pozitie * 360 / cyclesPerRevolution. Sensul PLUS depinde de ordinea
CLK/DT; nu este etichetat automat ca sens orar. Sensul afisat este schimbarea
neta din ultimul interval de achizitie; miscari scurte pot lipsi din raport,
dar contorul pastreaza deplasarea neta.

FreeRTOS: achizitie 20 ms ceruti, prioritate 3; conditionare 20 ms, prioritate
2, offset 10 ms; raport 500 ms, prioritate 1, offset 50 ms. Pe AVR cu tick
nominal 16 ms devin 32/32/512 ms si offset-uri 16/64 ms. Toleranta watchdog
influenteaza perioadele reale. Achizitia impulsurilor este asincrona prin ISR;
task-ul achizitioneaza periodic starea acumulata, nu esantioneaza CLK lent.

Limita fata de cerinta 3.2: encoderul are iesiri digitale binare, dar pozitia
decodata este multivaloare. Nu exista ADC -> tensiune -> unghi in acest montaj.
Este o adaptare digitala impulsuri -> unghi, cu filtre si saturare; nu poate
fi prezentata drept indeplinire literala a conversiei analogice cerute.
Acceptarea acestei adaptari trebuie confirmata cu profesorul. Daca acea
conversie este obligatorie, utilizati un potentiometru sau senzor analogic.

Demonstratie: rotire in ambele sensuri, revenire la reper, apasare SW,
comparatie unghi brut/filtrat, peste un tur pentru saturare. Mediana intarzie
schimbarile reale scurte; medierea netezeste suplimentar. Dupa oprire iesirea
filtrata converge la unghiul brut (sau la limita daca saturat).

Verificat: compilarea ambelor medii Mega; teste host pentru sens, bounce,
tranzitii invalide, limita contorului, unghi negativ, impuls si saturare.
Hardware-ul nu a fost testat de asistent.
