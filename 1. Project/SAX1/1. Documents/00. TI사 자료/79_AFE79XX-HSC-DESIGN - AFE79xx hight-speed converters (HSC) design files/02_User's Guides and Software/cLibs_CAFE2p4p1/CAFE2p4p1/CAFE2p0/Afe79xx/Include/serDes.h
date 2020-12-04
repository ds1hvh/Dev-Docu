#ifndef SERDES_H
#define SERDES_H

uint8_t serdesTx1010Pattern(uint8_t afeId, uint8_t laneNo);
uint8_t serdesTxSendData(uint8_t afeId, uint8_t laneNo);
uint8_t SetSerdesTxCursor(uint8_t afeId, uint8_t laneNo, uint8_t mainCursorSetting, uint8_t preCursorSetting, uint8_t postCursorSetting);
uint8_t getSerdesRxPrbsError(uint8_t afeId, uint8_t laneNo, uint32_t *errorRegValue);
uint8_t clearSerdesRxPrbsErrorCounter(uint8_t afeId, uint8_t laneNo);
uint8_t enableSerdesRxPrbsCheck(uint8_t afeId, uint8_t laneNo, uint8_t prbsMode, uint8_t enable);
uint8_t sendSerdesTxPrbs(uint8_t afeId, uint8_t laneNo, uint8_t prbsMode, uint8_t enable);
uint8_t getSerdesRxLaneEyeMarginValue(uint8_t afeId, uint8_t laneNo, uint16_t *regValue);
uint8_t resetSerDesDfeLane(uint8_t afeId, uint8_t laneNo);
uint8_t reAdaptSerDesLane(uint8_t afeId, uint8_t laneNo);
uint8_t resetSerDesDfeAllLanes(uint8_t afeId);
uint8_t reAdaptSerDesAllLanes(uint8_t afeId);
uint8_t getSerdesEye(uint8_t afeId, uint8_t laneNo, uint16_t *ber, uint16_t *extent);

#endif
