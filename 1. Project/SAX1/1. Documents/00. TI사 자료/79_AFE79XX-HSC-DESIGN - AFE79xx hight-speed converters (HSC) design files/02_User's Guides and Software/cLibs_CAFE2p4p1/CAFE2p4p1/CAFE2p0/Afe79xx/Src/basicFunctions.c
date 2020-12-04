/** @file basicFunctions.c
 * 	@brief	This file has Basic SPI functions.<br>
 * 		<b> Version 2.2:</b> <br>
 * 		1. Updated the log comment in serdesRawWrite function.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 *      2. Changed the C macros for all the spi wrapper function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
 *      3. Added closeAllPages function.<br>
*/
#include <stdint.h>
#include <stdio.h>

#include "afe79xxTypes.h"
#include "afe79xxLog.h"

#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeParameters.h"

#define MASK_BYTE(lsb, msb) (uint8_t)(((1 << ((msb) - (lsb) + 1)) - 1) << lsb)
#define MASK_SHORT(lsb, msb) (uint16_t)(((1 << ((msb) - (lsb) + 1)) - 1) << lsb)
#define CFG_SPI_READ_POLL_MAX_COUNT 500
#define AFE_REQ_SPI_ACCESS_MAX_COUNT 100
static const uint16_t jesdToSerdesLaneMappingLocal[8] = jesdToSerdesLaneMapping;

/**
		@brief SerDes Read
		@details SerDes registers are 16-bit wide while SPI is 8-bit. This necessitates a translation between SPI and SerDes. This function reads SerDes registers and returns the read value as a pointer.
		@param afeId AFE ID
		@param addr SerDes address
		@param readVal Pointer returning the read value 
		@return Returns if the function execution passed or failed.
*/
uint8_t serdesRawRead(uint8_t afeId, uint16_t addr, uint16_t *readVal)
{
    uint8_t errorStatus = 0;
    uint8_t ucValueHigh = 0;
    uint8_t ucValueLow = 0;
    uint16_t usAddr = (uint16_t)(((addr + 0x2000) & 0x3fff) << 1);

    AFE_PARAMS_VALID(readVal != NULL)

    /* It is important to read each Byte twice, but only the second matters. */
    AFE_SPI_EXEC(dev_spi_read(afeId, (usAddr + 1), &ucValueHigh));
    AFE_SPI_EXEC(dev_spi_read(afeId, (usAddr + 1), &ucValueHigh));
    AFE_SPI_EXEC(dev_spi_read(afeId, usAddr, &ucValueLow));
    AFE_SPI_EXEC(dev_spi_read(afeId, usAddr, &ucValueLow));

    *readVal = ((uint16_t)ucValueHigh << 8) | (uint16_t)ucValueLow;
    afeLogSpiLog("SerDes Raw READ: AFEID:%d: ADDR: 0X%X, Read Val: 0X%X", afeId, addr, *readVal);
    return RET_OK;
}

/**
		@brief SerDes Write
		@details SerDes registers are 16-bit wide while SPI is 8-bit. This necessitates a translation between SPI and SerDes. This function writes SerDes registers.
		@param afeId AFE ID
		@param addr SerDes address
		@param data Value to be written.
		@return Returns if the function execution passed or failed.
*/
uint8_t serdesRawWrite(uint8_t afeId, uint16_t addr, uint16_t data)
{
    uint8_t errorStatus = 0;
    afeLogSpiLog("SerDes Raw Write: AFEID:%d: ADDR: 0X%X, Write Val: 0X%X", afeId, addr, data);
    AFE_SPI_EXEC(dev_spi_write(afeId, (((addr + 0x2000) << 1) + 1) & 0x7fff, (uint8_t)((data >> 8) & 0xFF)));
    AFE_SPI_EXEC(dev_spi_write(afeId, ((addr + 0x2000) << 1) & 0x7fff, (uint8_t)(data & 0xff)));
    return RET_OK;
}

