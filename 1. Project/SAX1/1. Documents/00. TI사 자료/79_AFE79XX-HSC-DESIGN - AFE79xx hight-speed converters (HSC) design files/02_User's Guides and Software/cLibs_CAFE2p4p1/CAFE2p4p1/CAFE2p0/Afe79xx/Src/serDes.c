/** @file serDes.c
 * 	@brief	This file has SerDes related functions.<br>
 * 		<b> Version 2.2:</b> <br>
 * 		1. Fixed a bug in getSerDesEye function.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 * 		2. Updated function definition of: getSerdesEye.<br>
 * 		3. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
 * 		4. Added functions resetSerDesDfeLane, reAdaptSerDesLane, resetSerDesDfeAllLanes and reAdaptSerDesAllLanes.<br>
 */

#include <stdint.h>
#include "afe79xxLog.h"
#include "afe79xxTypes.h"
#include "afeCommonMacros.h"
#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeParameters.h"
#include "serDes.h"

/**
    @brief Send 1010 toggling pattern on AFE SerDes TX.
    @details Send 1010 toggling pattern on AFE SerDes TX.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to STX1-STX8, the physical SerDes lanes.
	@return Returns if the function execution passed or failed.
*/
uint8_t serdesTx1010Pattern(uint8_t afeId, uint8_t laneNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x0, 0xf, 0xf));					/*TX_TEST_DATA_SOURCE*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x2000, 0xd, 0xd));					/*TX_TEST_EN*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x0, 0xe, 0xe));					/*TX_PRBS_CLOCK_EN*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a1, laneNo, 0xaaaa, 0x0, 0xf));
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a2, laneNo, 0xaaaa, 0x0, 0xf));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Send JESD data on AFE SerDes TX.
    @details Send JESD data pattern on AFE SerDes TX.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to STX1-STX8, the physical SerDes lanes.
	@return Returns if the function execution passed or failed.
*/
uint8_t serdesTxSendData(uint8_t afeId, uint8_t laneNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x0, 0xd, 0xd));					/*TX_TEST_EN*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x0, 0xe, 0xe));					/*TX_PRBS_CLOCK_EN*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, 0x0, 0xf, 0xf));					/*TX_TEST_DATA_SOURCE*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set SerDes TX Cursor.
    @details Set SerDes TX Cursor. Below table shows the mapping between different settings and the equalization it provides.<br>
		Column (1):Pre-Cursor equalization acheived. (dB in relative to post cursor)	<br>
		Column (2):Main Cursor equalization acheived.(dB in relative to pre cursor)<br>
		Column (3):Post-Cursor equalization acheived.(dB in relative to pre cursor)<br>
		Column (4):Pre-Cursor Setting to be programmed.<br>
		Column (5):Main Setting to be programmed.<br>
		Column (6):Post-Cursor setting to be programmed.<br><br>
		(1)________(2)________(3)________(4)________(5)________(6)<br>
		0________25________0________0________0________0<br>
		0________23________0________0________1________0<br>
		0________21________0________0________2________0<br>
		0________19________0________0________3________0<br>
		0________17________0________0________4________0<br>
		0________15________0________0________5________0<br>
		0________13________0________0________6________0<br>
		0________11________0________0________7________0<br>
		0________24________0.72________0________0________1<br>
		0________20________0.87________0________2________1<br>
		0________16________1.09________0________4________1<br>
		0________12________1.45________0________6________1<br>
		0________23________1.51________0________0________2<br>
		0________21________1.66________0________1________2<br>
		0________15________2.33________0________4________2<br>
		0________22________2.38________0________0________3<br>
		0________13________2.69________0________5________2<br>
		0________21________3.35________0________0________4<br>
		0________19________3.71________0________1________4<br>
		0________14________3.78________0________4________3<br>
		0________17________4.17________0________2________2<br>
		0________20________4.44________0________0________5<br>
		0________15________4.75________0________3________2<br>
		0________16________5.62________0________2________5<br>
		0________19________5.68________0________0________6<br>
		0________17________6.41________0________1________6<br>
		0________18________7.13________0________0________7<br>
		0.72________24________0________1________0________0<br>
		0.87________20________0________1________2________0<br>
		0.87________22________1.66________1________0________2<br>
		1.09________16________0________1________4________0<br>
		1.09________20________3.71________1________0________4<br>
		1.45________12________0________1________2________0<br>
		1.45________14________2.69________1________4________2<br>
		1.45________16________4.75________1________2________4<br>
		1.45________18________6.41________1________0________6<br>
		1.51________23________0________2________0________0<br>
		1.66________21________0________2________1________0<br>
		1.66________22________0.87________2________0________1<br>
		2.33________15________0________2________4________0<br>
		2.33________19________4.17________2________0________4<br>
		2.38________22________0________3________0________0<br>
		2.69________18________5.62________2________0________5<br>
		2.69________13________0________2________5________0<br>
		2.69________14________1.45________2________4________1<br>
		2.69________17________4.75________2________1________4<br>
		3.35________21________0________4________0________0<br>
		3.71________19________0________4________1________0<br>
		3.71________20________1.09________4________0________1<br>
		3.78________18________4.75________3________0________4<br>
		3.78________14________0________3________4________0<br>
		4.17________17________0________4________2________0<br>
		4.17________19________2.33________4________0________2<br>
		4.44________20________0________5________0________0<br>
		4.75________15________0________4________3________0<br>
		4.75________16________1.45________4________2________1<br>
		4.75________18________3.78________4________0________3<br>
		4.75________17________2.69________4________1________2<br>
		5.62________16________0________5________2________0<br>
		5.62________18________2.69________5________0________2<br>
		5.68________19________0________6________0________0<br>
		6.41________18________1.45________6________0________1<br>
		6.41________17________0________6________1________0<br>
		7.13________18________0________7________0________0
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to STX1-STX8, the physical SerDes lanes.
	@param mainCursorSetting Main Cursor Setting.
	@param preCursorSetting Pre Cursor Setting.
	@param postCursorSetting Post Cursor Setting.
	@return Returns if the function execution passed or failed.
*/
uint8_t SetSerdesTxCursor(uint8_t afeId, uint8_t laneNo, uint8_t mainCursorSetting, uint8_t preCursorSetting, uint8_t postCursorSetting)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint32_t cursorSetting;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	cursorSetting = (mainCursorSetting << 5) + (postCursorSetting << 11) + (preCursorSetting << 8);
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80f6, laneNo, cursorSetting, 0x5, 0xd));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the AFE SerDes RX PRBS error.
    @details Reads the AFE SerDes RX PRBS error and returns the error value as pointer.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@param errorRegValue PRBS error register value. This value increments by 3 for each PRBS error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getSerdesRxPrbsError(uint8_t afeId, uint8_t laneNo, uint32_t *errorRegValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_PARAMS_VALID(errorRegValue != NULL);
	uint16_t readValue;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x804c, laneNo, 0, 0xf, &readValue));
	*errorRegValue = readValue << 16;
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x804d, laneNo, 0, 0xf, &readValue));
	*errorRegValue = *errorRegValue + readValue;
	*errorRegValue = *errorRegValue / 3;
	afeLogInfo("Number of Errors seen in lane %d are %d", laneNo, *errorRegValue);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clear the AFE SerDes RX PRBS error counter.
    @details Clearss the AFE SerDes RX PRBS error counter.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@return Returns if the function execution passed or failed.
