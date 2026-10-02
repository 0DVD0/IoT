#include "microphoneReport.h"
#include <stdio.h>
void microphonePrintReport(const MicrophoneSignals &s, uint32_t nowMs) {
  printf("\n[MIC] t=%lu proba=%lu varsta=%lu ms pierdute=%lu\n",
    (unsigned long)nowMs, (unsigned long)s.number,
    (unsigned long)(nowMs-s.reading.sampledAtMs), (unsigned long)s.dropped);
  printf("ADC min=%u max=%u P-P=%u median=%u esantioane=%u\n",
    (unsigned)s.reading.minimumAdc, (unsigned)s.reading.maximumAdc,
    (unsigned)s.reading.peakToPeakAdc, (unsigned)s.medianAdc, (unsigned)s.reading.sampleCount);
  printf("P-P brut=%u mV filtrat=%u mV nivel relativ=%u%% saturat=%u clipping=%u\n",
    (unsigned)(s.rawMillivolts+0.5f), (unsigned)(s.filteredMillivolts+0.5f),
    (unsigned)(s.relativePercent+0.5f), (unsigned)s.saturated, (unsigned)s.reading.clipped);
}
