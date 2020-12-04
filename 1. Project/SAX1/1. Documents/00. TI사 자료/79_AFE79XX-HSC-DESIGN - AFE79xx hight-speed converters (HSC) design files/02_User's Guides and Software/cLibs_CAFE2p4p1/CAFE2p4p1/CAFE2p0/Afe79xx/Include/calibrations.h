#ifndef CALIBRATIONS_H
#define CALIBRATIONS_H
uint8_t doRxDsaCalib(uint8_t afeId, uint8_t rxChainForCalib, uint8_t fbChainForCalib, uint8_t useTxForCalib, uint8_t rxDsaBandCalibMode, uint8_t *readPacket, uint16_t *readPacketSize);
uint8_t doTxDsaCalib(uint8_t afeId, uint8_t txChainForCalib, uint8_t txDsaCalibMode, uint8_t txDsaBandCalibMode, uint8_t *readPacket, uint16_t *readPacketSize);
uint8_t loadTxDsaPacket(uint8_t afeId, uint8_t *array, uint16_t arraySize);
uint8_t loadRxDsaPacket(uint8_t afeId, uint8_t *array, uint16_t arraySize);
#endif
