#ifndef CONTROLS_H
#define CONTROLS_H
uint8_t getChipVersion(uint8_t afeId);
uint8_t checkSysref(uint8_t afeId, uint8_t clearSysrefFlag, uint8_t *sysrefReceived);
uint8_t sendSysref(uint8_t afeId, uint8_t spiSysref, uint8_t getSpiAccess);
uint8_t overrideTdd(uint8_t afeId, uint8_t rx, uint8_t fb, uint8_t tx, uint8_t enableOverride);
uint8_t overrideTddPins(uint8_t afeId, uint8_t rx, uint8_t fb, uint8_t tx);
uint8_t checkPllLockStatus(uint8_t afeId, uint8_t *pllLockStatus);
uint8_t clearPllStickyLockStatus(uint8_t afeId);
uint8_t readAlarmPinStatus(uint8_t afeId, uint8_t alarmNo, uint8_t *status);
uint8_t clearSpiAlarms(uint8_t afeId);
uint8_t readSpiAlarms(uint8_t afeId, uint8_t *alarmStatus);
uint8_t readTxPower(uint8_t afeId, uint8_t chNo, uint16_t windowLen, double *powerReadB0, double *powerReadB1);
uint8_t getRxRmsPower(uint8_t afeId, uint8_t chNo, double *avg_pwrdb);
uint8_t getFbRmsPower(uint8_t afeId, uint8_t chNo, double *avg_pwrdb);
uint8_t clearAllAlarms(uint8_t afeId);
uint8_t overrideAlarmPin(uint8_t afeId, uint8_t alarmNo, uint8_t overrideSel, uint8_t overrideVal);
uint8_t overrideRelDetPin(uint8_t afeId, uint8_t chNo, uint8_t overrideSel, uint8_t overrideVal);
uint8_t overrideDigPkDetPin(uint8_t afeId, uint8_t chNo, uint8_t pinNo, uint8_t overrideSel, uint8_t overrideVal);
uint8_t checkDeviceHealth(uint8_t afeId, uint16_t *allOk);
#endif
