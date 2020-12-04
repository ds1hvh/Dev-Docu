#ifndef MACRO_CONSTANTS_H
#define MACRO_CONSTANTS_H

uint8_t splitToByte(uint64_t val, uint8_t numBytes, uint8_t *splitByteList);
uint8_t writeOperandList(uint8_t afeId, uint8_t *operandList, uint8_t numOfOperands);
uint8_t readResultRegSpi(uint8_t afeId, uint8_t regNum, uint32_t *result);
uint8_t waitForMacroReady(uint8_t afeId);
uint8_t waitForMacroDone(uint8_t afeId);
uint8_t waitForMacroAck(uint8_t afeId);
uint8_t checkForMacroError(uint8_t afeId, uint8_t *errorReg);
uint8_t executeMacro(uint8_t afeId, uint8_t *byteList, uint8_t numOfOperands, uint8_t opcode);
uint8_t triggerMacro(uint8_t afeId, uint8_t opcode);
uint8_t enableMemAccess(uint8_t afeId, uint8_t en);
uint8_t doSystemTuneSelective(uint8_t afeId, uint8_t rxChList, uint8_t fbChList, uint8_t txChList, uint8_t sectionEnable);
uint8_t updateSystemTxChannelFreqConfig(uint8_t afeId, uint8_t txChList, uint8_t listNCO, uint32_t txNCO, uint8_t immUpdt, uint8_t reload);
uint8_t checkMcuHealth(uint8_t afeId, uint8_t *healthOk);
uint8_t txCalibSiggen(uint8_t afeId, uint8_t chNo, uint8_t configOption, uint32_t freq0, uint8_t freq0Amp);
#endif
