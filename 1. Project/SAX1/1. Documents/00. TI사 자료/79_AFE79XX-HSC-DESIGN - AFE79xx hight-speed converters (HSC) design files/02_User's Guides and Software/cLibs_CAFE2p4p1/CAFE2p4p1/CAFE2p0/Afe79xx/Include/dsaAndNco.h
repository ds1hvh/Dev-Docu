#ifndef DSAANDNCO_H
#define DSAANDNCO_H
uint8_t setTxDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting);
uint8_t setFbDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting);
uint8_t setRxDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting);
uint8_t setRxDigGain(uint8_t afeId, uint8_t chNo, uint8_t bandNo, uint8_t dsaSetting);
uint8_t setRxDsaMode(uint8_t afeId, uint8_t topNo, uint8_t mode);
uint8_t setPinRxDsaSettings(uint8_t afeId, uint8_t chNo, uint8_t dsaInit, uint8_t dsaStep, uint8_t maxDelay);
uint8_t setTxDigGain(uint8_t afeId, uint8_t chNo, uint8_t bandNo, int16_t dig_gain);
uint8_t txDsaIdxGainSwap(uint8_t afeId, uint8_t chNo, uint8_t anaAttn0, uint8_t anaAttn1, int8_t digB0Gain0, int8_t digB0Gain1, int8_t digB1Gain0, int8_t digB1Gain1);
uint8_t updateTxGainParam(uint8_t afeId, uint8_t mode, uint8_t transitTime, uint8_t maxAnaDsa);
uint8_t updateTxGain(uint8_t afeId, uint8_t txChainSel, uint8_t gainValidity, uint16_t tx0B0Dsa, uint16_t tx0B1Dsa, uint16_t tx1B0Dsa, uint16_t tx1B1Dsa);
uint8_t updateTxNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t nco);
uint8_t updateTxNcoDb(uint8_t afeId, uint8_t chNo, uint8_t nco, uint32_t band0Nco0, uint32_t band1Nco0, uint32_t band0Nco1, uint32_t band1Nco1);
uint8_t rxNCOSel(uint8_t afeId, uint8_t chno, uint8_t BandId, uint8_t ovr, uint8_t NCOId);
uint8_t fbNCOSel(uint8_t afeId, uint8_t topno, uint8_t ovr, uint8_t NCOId);
uint8_t updateRxNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t band, uint8_t nco);
uint8_t updateFbNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t nco);
uint8_t readRxNco(uint8_t afeId, uint8_t chNo, uint8_t band, uint8_t nco, double *ncoFreq);
uint8_t readTxNco(uint8_t afeId, uint8_t chNo, uint8_t band, uint8_t nco, int64_t *val);
uint8_t setFbDsaPerTx(uint8_t afeId, uint8_t pinNo, uint8_t dsaSetting);
uint8_t fbDsaPerTxEn(uint8_t afeId, uint8_t en);

#endif