/**
		@brief SPI Write Wrapper
		@details Writes the value to the specified bits of the register.
		@param afeId AFE ID
		@param addr SPI address
		@param data Value to be written.
		@param lsb lsb of the field.
		@param msb msb of the field.
		@return Returns if the function execution passed or failed.
*/
uint8_t afeSpiWriteWrapper(uint8_t afeId, uint16_t addr, uint8_t data, uint8_t lsb, uint8_t msb)
{
    uint8_t errorStatus = 0;
    uint8_t readValue = 0;
    uint8_t writeValue = 0;
    uint8_t mask = 0;

    AFE_PARAMS_VALID((msb < 8) && (lsb <= msb));
    afeLogSpiLog("WRITE: afeId: %d, addr: 0x%X, data: 0x%X, lsb: %d, msb: %d", afeId, addr, data, lsb, msb);
    if ((msb == 7) && (lsb == 0))
    {
        AFE_SPI_EXEC(dev_spi_write(afeId, addr, data));
        return RET_OK;
    }

    AFE_SPI_EXEC(dev_spi_read(afeId, addr, &readValue));
    mask = MASK_BYTE(lsb, msb);
    writeValue = (readValue & (0xFF ^ mask)) | (data & mask);
    AFE_SPI_EXEC(dev_spi_write(afeId, addr, writeValue));

    return RET_OK;
}

/**
		@brief SPI Read Wrapper
		@details Reads the value to the specified bits of the register and returns as a pointer.
		@param afeId AFE ID
		@param addr SPI address
		@param lsb lsb of the field.
		@param msb msb of the field.
		@param readVal pointer of the read value.
		@return Returns if the function execution passed or failed.
*/
uint8_t afeSpiReadWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t *readVal)
{
    uint8_t errorStatus = 0;
    uint8_t readValue = 0;

    AFE_PARAMS_VALID((msb < 8) && (lsb <= msb));
    AFE_PARAMS_VALID(readVal != NULL)

    AFE_SPI_EXEC(dev_spi_read(afeId, addr, &readValue));
    *readVal = (readValue & MASK_BYTE(lsb, msb)) >> lsb;
    afeLogSpiLog("READ: afeId: %d, addr: 0x%X, Read Value: 0x%X, lsb: %d, msb: %d", afeId, addr, *readVal, lsb, msb);
    return RET_OK;
}

/**
		@brief SerDes Write Wrapper
		@details Writes the value to the specified bits of the SerDes register.
		@param afeId AFE ID
		@param addr SerDes address
		@param data Value to be written.
		@param lsb lsb of the field.
		@param msb msb of the field.
		@return Returns if the function execution passed or failed.
*/
uint8_t serdesWriteWrapper(uint8_t afeId, uint16_t addr, uint16_t data, uint8_t lsb, uint8_t msb)
{
    uint8_t errorStatus = 0;
    uint16_t readValue = 0;
    uint16_t writeValue = 0;
    uint16_t mask = 0;

    AFE_PARAMS_VALID((msb < 16) && (lsb <= msb));
    afeLogSpiLog("SerDes WRITE: afeId: %d, addr: 0x%X, data: 0x%X, lsb: %d, msb: %d", afeId, addr, data, lsb, msb);
    if ((msb == 15) && (lsb == 0))
    {
        AFE_SPI_EXEC(serdesRawWrite(afeId, addr, data));
        return RET_OK;
    }

    AFE_FUNC_EXEC(serdesRawRead(afeId, addr, &readValue));
    mask = MASK_SHORT(lsb, msb);
    writeValue = (readValue & (0xFFFF ^ mask)) | (data & mask);
    AFE_SPI_EXEC(serdesRawWrite(afeId, addr, writeValue));
    return RET_OK;
}

/**
		@brief SerDes Read Wrapper
		@details Reads the value to the specified bits of the SerDes register and returns as a pointer.
		@param afeId AFE ID
		@param addr SerDes address
		@param lsb lsb of the field.
		@param msb msb of the field.
		@param readVal Pointer of the value to be written.
		@return Returns if the function execution passed or failed.
*/
uint8_t serdesReadWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint16_t *readVal)
{
    uint8_t errorStatus = 0;
    uint16_t readValue = 0;

    AFE_PARAMS_VALID((msb < 16) && (lsb <= msb));
    AFE_PARAMS_VALID(readVal != NULL)

    AFE_FUNC_EXEC(serdesRawRead(afeId, addr, &readValue));

    *readVal = (readValue & MASK_SHORT(lsb, msb)) >> lsb;
    afeLogSpiLog("SerDes READ: afeId: %d, addr: 0x%X, Read Value: 0x%X, lsb: %d, msb: %d", afeId, addr, *readVal, lsb, msb);

    return RET_OK;
}

