/** @file baseFunc.c
 * 	@brief	This file has functions which can be edited by customers to integrate it into their system.<br>
 * 		<b> Version 2.1.1:</b> <br>
 *      1. Fixed warnings in the bringup functions.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 *      2. Added functions to bringup from file.<br>
 *      3. Added functions for calibration.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "afe79xxTypes.h"
#include "afe79xxLog.h"
#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeCommonMacros.h"

/**
    @brief AFE SPI Write driver function.
    @details AFE SPI Write driver function. The contents of this function should be replaced by host SPI driver function.
    @param afeId AFE ID
	@param addr Address to be written to.
	@param data value to be written.
	@return Returns if the function execution passed or failed.
*/
uint8_t dev_spi_write(uint8_t afeId, uint16_t addr, uint8_t data)
{
    afeLogDbg("WRITE: afeId: %d, addr: 0x%X, data: 0x%X", afeId, addr, data);
    /* TBD: User domain */
    return RET_OK;
}

/**
    @brief AFE SPI read driver function.
    @details AFE SPI read driver function and returns the read value as pointer. The contents of this function should be replaced by host SPI driver function.
    @param afeId AFE ID
	@param addr Address to be read from.
	@param readVal Pointer return of the value read.
	@return Returns if the function execution passed or failed.
*/
uint8_t dev_spi_read(uint8_t afeId, uint16_t addr, uint8_t *readVal)
{
    /* TBD: User domain */
    *readVal = 0;
    afeLogDbg("READ: AFEID:%d: ADDR: 0X%X, Read Val: 0X%X", afeId, addr, *readVal);
    return RET_OK;
}

/**
    @brief AFE single shot Pin Sysref.
    @details AFE single shot pin sysref driver function. The contents of this function should be replaced by host driver function.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t giveSingleSysrefPulse(uint8_t afeId)
{
    /* Code for giving Sysref pulse. This is for cases where the pulse mode of Sysref. */
    afeLogInfo("Giving Pin Sysref Pulse for AFE %d", afeId);
    return RET_OK;
}

/**
    @brief Give ADC input for factory Calibration
    @details Give ADC input for factory Calibration. The contents of this function contents should be replaced by host driver function.
    @param afeId AFE ID
    @param chNo Channel Number.<br>
            0-RXA
            1-RXB
            2-RXC
            3-RXD
            4-FBAB
            5-FBCD
    @param bandNo Band Number 0/1.<br>
	@return Returns if the function execution passed or failed.
*/
uint8_t giveAfeAdcInput(uint8_t afeId, uint8_t chNo, uint8_t bandNo)
{
    if (chNo < AFE_NUM_RX_CHANNELS)
    {
        afeLogInfo("AFE%d: Gave input to RX%d, Band%d.", afeId, chNo, bandNo);
    }
    else
    {
        afeLogInfo("AFE%d: Gave input to FB%d, Band%d.", afeId, chNo - AFE_NUM_RX_CHANNELS, bandNo);
    }
    return RET_OK;
}

/**
    @brief Connect TX Output to FB for factory Calibration
    @details Connect TX Output to FB for factory Calibration.<br>
        The contents of this function contents should be replaced by host driver function.
    @param afeId AFE ID
    @param txChNo Bit Wise TX Channel Select.<br>
			Bit0 for TXA<br>
			Bit1 for TXB<br>
			Bit2 for TXC<br>
			Bit3 for TXD
    @param fbChNo Bit Wise FB Channel Select.
			Bit0 for FBAB<br>
			Bit1 for FBCD<br>
            When this is 1 or 2, connect the TX represented by txChNo to FBAB or FBCD respectively.<br>
            When this is 3, connect the TX represented by txChNo[1:0] to FBAB and TX represented by txChNo[3:2] to FBCD
    @param bandNo Bit Wise Band Select.
			Bit0 for Band 0<br>
			Bit1 for Band 1
	@return Returns if the function execution passed or failed.
*/
uint8_t connectAfeTxToFb(uint8_t afeId, uint8_t txChNo, uint8_t fbChNo, uint8_t bandNo)
{
    afeLogInfo("AFE%d: Connecting TX 0x%x to FB: 0x%x, Band%d.", afeId, txChNo, fbChNo, bandNo);
    return RET_OK;
}

/**
    @brief Wait in Seconds
    @details Wait in Seconds. The contents of this function should be replaced by host driver function.
    @param wait_s Wait time in seconds.
	@return Returns if the function execution passed or failed.
*/
uint8_t wait(uint32_t wait_s)
{
    afeLogSpiLog("WAIT: %d", wait_s);
    /* TBD: User domain */
    return RET_OK;
}

/**
    @brief Wait in milli Seconds
    @details Wait in milli Seconds. The contents of this function should be replaced by host driver function.
    @param wait_ms Wait time in seconds.
	@return Returns if the function execution passed or failed.
*/
uint8_t waitMs(uint32_t wait_ms)
{
    afeLogSpiLog("WAITms: %d", wait_ms);
    /* TBD: User domain */
    return RET_OK;
}

static uint32_t AFE_CURRENT_LOG_LEVEL = AFE_LOG_LEVEL_INFO;

/**
    @brief Set the AFE Log Level.
    @details Sets the AFE Log Level. There are multiple levels of logging as below.<br>
            AFE_LOG_LEVEL_ERROR     0   :   Error conditions<br>
            AFE_LOG_LEVEL_WARNING   1   :   warning conditions<br>
            AFE_LOG_LEVEL_INFO      2   :   informational<br>
            AFE_LOG_LEVEL_SPILOG    3   :   SPI-level messages<br>
            AFE_LOG_LEVEL_DEBUG     4   :   debug-level messages<br>
    @param level Log level.
*/
void setAfeLogLvl(uint32_t level)
{
    if (level > AFE_LOG_LEVEL_DEBUG)
        return;
    AFE_CURRENT_LOG_LEVEL = level;
    return;
}

/**
    @brief Get the AFE Log Level.
    @details Returns the AFE Log Level. There are multiple levels of logging as below.<br>
            AFE_LOG_LEVEL_ERROR     0   :   Error conditions<br>
            AFE_LOG_LEVEL_WARNING   1   :   warning conditions<br>
            AFE_LOG_LEVEL_INFO      2   :   informational<br>
            AFE_LOG_LEVEL_SPILOG    3   :   SPI-level messages<br>
            AFE_LOG_LEVEL_DEBUG     4   :   debug-level messages<br>
    @return Returns the AFE Log Level.
*/
uint32_t getAfeLogLvl()
{
    return AFE_CURRENT_LOG_LEVEL;
}

/**
    @brief Logging function.
    @details The contents of this function should be replaced by host driver function.<br>
        Can handle different log levels differently.
    @param level This logger level of the caller function.
    @return Returns the AFE Log Level.
*/
void afeLogmsg(uint32_t level, const char *pcLogFmt, ...)
{
    va_list arg;
    va_start(arg, pcLogFmt);
    if (level <= AFE_CURRENT_LOG_LEVEL)
    {
        vprintf(pcLogFmt, arg);
    }
    va_end(arg);
}