*/
uint8_t clearSerdesRxPrbsErrorCounter(uint8_t afeId, uint8_t laneNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8042, laneNo, 0x1, 0x5, 0x5));
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8042, laneNo, 0x0, 0x5, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Enables the AFE SerDes RX PRBS check.
    @details Enables the AFE SerDes RX PRBS check.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@param prbsMode PRBS Mode Selection. 0 for PRBS9, 1 for PRBS15, 2 for PRBS23 and 3 for PRBS31.
	@param enable 1 will enable the PRBS check, 0 will disable the PRBS check.
	@return Returns if the function execution passed or failed.
*/
uint8_t enableSerdesRxPrbsCheck(uint8_t afeId, uint8_t laneNo, uint8_t prbsMode, uint8_t enable)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint16_t readValue = 0, writeValue = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x8042, laneNo, 0, 0xf, &readValue));

	writeValue = (readValue & 0xffd1) | ((prbsMode & 3) << 2) | ((enable & 1) << 1) | (1 << 5);
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8042, laneNo, writeValue, 0, 0xf));
	writeValue = (readValue & 0xffd1) + ((prbsMode & 3) << 2) + ((enable & 1) << 1) + (0 << 5);
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8042, laneNo, writeValue, 0, 0xf));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Sends the AFE SerDes TX PRBS pattern.
    @details Sends the AFE SerDes TX PRBS pattern.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to STX1-STX8, the physical SerDes lanes.
	@param prbsMode PRBS Mode Selection. 0 for PRBS9, 1 for PRBS15, 2 for PRBS23 and 3 for PRBS31.
	@param enable 1 will enable the PRBS transmission, 0 will disable the PRBS pattern transmission.
	@return Returns if the function execution passed or failed.