/**
    @brief SerDes Lane Write Wrapper
    @details Writes the value to the specified bits of the SerDes lane register of the corresponding lane by adding the appropriate offset.
    @param afeId AFE ID
    @param addr SerDes lane base address
    @param laneNo SerDes lane Number. Values supported are: 0-7.
    @param data Value to be written.
    @param lsb lsb of the field.
    @param msb msb of the field.
    @return Returns if the function execution passed or failed.
*/
uint8_t serdesLaneWriteWrapper(uint8_t afeId, uint16_t addr, uint8_t laneNo, uint16_t data, uint8_t lsb, uint8_t msb)
{
    uint8_t errorStatus = 0;
    uint16_t readValue = 0;
    uint16_t writeValue = 0;
    uint16_t mask = 0;
    uint16_t usAddr = 0;

    AFE_PARAMS_VALID((msb < 16) && (lsb <= msb));
    AFE_PARAMS_VALID(laneNo < ARRAY_SIZE(jesdToSerdesLaneMappingLocal));

    usAddr = addr + (0x100 * jesdToSerdesLaneMappingLocal[laneNo]);
    if (msb == 15 && lsb == 0)
    {
        AFE_SPI_EXEC(serdesRawWrite(afeId, usAddr, data));
        return RET_OK;
    }

    AFE_FUNC_EXEC(serdesRawRead(afeId, usAddr, &readValue));
    mask = MASK_SHORT(lsb, msb);
    writeValue = (readValue & (0xFFFF ^ mask)) | (data & mask);
    AFE_SPI_EXEC(serdesRawWrite(afeId, usAddr, writeValue));
    return RET_OK;
}

/**
    @brief SerDes Lane Read Wrapper
    @details Reads the value to the specified bits of the SerDes lane register of the corresponding lane by adding the appropriate offset. Returns the value as pointer.
    @param afeId AFE ID
    @param addr SerDes lane base address
    @param laneNo SerDes lane number. 0-7 is the supported range.
    @param lsb lsb of the field.
    @param msb msb of the field.
	@param readVal Pointer of the value to be written.
    @return Returns if the function execution passed or failed.
*/
uint8_t serdesLaneReadWrapper(uint8_t afeId, uint16_t addr, uint32_t laneNo, uint8_t lsb, uint8_t msb, uint16_t *readVal)
{
    uint8_t errorStatus = 0;
    uint16_t readValue = 0;
    uint16_t usAddr = 0;

    AFE_PARAMS_VALID((msb < 16) && (lsb <= msb));
    AFE_PARAMS_VALID(laneNo < ARRAY_SIZE(jesdToSerdesLaneMappingLocal));
    AFE_PARAMS_VALID(readVal != NULL)

    usAddr = addr + (0x100 * jesdToSerdesLaneMappingLocal[laneNo]);
    AFE_FUNC_EXEC(serdesRawRead(afeId, usAddr, &readValue));

    *readVal = (readValue & MASK_SHORT(lsb, msb)) >> lsb;
    return RET_OK;
}

/**
    @brief AFE SPI Check Wrapper
    @details Reads and checks if the value of the field is as expected. Check Pass condition is (readValue&mask)==(data&mask) where mask = (((1 << ((msb) - (lsb) + 1)) - 1) << lsb);
    @param afeId AFE ID
    @param addr SPI address
    @param data Expected Value.
    @param lsb lsb of the field.
    @param msb msb of the field.
    @param pbSame Pointer return. Returns 0 if the check passes and if check fails.
    @return Returns if the function execution passed or failed.
*/
uint8_t afeSpiCheckWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t data, uint8_t *pbSame)
{
    uint8_t errorStatus = 0;
    uint8_t readValue = 0;
    uint8_t mask = 0;

    AFE_PARAMS_VALID((msb < 8) && (lsb <= msb));
    AFE_PARAMS_VALID(pbSame != NULL)

    AFE_SPI_EXEC(dev_spi_read(afeId, addr, &readValue));

    mask = MASK_BYTE(lsb, msb);
    afeLogSpiLog("READ Check: afeId: %d, addr: 0x%X, lsb: %d, msb: %d, Read Value: 0x%X, Expected Value:0x%X", afeId, addr, lsb, msb, readValue & mask, data & mask);
    if ((readValue & mask) != (data & mask))
    {
        *pbSame = 1;
        afeLogInfo("addr[0x%04X], lsb[%d], msb[%d], data[0x%02X] not matching with expected value[0x%02X]", addr, lsb, msb, readValue, data);
    }
    else
        *pbSame = 0;
    afeLogSpiLog("READ Check: afeId: %d, addr: 0x%X, lsb: %d, msb: %d, Read Value: 0x%X, Expected Value:0x%X", afeId, addr, lsb, msb, readValue & mask, data & mask);
    return RET_OK;
}

