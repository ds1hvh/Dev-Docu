#ifndef HAGC_H
#define HAGC_H
uint8_t agcStateControlConfig(uint8_t afeId, uint8_t chNo, uint16_t agcstate);
uint8_t agcDigDetConfig(uint8_t afeId, uint8_t chNo, uint8_t bigStepAttkEn, uint8_t smallStepAttkEn, uint8_t bigStepDecEn, uint8_t smallStepDecEn, uint8_t powerAttkEn, uint8_t powerDecEn,
						uint8_t bigStepAttkThresh, uint8_t smallStepAttkThresh, uint8_t bigStepDecThresh, uint8_t smallStepDecThresh, uint8_t powerAttkThresh, uint8_t powerDecThresh);
uint8_t agcDigDetTimeConstantConfig(uint8_t afeId, uint8_t chNo, uint32_t bigStepAttkWinLen, uint32_t miscStepAttkWinLen, uint32_t decayWinLen);
uint8_t agcDigDetAbsoluteNumCrossingConfig(uint8_t afeId, uint8_t chNo, uint32_t bigStepAttkNumHits, uint32_t smallStepAttkNumHits, uint32_t bigStepDecNumHits, uint32_t smallStepDecNumHits);
uint8_t agcDigDetRelativeNumCrossingConfig(uint8_t afeId, uint8_t chNo, uint32_t bigStepAttkNumHits, uint32_t smallStepAttkNumHits, uint32_t bigStepDecNumHits, uint32_t smallStepDecNumHits);
uint8_t externalAgcConfig(uint8_t afeId, uint8_t chNo, uint16_t pin0sel, uint16_t pin1sel, uint16_t pin2sel, uint16_t pin3sel, uint8_t pkDetPinLsbSel, uint8_t pulseExpansionCount, uint8_t noLsbsToSend);
uint8_t minMaxDsaAttnConfig(uint8_t afeId, uint8_t chNo, uint8_t minDsaAttn, uint8_t maxDsaAttn);
uint8_t agcGainStepSizeConfig(uint8_t afeId, uint8_t chNo, uint8_t bigStepAttkStepSize, uint8_t smallStepAttkStepSize, uint8_t bigStepDecayStepSize, uint8_t smallStepDecayStepSize);
uint8_t internalAgcConfig(uint8_t afeId, uint8_t chNo, uint8_t tdd_freeze_agc, uint16_t blank_time_extcomp, uint8_t en_agcfreeze_pin, uint8_t extCompControlEn);
uint8_t rfAnalogDetConfig(uint8_t afeId, uint8_t chNo, uint8_t rfdeten, uint8_t rfDetMode, uint8_t rfDetNumHitsMode, uint32_t rfdetnumhits, uint8_t rfdetThreshold, uint8_t rfdetstepsize);
uint8_t extLnaConfig(uint8_t afeId, uint8_t chNo, uint8_t singleDualBandMode, uint8_t lnaGainMargin, uint8_t enBandDet, uint8_t tapOffPoint);
uint8_t extLnaGainConfig(uint8_t afeId, uint8_t chNo, uint16_t lnaGainB0, uint16_t lnaPhaseB0, uint16_t lnaGainB1, uint16_t lnaPhaseB1);
uint8_t alcConfig(uint8_t afeId, uint8_t chNo, uint8_t alcMode, uint8_t totalGainRange, uint8_t minAttnAlc, uint8_t useMinAttnAgc);
uint8_t fltPtConfig(uint8_t afeId, uint8_t chNo, uint8_t fltPtMode, uint8_t fltPtFmt);
uint8_t coarseFineConfig(uint8_t afeId, uint8_t chNo, uint8_t stepSize, uint8_t nBitIndex, uint8_t indexInvert, uint8_t indexSwapIQ, uint8_t sigBackOff, uint8_t gainChangeIndEn);

#endif
