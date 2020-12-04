
#ifndef _BASE_FUNCTIONS_H
#define _BASE_FUNCTIONS_H

uint8_t dev_spi_write(uint8_t afeId, uint16_t addr, uint8_t data);
uint8_t dev_spi_read(uint8_t afeId, uint16_t addr, uint8_t *readVal);
uint8_t wait(uint32_t wait_s);
uint8_t waitMs(uint32_t wait_ms);
void afeLogmsg(uint32_t level, const char *pcLogFmt, ...);
void setAfeLogLvl(uint32_t level);
uint32_t getAfeLogLvl();
uint8_t giveSingleSysrefPulse(uint8_t afeId);
uint8_t giveAfeAdcInput(uint8_t afeId, uint8_t rxChNo, uint8_t bandNo);
uint8_t connectAfeTxToFb(uint8_t afeId, uint8_t txChNo, uint8_t fbChNo, uint8_t bandNo);
#endif