/**
    @brief AFE SPI Poll Wrapper
    @details Polls and checks if the value of the field is as expected. Check Pass condition is (readValue&mask)==(data&mask) where mask = (((1 << ((msb) - (lsb) + 1)) - 1) << lsb);
    @param afeId AFE ID
    @param addr SPI address
    @param expectedData Expected Value.
    @param lsb lsb of the field.
    @param msb msb of the field.
    @return Returns if the function execution passed or failed. It returns fail even when the read data didn't match the expected value.
*/
uint8_t afeSpiPollWrapper(uint8_t afeId, uint16_t addr, uint8_t expectedData, uint8_t lsb, uint8_t msb)
{
    uint8_t errorStatus = 0;
    uint32_t count = 0;
    uint8_t mask = 0;
    uint8_t readValue = 0;

    AFE_PARAMS_VALID((msb < 8) && (lsb <= msb));

    mask = MASK_BYTE(lsb, msb);
    afeLogSpiLog("Poll: afeId: %d, addr: 0x%X, lsb: %d, msb: %d, Expected Value:0x%X", afeId, addr, lsb, msb, expectedData);

    for (count = 0; count < CFG_SPI_READ_POLL_MAX_COUNT; count++)
    {
        AFE_SPI_EXEC(dev_spi_read(afeId, addr, &readValue));
        if ((readValue & mask) == (expectedData & mask))
            break;
        AFE_FUNC_EXEC(waitMs(2));
    }

    if (count >= CFG_SPI_READ_POLL_MAX_COUNT)
    {
        afeLogErr("%s", "timeout! ");
        return RET_EXEC_FAIL;
    }
    return RET_OK;
}

/**
    @brief AFE SPI Poll Wrapper
    @details Polls and checks if the value of the field is as expected. Check Pass condition is (readValue&mask)==(data&mask) where mask = (((1 << ((msb) - (lsb) + 1)) - 1) << lsb); Function definition reordered from afeSpiPollWrapper to suit the log format.
    @param afeId AFE ID
    @param addr SPI address
    @param expectedData Expected Value.
    @param lsb lsb of the field.
    @param msb msb of the field.
    @return Returns if the function execution passed or failed. It returns fail even when the read data didn't match the expected value.
*/
uint8_t afeSpiPollLogWrapper(uint8_t afeId, uint16_t addr, uint8_t lsb, uint8_t msb, uint8_t expectedData)
{
    return afeSpiPollWrapper(afeId, addr, expectedData, lsb, msb);
}

typedef enum PLL_SPI_REG_TYPE
{
    PLL_SPI_REG_OFF = 0,
    PLL_SPI_REG_A,
    PLL_SPI_REG_B,

    PLL_SPI_REG_TYPE_SIZE
} PllSpiRegType_e;

