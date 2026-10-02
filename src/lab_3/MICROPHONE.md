# Laboratoarele 3.1 / 3.2: microfon analogic

Modulul din fotografie seamana cu KY-038/KY-037. Verificati inscriptiile:
VCC/+ -> 5V, GND/- -> GND, AO/A0 -> A0 pe Mega. DO/D0 ramane neconectat.
Nu deduceti ordinea pinilor din orientarea fotografiei.
Referinta modulului: https://sensorkit.joy-it.net/en/sensors/ky-038

PlatformIO Project Tasks:
- megaatmega2560_lab31_microphone: achizitie si raportare;
- megaatmega2560_lab32_microphone: achizitie, conditionare, raportare.

Ambele folosesc STDIO la 115200 baud. main.cpp selecteaza variantele cu
LAB_SELECT=10/11 din mediile respective. Variantele NTC raman disponibile.

Driver: MICROPHONE_LIB. Semnale/conversie: MICROPHONE_SIGNALS_LIB.
Raportare: MICROPHONE_REPORT_LIB. Task-uri comune: MICROPHONE_APP_LIB.
Punctele de intrare setup/loop sunt in app_lab_3.1_microphone.h/.cpp si
app_lab_3.2_microphone.h/.cpp.

Microfonul produce o tensiune variabila, nu direct o valoare de volum.
Driverul citeste repetat ADC timp de 20 ms, cu intreruperile active,
si calculeaza max-min (amplitudine varf-la-varf). Task-ul ocupa procesorul
in aceasta fereastra. Se repeta la 100 ms ceruti (112 ms nominal pe AVR).
Nu este inregistrare audio continua: sunetele dintre ferestre pot fi ratate;
nu exista o frecventa de esantionare audio garantata sau analiza spectrala.

3.1: P-P ADC -> milivolti -> nivel relativ.
3.2: P-P ADC -> mediana 5 ferestre -> mediere ponderata 1,2,3 -> mV -> %.
Filtrele se aplica amplitudinilor, nu direct formei de unda audio. Un sunet
real foarte scurt (o bataie din palme) poate fi eliminat de mediana; folositi
un sunet sustinut pentru a demonstra cresterea iesirii filtrate.

mV = ADC * 5000 / 1023; nivel = mV * 100 / relativeFullScaleMv.
Referinta configurabila este 500 mV P-P in microphoneApp.cpp. Nu reprezinta
100% din volumul fizic maxim. Iesirea procentuala este limitata la 0..100.
Nu se raporteaza dB SPL, pascali sau intensitate W/m2: lipsesc sensibilitatea
calibrata si lantul de masura acustic. Daca profesorul cere obligatoriu o
marime acustica absoluta, este necesara calibrarea sau alt senzor documentat.

Raportul afiseaza min/max, P-P brut, mediana, tensiune bruta/filtrata,
procent, saturare, clipping ADC, numar proba, varsta si pierderi FIFO.
Clipping inseamna ca o citire a atins 0 sau 1023, nu dovedeste un defect.
Saturarea procentului este distincta de clipping ADC.

3.2 transmite ferestrele prin FIFO de 8 elemente, apoi publica ultima stare
intr-un mailbox de un element. Offset filtrare 30 ms ceruti, raport 70 ms.
Raportarea are perioada 500 ms ceruti (512 ms nominal). Toleranta watchdog
si preemptia afecteaza timpii reali. Nu toate ferestrele apar in terminal.

Test fizic: observati zgomotul de fond, apoi vorbiti/aplicati sunet sustinut
aproape de microfon; comparati brut si filtrat. Unele module au semnal AO
foarte mic. Potentiometrul regleaza pragul comparatorului DO; nu presupuneti
ca este un control de amplificare AO. Ajustati referinta relativa numai dupa
observarea valorilor reale; aceasta nu inlocuieste calibrarea in dB.