*/
uint8_t sendSerdesTxPrbs(uint8_t afeId, uint8_t laneNo, uint8_t prbsMode, uint8_t enable)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint16_t readValue = 0;
	uint32_t writeValue = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x80a0, laneNo, 0, 0xf, &readValue));
	writeValue = (readValue & 0x14ff) | ((prbsMode & 3) << 8) | ((enable & 1) << 11) | ((enable & 1) << 13) | ((enable & 1) << 14) | ((enable & 1) << 15);
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x80a0, laneNo, writeValue, 0, 0xf));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the AFE SerDes RX Eye margin value.
    @details Reads the AFE SerDes RX Eye margin value and returns the value as a pointer. This value*0.5 is the eye margin in mV post equalization.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@param regValue Eye Margin value status.
	@return Returns if the function execution passed or failed.
*/
uint8_t getSerdesRxLaneEyeMarginValue(uint8_t afeId, uint8_t laneNo, uint16_t *regValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x8030, laneNo, 0, 11, regValue));
	afeLogInfo("Eye Margin Value read out is on lane %d is %d", laneNo, *regValue);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets the AFE SerDes RX DFE lane.
    @details Resets the AFE SerDes RX DFE lane.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@return Returns if the function execution passed or failed.
*/
uint8_t resetSerDesDfeLane(uint8_t afeId, uint8_t laneNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint16_t readValue = 0;
	uint32_t writeValue = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (laneNo >> 2))) << 5) & 0xff, 0x0, 0x7)); /*serdes_jesd*/
	AFE_FUNC_EXEC(serdesLaneReadWrapper(afeId, 0x8000, laneNo, 0, 0xf, &readValue));
	writeValue = (readValue & 0x7fff);
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8000, laneNo, writeValue | 0x8000, 0, 0xf));
	AFE_FUNC_EXEC(serdesLaneWriteWrapper(afeId, 0x8000, laneNo, writeValue, 0, 0xf));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Readapts the AFE SerDes RX lane.
    @details Readapts the AFE SerDes RX lane. This calls resetSerDesDfeLane within this function for the specific lane.
    @param afeId AFE ID
	@param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
	@return Returns if the function execution passed or failed.