/**
    @brief Requesting PLL Spi Access
    @details For access PLL registers, the access to the PLL page should be requested and we should proceed only after it is granted. After the access is complete, the SPI access should be. This function does these operations. This access is independent for SPIA and SPIB.
    @param afeId AFE ID
    @param regType 0-Relinquish SPI access<br>
                1- Request Access for SPIA
                2- Request Access for SPIB
    @return Returns if the function execution passed or failed. It returns fail even when the request has not been granted.
*/
uint8_t requestPllSpiAccess(uint8_t afeId, uint32_t regType)
{
    uint32_t count = 0;
    uint8_t readVal = 0;
    uint8_t errorStatus = 0;
    AFE_PARAMS_VALID(regType < PLL_SPI_REG_TYPE_SIZE);

    /*  "Requesting/releasing SPI Access to PLL Pages"  */
    if (regType == PLL_SPI_REG_A)
    {
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x40, 0, 7)); /*digtop*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x170, 0x1, 0, 0)); /*pll_reg_spi_req_a*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x540, 0x0, 0, 0));
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0, 7));
        if (errorStatus != 0)
            return RET_EXEC_FAIL;

        for (count = 0; count < AFE_REQ_SPI_ACCESS_MAX_COUNT; count++)
        {
            AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x40, 0, 7)); /*digtop*/
            AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x171, 0, 0, &readVal));
            AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0, 7));
            if (errorStatus != 0)
                return RET_EXEC_FAIL;
            if (readVal == 1)
                break;
            AFE_FUNC_EXEC(waitMs(1));
        }

        if (count >= AFE_REQ_SPI_ACCESS_MAX_COUNT)
        {
            afeLogErr("%s", "SPIA didn't get control of PLL pages.");
            return RET_EXEC_FAIL;
        }
        afeLogInfo("%s", "SPIA has got control of PLL pages.");
    }
    else if (regType == PLL_SPI_REG_B)
    {
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x40, 0, 7)); /*digtop*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x170, 0x0, 0, 0)); /*pll_reg_spi_req_a*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x540, 0x1, 0, 0));
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0, 7));
        if (errorStatus != 0)
            return RET_EXEC_FAIL;

        for (count = 0; count > AFE_REQ_SPI_ACCESS_MAX_COUNT; count++)
        {
            AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x40, 0, 7)); /*digtop*/
            AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x541, 0, 0, &readVal));
            AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0, 7));
            if (errorStatus != 0)
                return RET_EXEC_FAIL;
            if (readVal == 0)
                break;
            AFE_FUNC_EXEC(waitMs(20));
        }
        if (count >= AFE_REQ_SPI_ACCESS_MAX_COUNT)
        {
            afeLogErr("%s", "SPIB didn't get control of PLL pages.");
            return RET_EXEC_FAIL;
        }
        afeLogInfo("%s", "SPIB has got control of PLL pages.");
    }
    else
    {
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x40, 0, 7)); /*digtop*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x170, 0x0, 0, 0)); /*pll_reg_spi_req_a*/
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x540, 0x0, 0, 0));
        AFE_FUNC_EXEC(waitMs(20));
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0, 7));
        if (errorStatus != 0)
            return RET_EXEC_FAIL;
        afeLogInfo("%s", "PLL Pages SPI control relinquished.");
    }

    return RET_OK;
}

static const uint32_t ausAddr[] =
    {
        0,
        0x7fe0,
        0xffc0,
        0x17fa0,
        0x1ff80,
        0x27f60,
        0x2ff40,
        0x37f20,
        0x3ff00};

/**
    @brief Reads the MCU Memory
    @details This reads the MCU memory and returns the value as a pointer.
    @param afeId AFE ID
    @param addr Memory Address.
    @param readVal Value read returned as a pointer.
    @param noBytes Number of bytes to be read. Supported values: 0<noBytes<=8
    @return Returns if the function execution passed or failed.
*/
uint8_t readTopMem(uint8_t afeId, uint32_t addr, uint64_t *readVal, uint32_t noBytes)
{
    uint16_t addr_high = 0;
    uint16_t usAddr = 0;
    uint32_t count = 0;
    uint8_t errorStatus = 0;
    uint8_t readValue8 = 0;
    AFE_PARAMS_VALID(addr < ausAddr[ARRAY_SIZE(ausAddr) - 1]);
    AFE_PARAMS_VALID(readVal != NULL)
    AFE_PARAMS_VALID(noBytes != 0 && noBytes <= 8);
    for (count = 1; count < ARRAY_SIZE(ausAddr); count++)
    {
        if (addr < ausAddr[count])
        {
            addr_high = count - 1;
            usAddr = (uint16_t)(addr - ausAddr[count - 1]) + 0x20;
            break;
        }
    }
    *readVal = 0;
    AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0, 7));
    AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, (addr_high << 2), 2, 4));
    AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x08, 0, 7));
    for (count = 0; count < noBytes; count++)
    {
        AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, (usAddr + count), 0, 7, &readValue8));
        *readVal = *readVal | (readValue8 << (8 * count));
        if (errorStatus != 0)
            break;
    }
    AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0x0, 0x7));
    if (errorStatus != 0)
        return RET_EXEC_FAIL;
    else
        return RET_OK;
}

/**
    @brief Close All Pages
    @details This function closes all the pages. Need to be called in case of a SPI/function to ensure no open page is present.
    @param afeId AFE ID
    @return Returns if the function execution passed or failed.
*/
uint8_t closeAllPages(uint8_t afeId)
{
    uint8_t errorStatus = 0;
    for (uint8_t addr = AFE_PAGE_START_ADDR; addr <= AFE_PAGE_END_ADDR; addr += 1)
    {
        AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, addr, 0x00, 0, 7));
    }
    if (errorStatus != 0)
        return RET_EXEC_FAIL;
    else
        return RET_OK;
}
