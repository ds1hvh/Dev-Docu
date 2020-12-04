
#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H

uint8_t serdesRawRead(uint8_t afeId, uint16_t addr, uint16_t *readVal);
uint8_t serdesRawWrite(uint8_t afeId, uint16_t addr, uint16_t data);
uint8_t afeSpiWriteWrapper(uint8_t afeId, uint16_t addr, uint8_t data, uint8_t lsb, uint8_t msb);
uint8_t afeSpiReadWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t *readVal);
uint8_t serdesWriteWrapper(uint8_t afeId, uint16_t addr, uint16_t data, uint8_t lsb, uint8_t msb);
uint8_t serdesReadWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint16_t *readVal);
uint8_t serdesLaneWriteWrapper(uint8_t afeId, uint16_t addr, uint8_t laneNo, uint16_t data, uint8_t lsb, uint8_t msb);
uint8_t serdesLaneReadWrapper(uint8_t afeId, uint16_t addr, uint32_t laneNo, uint8_t lsb, uint8_t msb, uint16_t *readVal);
uint8_t afeSpiCheckWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t data, uint8_t *pbSame);
uint8_t afeSpiPollWrapper(uint8_t afeId, uint16_t addr, uint8_t expectedData, uint8_t lsb, uint8_t msb);
uint8_t afeSpiPollLogWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t expectedData);
uint8_t requestPllSpiAccess(uint8_t afeId, uint32_t regType);
uint8_t readTopMem(uint8_t afeId, uint32_t addr, uint64_t *readVal, uint32_t noBytes);
uint8_t closeAllPages(uint8_t afeId);

#endif
