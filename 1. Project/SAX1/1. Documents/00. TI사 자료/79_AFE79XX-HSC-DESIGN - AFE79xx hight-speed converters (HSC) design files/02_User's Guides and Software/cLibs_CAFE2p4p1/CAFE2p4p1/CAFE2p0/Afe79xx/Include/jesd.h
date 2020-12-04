#ifndef JESD_H
#define JESD_H
uint8_t dacJesdSendData(uint8_t afeId, uint8_t topno);
uint8_t dacJesdConstantTestPatternValue(uint8_t afeId, uint8_t topno, uint8_t enable, uint8_t chNo, uint8_t bandNo, uint16_t valueI, uint16_t valueQ);
uint8_t dacJesdSendRampTestPattern(uint8_t afeId, uint8_t topno, uint8_t increment);
uint8_t getJesdRxLaneErrors(uint8_t afeId, uint8_t laneNo, uint8_t *error);
uint8_t getJesdRxLaneFifoErrors(uint8_t afeId, uint8_t laneNo, uint8_t *error);
uint8_t getJesdRxMiscSerdesErrors(uint8_t afeId, uint8_t jesdNo, uint8_t *errorValue);
uint8_t getJesdRxAlarms(uint8_t afeId, uint8_t *error);
uint8_t getJesdRxLinkStatus(uint8_t afeId, uint16_t *linkStatus);
uint8_t getJesdRxLinkStatus204B(uint8_t afeId, uint16_t *linkStatus);
uint8_t getJesdRxLinkStatus204C(uint8_t afeId, uint16_t *linkStatus);
uint8_t clearJesdTxAlarms(uint8_t afeId);
uint8_t clearJesdRxAlarms(uint8_t afeId);
uint8_t clearJesdRxAlarmsForPap(uint8_t afeId);
uint8_t jesdRxClearSyncErrorCnt(uint8_t afeId, uint8_t jesdNo);
uint8_t jesdRxGetSyncErrorCnt(uint8_t afeId, uint8_t jesdNo, uint8_t *linkErrorCount);
uint8_t jesdTxGetSyncErrorCnt(uint8_t afeId, uint8_t jesdLaneNo, uint8_t *linkErrorCount);
uint8_t adcRampTestPattern(uint8_t afeId, uint8_t topno, uint8_t chNo, uint8_t enable, uint8_t rampIncr);
uint8_t toggleSync(uint8_t afeId, uint8_t overrideValue);
uint8_t setJesdTxSyncOverride(uint8_t afeId, uint8_t syncNo, uint8_t overrideValue, uint8_t syncValue);
uint8_t setJesdRxSyncOverride(uint8_t afeId, uint8_t syncNo, uint8_t overrideValue, uint8_t syncValue);
uint8_t getJesdTxFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t *errors);
uint8_t jesdRxFullResetToggle(uint8_t afeId, uint8_t jesdNo);
uint8_t jesdTxFullResetToggle(uint8_t afeId, uint8_t jesdNo);
uint8_t adcDacSync(uint8_t afeId, uint8_t pinSysref);
uint8_t jesdRxResetStateMachine(uint8_t afeId, uint8_t linkNo);
uint8_t getAllLaneReady(uint8_t afeId, uint8_t jesdNo, uint8_t *rbdOffset);
uint8_t checkIfRbdIsGood(uint8_t afeId, uint8_t jesdNo, uint8_t *rbdStatus);
uint8_t setGoodRbd(uint8_t afeId, uint8_t jesdNo);
uint8_t maskJesdRxLaneErrors(uint8_t afeId, uint8_t laneNo, uint8_t maskValue);
uint8_t maskJesdRxLaneFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t losMaskValue, uint8_t fifoMaskValue);
uint8_t maskJesdRxMiscSerdesErrors(uint8_t afeId, uint8_t jesdNo, uint8_t maskSerdesPllLock);
uint8_t maskJesdTxFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t maskValue);
uint8_t maskJesdRxLaneErrorsToPap(uint8_t afeId, uint8_t laneNo, uint8_t maskValue);
uint8_t maskJesdRxLaneFifoErrorsToPap(uint8_t afeId, uint8_t jesdNo, uint8_t losMaskValue, uint8_t fifoMaskValue);
uint8_t maskJesdRxMiscSerdesErrorsToPap(uint8_t afeId, uint8_t jesdNo, uint8_t maskSerdesPllLock);
uint8_t setManualRbd(uint8_t afeId, uint8_t jesdNo, uint8_t value);

#endif