*/
uint8_t reAdaptSerDesLane(uint8_t afeId, uint8_t laneNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_FUNC_EXEC(resetSerDesDfeLane(afeId, laneNo));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets DFE of all the AFE SerDes RX lanes.
    @details Resets DFE of all the AFE SerDes RX lanes. This calls resetSerDesDfeLane within this function for all the lanes.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t resetSerDesDfeAllLanes(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	for (uint8_t laneNo = 0; laneNo < AFE_NUM_SERDES_LANES; laneNo++)
	{
		AFE_FUNC_EXEC(resetSerDesDfeLane(afeId, laneNo));
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Readapts all the AFE SerDes RX lanes.
    @details Readapts all the AFE SerDes RX lanes. This calls reAdaptSerDesLane within this function for all the lanes.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t reAdaptSerDesAllLanes(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(resetSerDesDfeAllLanes(afeId));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the status of the SerDes Eye Read.
    @details Checks the status of the SerDes Eye Read. This function is called getSerdesEye and shouldn't be called independently.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t parse_response(uint8_t afeId, uint16_t *responseRet)
{
	uint16_t response = 0;
	uint8_t errorStatus = 0;
	while (1)
	{
		AFE_FUNC_EXEC(serdesRawRead(afeId, 0x9815, &response));
		if (response >> 12 == 0)
		{
			break;
		}
	}
	uint8_t status = (response >> 8) & 0xf;
	uint8_t data = response & 0xff;
	if (status == 0x3)
	{
		if (data == 0x02)
		{
			afeLogErr("%s", "Invalid input");
			errorStatus |= 1;
		}
		else if (data == 0x03)
		{
			afeLogErr("%s", "Phy not ready");
			errorStatus |= 1;
		}
		else if (data == 0x05)
		{
			afeLogInfo("%s", "Eye monitor going on");
		}
		else if (data == 0x06)
		{
			afeLogErr("%s", "Eye monitor cancelled");
		}
		else if (data == 0x07)
		{
			afeLogErr("%s", "Eye monitor not started");
			errorStatus |= 1;
		}
		else
		{
			afeLogInfo("%d", data);
		}
	}
	*responseRet = response;
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Initiates the SerDes Eye Read.
    @details Initiates the SerDes Eye Read. This function is called getSerdesEye and shouldn't be called independently.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t em_start(uint8_t afeId, uint8_t lane_num, uint8_t ber_exp, uint8_t mode)
{
	// uint8_t instanceNo = lane_num >> 2;
	uint8_t errorStatus = 0;
	uint16_t response = 0;
	// uint8_t jesdToSerdesLaneMappingLocal[8] = jesdToSerdesLaneMapping;
	// lane_num = jesdToSerdesLaneMappingLocal[lane_num];
	uint16_t command = 0x1000 | (lane_num & 0x3) | ((ber_exp & 0xF) << 4);
	AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9816, mode));
	AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9815, command));
	AFE_FUNC_EXEC(parse_response(afeId, &response));
	uint8_t status = (response >> 8) & 0xf;

	if (status == 0x0)
	{
		afeLogInfo("%s", "Eye monitor starts...");
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the Progress of the SerDes Eye Read.
    @details Checks the Progress of the SerDes Eye Read. This function is called getSerdesEye and shouldn't be called independently.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t em_report_progress(uint8_t afeId, uint8_t *progress)
{
	uint8_t errorStatus = 0;
	uint16_t command = 0x2000;
	uint16_t response = 0;
	AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9815, command));
	AFE_FUNC_EXEC(parse_response(afeId, &response));
	uint8_t status = (response >> 8) & 0xf;
	uint8_t data = response & 0xff;
	if (status == 0x1)
	{
		*progress = data;
	}
	else
	{
		*progress = 0xff;
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the SerDes Eye parameters.
    @details Reads the SerDes Eye parameters. This function is called getSerdesEye and shouldn't be called independently.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t em_read(uint8_t afeId, uint16_t *ber)
{
	uint8_t errorStatus = 0;
	uint16_t response = 0;
	uint8_t m = 0;
	uint8_t status = 0;
	for (int8_t phase = -16; phase < 17; phase++)
	{
		uint8_t pindex = phase + 16;
		for (int16_t margin = -47; margin < 48; margin += 16)
		{
			uint16_t marginValue;
			uint16_t phaseValue;
			uint16_t serdesReadVal;
			if (margin < 0)
			{
				marginValue = 65536 + margin;
			}
			else
			{
				marginValue = margin;
			}
			if (phase < 0)
			{
				phaseValue = 65536 + phase;
			}
			else
			{
				phaseValue = phase;
			}
			AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9816, marginValue & 0xffff));
			AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9815, (phaseValue & 0xFF) | 0x3000));
			AFE_FUNC_EXEC(parse_response(afeId, &response));
			status = (response >> 8) & 0xf;

			if (status == 0x2)
			{
				for (uint8_t i = 0; i < 16; i++)
				{
					m = margin + i;
					if (m < 48)
					{
						AFE_FUNC_EXEC(serdesRawRead(afeId, (0x9f00 + i), &serdesReadVal));
						ber[(47 + m) + (pindex * 95)] = serdesReadVal;
					}
				}
			}
			else
			{
				for (uint8_t i = 0; i < 16; i++)
				{
					m = margin + i;
					if (m < 48)
					{
						ber[(47 + m) + (pindex * 95)] = 0;
					}
				}
			}
			afeLogInfo("%d", ber[(47 + m) + (pindex * 95)]);
		}
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Stops the the SerDes Eye Read.
    @details Stops the the SerDes Eye Read. This function is called getSerdesEye and shouldn't be called independently.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t em_cancel(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(serdesRawWrite(afeId, 0x9815, 0x4000));
	uint16_t response = 0;
	AFE_FUNC_EXEC(parse_response(afeId, &response));

	afeLogInfo("%d", response);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the SerDes Eye for a given lane.
    @details Reads the SerDes Eye for a given lane. The ber array and the extent returned by this function should be fed to the python script to plot the eye diagram.
    @param afeId AFE ID
    @param laneNo Values 0-7, refer to SRX1-SRX8, the physical SerDes lanes.
    @param ber Pointer of array with 3135 elements.
    @param extent scaling factor of the ber needed by the function.
	@return Returns if the function execution passed or failed.
*/
uint8_t getSerdesEye(uint8_t afeId, uint8_t laneNo, uint16_t *ber, uint16_t *extent)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	AFE_PARAMS_VALID(ber != NULL);
	AFE_PARAMS_VALID(extent != NULL);
	//uint16_t extent;
	uint8_t jesdToSerdesLaneMappingLocal[8] = jesdToSerdesLaneMapping;
	uint8_t instanceNo = laneNo >> 2;
	laneNo = jesdToSerdesLaneMappingLocal[laneNo];
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x20 << instanceNo, 0x0, 0x7));
	AFE_FUNC_EXEC(em_start(afeId, laneNo, 7, 1)); /*lane0, ber_exp 10^7, mode 1*/
	uint8_t old_progress = 0;
	while (1)
	{
		uint8_t progress = 0;
		AFE_FUNC_EXEC(em_report_progress(afeId, &progress));
		if (progress == 0xff)
		{
			break;
		}
		if (old_progress != progress)
		{
			afeLogInfo("%d", progress);
		}
		if (progress == 100)
		{
			break;
		}
		old_progress = progress;
	}
	AFE_FUNC_EXEC(em_read(afeId, ber));

	AFE_FUNC_EXEC(serdesRawRead(afeId, 0x9816, extent));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x00, 0x0, 0x7));
	afeLogInfo("Extent: %d", *extent);
	/*getSerdesEye*/
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
