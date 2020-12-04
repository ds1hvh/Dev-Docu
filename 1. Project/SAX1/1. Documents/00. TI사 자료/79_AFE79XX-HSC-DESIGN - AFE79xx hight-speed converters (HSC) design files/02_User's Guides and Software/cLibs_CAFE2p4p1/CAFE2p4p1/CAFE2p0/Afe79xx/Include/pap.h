#ifndef PAP_H
#define PAP_H

uint8_t configurePapMaDet(uint8_t afeId, uint8_t chno, uint8_t maEnable, uint16_t maNumSample, uint16_t maWindowCntr,
						  uint16_t maWindowCntrTh, uint16_t maThreshB0, uint16_t maThreshB1, uint16_t maThreshComb);
uint8_t configurePapHpfDet(uint8_t afeId, uint8_t chno, uint8_t hpfEnable, uint16_t hpfNumSample, uint16_t hpfWindowCntr,
						   uint16_t hpfWindowCntrTh, uint16_t hpfThreshB0, uint16_t hpfThreshB1, uint16_t hpfThreshComb);
uint8_t configurePap(uint8_t afeId, uint8_t chno, uint8_t enable, uint8_t multMode, uint8_t rampDownStartVal,
					 uint8_t attnStepSize, uint8_t gainStepSize, uint8_t detectInWaitState, float triggerToRampDown,
					 float waitCounter, float triggerClearToRampUp, float amplUpdateCycles, float alarmPulseGPIO,
					 uint8_t alarmMask, uint8_t alarmChannelMask, uint8_t alarmPinDynamicMode, uint8_t rampStickyMode);
uint8_t rampStickyClear(uint8_t afeId, uint8_t chno);
uint8_t papAlarmStatus(uint8_t afeId, uint8_t chno, uint8_t *alarmTriggered);
uint8_t clearPapAlarms(uint8_t afeId, uint8_t chno);
uint8_t configLaneErrorsForTxPap(uint8_t afeId, uint8_t chno, uint8_t laneMask);

#endif
