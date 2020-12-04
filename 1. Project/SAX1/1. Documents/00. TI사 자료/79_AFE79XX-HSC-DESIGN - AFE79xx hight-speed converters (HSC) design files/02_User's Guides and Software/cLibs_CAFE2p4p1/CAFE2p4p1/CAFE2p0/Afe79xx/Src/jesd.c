/** @file jesd.c
 * 	@brief	This file has JESD related functions.<br>
 * 		<b> Version 2.2:</b> <br>
 * 		1. Fixed bug in adcDacSync.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 * 		2. Added function getAllLaneReady which is useful for setting the RBD.<br>
 * 		3. Fixed dacJesdConstantTestPatternValue function for higher DAC interface rates.<br>
 * 		4. Removed redundant writes in functions.<br>
 * 		5. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
 * 		6. Added functions jesdRxFullResetToggle, jesdTxFullResetToggle, jesdRxClearDataPath and jesdTxClearDataPath. Cleaned up adcDacSync by calling these sub functions.<br>
*/
////DACJESD
#include <stdint.h>
#include <afe79xxLog.h>
#include <afe79xxTypes.h>

#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeParameters.h"
#include "controls.h"

#include "jesd.h"

/**
    @brief Send JESD Data from SerDes to DAC
    @details Send JESD Data from SerDes. This should be called to change from test pattern mode to normal data mode.
    @param afeId AFE ID
	@param topno 0-AB and 1-CD.
	@return Returns if the function execution passed or failed.
*/
uint8_t dacJesdSendData(uint8_t afeId, uint8_t topno)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topno < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (topno))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb7, 0x0, 0x0, 0x2));							  /*tx_jesd_test_sig_gen_mode*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Send Constant Test Pattern to DAC
    @details Send Constant Test Pattern to DAC. This test pattern is near the JESD-DUC interface. The output of the DAC will be a single tone for each band at the mixer frequency.
    @param afeId AFE ID
	@param topno 0-AB and 1-CD.
	@param enable 0-Send Data From SERDES. 1- Send Constant Test Pattern. This is common for AB/CD.
	@param chNo  0-A/C, 1-B/D
	@param bandNo 0-Band 0, 1- Band1. In single band case, this should always be 0.
	@param valueI Value to be sent on I
	@param valueQ Value to be sent on Q
	@return Returns if the function execution passed or failed.
*/
uint8_t dacJesdConstantTestPatternValue(uint8_t afeId, uint8_t topno, uint8_t enable, uint8_t chNo, uint8_t bandNo, uint16_t valueI, uint16_t valueQ)
{
	uint8_t errorStatus = 0;
	uint8_t noOfParallelization = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topno < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(bandNo < AFE_NUM_BANDS_PER_RX);
	noOfParallelization = (float)(systemParams[afeId].FadcFb * systemParams[afeId].ducFactorTx[(topno << 1) + chNo] / systemParams[afeId].Fdac);
	if (enable == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (topno))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb7, 0x1, 0x0, 0x2));							  /*tx_jesd_test_sig_gen_mode*/

		chNo = (chNo * 2) + bandNo;
		if (chNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb9, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input0*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb8, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input0*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xbb, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input1*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xba, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input1*/
		}

		if ((chNo == 1) || (chNo == 0 && noOfParallelization >= 2))
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xbd, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input2*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xbc, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input2*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xbf, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input3*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xbe, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input3*/
		}

		if (chNo == 2)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc1, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input4*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc0, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input4*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc3, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input5*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc2, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input5*/
		}

		if ((chNo == 3) || (chNo == 2 && noOfParallelization >= 2))
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc5, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input6*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc4, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input6*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc7, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input7*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc6, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input7*/
		}

		if (chNo == 4)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc9, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input8*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc8, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input8*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcb, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input9*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xca, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input9*/
		}

		if ((chNo == 5) || (chNo == 4 && noOfParallelization >= 2))
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcd, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input10*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcc, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input10*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcf, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input11*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xce, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input11*/
		}

		if (chNo == 6)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd1, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input12*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd0, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input12*/

			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd3, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input13*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd2, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input13*/
		}

		if ((chNo == 7) || (chNo == 6 && noOfParallelization >= 2))
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd5, (valueI >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input14*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd4, (valueI)&0xff, 0x0, 0x7));		/*jesd_shorttest_input14*/

			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd7, (valueQ >> 8) & 0xff, 0x0, 0x7)); /*jesd_shorttest_input15*/
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd6, (valueQ)&0xff, 0x0, 0x7));		/*jesd_shorttest_input15*/
		}
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	}

	else
	{
		AFE_FUNC_EXEC(dacJesdSendData(afeId, topno));
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Send Ramp Test pattern to DAC
    @details Send Ramp Test pattern to DAC. This test pattern is near the JESD-DUC interface. 
    @param afeId AFE ID
	@param topno 0-AB and 1-CD.
	@param increment increment+1 is the step value of the ramp.
	@return Returns if the function execution passed or failed.
*/
uint8_t dacJesdSendRampTestPattern(uint8_t afeId, uint8_t topno, uint8_t increment)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topno < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (topno))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb7, 0x2, 0x0, 0x2));							  /*tx_jesd_test_sig_gen_mode*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb7, ((increment) << 4) & 0xff, 0x4, 0x7)); /*tx_jesd_ramptest_incr*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the DAC JESD Lane  Errors.
    @details Reads the DAC JESD Lane Errors, logs their meaning and returns the alarm status as pointer.
    @param afeId AFE ID
	@param laneNo JESD Lane Number. Note that this is the JESD Lane Number which is post JESD-SerDes Mux(towards the AFE side).
	@param error Pointer return of the status. Value of zero means there is no error and any non-zero value refers to an error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxLaneErrors(uint8_t afeId, uint8_t laneNo, uint8_t *error)
{
	/* These are link related errors */
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(error != NULL);
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint8_t ulRegValue, a;
	const char *laneErrorBits[8];
	uint8_t errorStatus = 0;

	if ((laneNo & 0x4) == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x11c + (laneNo & 0x3), 0x0, 0x7, &ulRegValue));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	*error = 0;
	laneErrorBits[7] = "multiframe alignment error";
	laneErrorBits[6] = "frame alignment error";
	laneErrorBits[5] = "link configuration error";
	laneErrorBits[4] = "elastic buffer overflow (bad RBD value)";
	laneErrorBits[3] = "elastic buffer match error. The first no-/K/ does not match 'match_ctrl' and 'match_data' programmed values";
	laneErrorBits[2] = "code synchronization error";
	laneErrorBits[1] = "JESD 204B: 8b/10b not-in-table code error. JESD 204C: sync_header_invalid_err";
	laneErrorBits[0] = "JESD 204B: 8b/10b disparty error. JESD 204C: sync_header_parity_err";
	for (a = 0; a < 8; a++)
	{
		if (((ulRegValue >> a) & 1) == 1)
		{
			afeLogErr("JESD RX Lane Error for lane %d seen: %s", laneNo, laneErrorBits[a]);
			*error |= 0x1 << a;
		}
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the DAC JESD Lane FIFO Errors.
    @details Reads the DAC JESD Lane FIFO Errors, logs their meaning and returns the alarm status as pointer. These are SerDes FIFO errors. If this error is present, either the SerDes is likely seeing some eye based issues .
    @param afeId AFE ID
	@param laneNo JESD Lane Number. Note that this is the JESD Lane Number which is post JESD-SerDes Mux(towards the AFE side).
	@param error Pointer return of the status. Value of zero means there is no error and any non-zero value refers to an error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxLaneFifoErrors(uint8_t afeId, uint8_t laneNo, uint8_t *error)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(error != NULL);
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	uint8_t ulRegValue;
	*error = 0;

	if ((laneNo & 0x4) == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x119, 0x0, 0x7, &ulRegValue));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	if (((ulRegValue >> laneNo) & 0x1) == 1)
	{
		afeLogErr("SerDes Rx Lane %d got LOSS of lock error. There may be a SerDes Eye issue.", laneNo);
		*error = 0x1;
	}
	if (((ulRegValue >> (4 + laneNo)) & 0x1) == 1)
	{
		afeLogErr("SerDes Rx Lane %d got FIFO Error. There may be a SerDes Eye issue or FIFO offset issue.", laneNo);
		*error = 0x2;
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the DAC JESD Miscellaneous Errors.
    @details Reads the DAC JESD Miscellaneous Errors, logs their meaning and returns the alarm status as pointer.
    @param afeId AFE ID
	@param jesdNo JESD Instance Number (0/1). Note that this is the JESD Lane Number which is post JESD-SerDes Mux(towards the AFE side).
	@param errorValue Pointer return of the status. Value of zero means there is no error and any non-zero value refers to an error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxMiscSerdesErrors(uint8_t afeId, uint8_t jesdNo, uint8_t *errorValue)
{
	/* jesdNo is 0 for AB and 1 for CD */
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(errorValue != NULL);
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	uint8_t regValue;
	*errorValue = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));

	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x118, 0x0, 0x7, &regValue));
	if (((regValue >> 6) & 1) == 1)
	{
		afeLogErr("%s", "SerDes AB PLL Loss of Lock");
		*errorValue |= 0x1;
	}
	if (((regValue >> 7) & 1) == 1)
	{
		afeLogErr("%s", "SerDes CD PLL Loss of Lock");
		*errorValue |= 0x2;
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read all the DAC JESD Errors.
    @details Reads all the DAC JESD Errors, logs their meaning and returns the alarm status as pointer.
    @param afeId AFE ID
	@param error Pointer return of the status. Value of zero means there is no error and any non-zero value refers to an error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxAlarms(uint8_t afeId, uint8_t *error)
{
	uint8_t errorStatus = 0;
	uint8_t laneNo, topNo;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(error != NULL);
	for (laneNo = 0; laneNo < 8; laneNo = laneNo + 1)
	{
		AFE_FUNC_EXEC(getJesdRxLaneErrors(afeId, laneNo, error));
		AFE_FUNC_EXEC(getJesdRxLaneFifoErrors(afeId, laneNo, error));
	}

	for (topNo = 0; topNo < 2; topNo = topNo + 1)
	{
		AFE_FUNC_EXEC(getJesdRxMiscSerdesErrors(afeId, topNo, error));
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read Link Status for DAC JESD.
    @details Reads link status for DAC JESD for all the enabled lanes and returns it. This calls functions getJesdRxLinkStatus204B/getJesdRxLinkStatus204C based on set systemParams[afeId].jesdProtocol.
    @param afeId AFE ID
	@param linkStatus Pointer return of the status.<br>
				Return Value is 4 bits. 2 bits for top 4 lanes and 2 bits for bottom 4 lanes.<br>
						=0 Idle state. No change in state.<br>
						=1 CGS Passed. Still in K characters mode.<br>
						=2 Link is up.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxLinkStatus(uint8_t afeId, uint16_t *linkStatus)
{
	uint8_t errorStatus = 0;
	uint16_t funcRet = 0;
	if (systemParams[afeId].jesdProtocol == 0)
	{
		AFE_FUNC_EXEC(getJesdRxLinkStatus204B(afeId, &funcRet));
	}
	else
	{
		AFE_FUNC_EXEC(getJesdRxLinkStatus204C(afeId, &funcRet));
	}
	*linkStatus = funcRet;

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read Link Status for for DAC JESD204B.
    @details Reads link status for all the enabled lanes and returns it.
    @param afeId AFE ID
	@param linkStatus Pointer return of the status.<br>
				Return Value is 4 bits. 2 bits for top 4 lanes and 2 bits for bottom 4 lanes.<br>
						=0 Idle state. No change in state.<br>
						=1 In JESD204B: CGS Passed. Still in K characters mode. In JESD204C:Header Aligned but EoEMB lock yet to happen. <br>
						=2 Link is up.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxLinkStatus204B(uint8_t afeId, uint16_t *linkStatus)
{
	/* 
		Reads the lane enables and accordingly returns the status of the link 
		Return Value is 4 bits. 2 bits for top 4 lanes and 2 bits for bottom 4 lanes.
						=0 Idle state. No change in state.
						=1 CGS Passed. Still in K characters mode.
						=2 Link is up.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(linkStatus != NULL);
	uint8_t laneEna0, laneEna1;
	uint8_t csState0, csState1;
	uint8_t expectedCsState0, expectedCsState1;
	uint8_t expectedFsState0, expectedFsState1;
	uint8_t fsState0, fsState1;
	uint8_t linkStatus0, linkStatus1;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x64, 0x0, 0x7, &laneEna0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa2, 0x0, 0x7, &csState0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa4, 0x0, 0x7, &fsState0));

	expectedCsState0 = 0;
	expectedFsState0 = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		if (((laneEna0 >> i) & 1) != 0)
		{
			expectedCsState0 = expectedCsState0 + (2 << (i << 1));
			expectedFsState0 = expectedFsState0 + (1 << (i << 1));
		}
	}

	if (expectedCsState0 == csState0)
	{
		if (expectedFsState0 == fsState0)
		{
			linkStatus0 = 2;
			afeLogInfo("%s", "DAC JESD RX AB. Link is Up.");
		}
		else
		{
			linkStatus0 = 1;
			afeLogErr("%s", "DAC JESD RX AB. Link Not Up. Passed CS State but failed FS State. Probably still receiving K Characters");
		}
	}
	else
	{
		linkStatus0 = 0;
		afeLogErr("%s", "DAC JESD RX AB. Link Not Up. Didn't pass CS State.");
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x64, 0x0, 0x7, &laneEna1));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa2, 0x0, 0x7, &csState1));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa4, 0x0, 0x7, &fsState1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	expectedCsState1 = 0;
	expectedFsState1 = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		if (((laneEna1 >> i) & 1) != 0)
		{
			expectedCsState1 = expectedCsState1 + (2 << (i << 1));
			expectedFsState1 = expectedFsState1 + (1 << (i << 1));
		}
	}

	if (expectedCsState1 == csState1)
	{
		if (expectedFsState1 == fsState1)
		{
			linkStatus1 = 2;
			afeLogInfo("%s", "DAC JESD RX CD. Link is Up.");
		}
		else
		{
			linkStatus1 = 1;
			afeLogErr("%s", "DAC JESD RX CD. Link Not Up. Passed CS State but failed FS State. Probably still receiving K Characters");
		}
	}
	else
	{
		linkStatus1 = 0;
		afeLogErr("%s", "DAC JESD RX CD. Link Not Up. Didn't pass CS State.");
	}

	*linkStatus = ((linkStatus1 << 2) + linkStatus0);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read Link Status for for DAC JESD204B.
    @details Reads link status for all the enabled lanes and returns it.
    @param afeId AFE ID
	@param linkStatus Pointer return of the status.<br>
				Return Value is 4 bits. 2 bits for top 4 lanes and 2 bits for bottom 4 lanes.<br>
						=0 Idle state. No change in state.<br>
						=1 Header Aligned but EoEMB lock yet to happen.
						=2 Link is up.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdRxLinkStatus204C(uint8_t afeId, uint16_t *linkStatus)
{
	/*
		Reads the lane enables and accordingly returns the status of the link 
		Return Value is 4 bits. 2 bits for top 4 lanes and 2 bits for bottom 4 lanes.
						=0 Idle state. No change in state.
						=1 Header Aligned but EoEMB lock yet to happen.
						=2 Link is up.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(linkStatus != NULL);
	uint8_t laneEna0, laneEna1;
	uint8_t csState0, csState1;
	uint8_t expectedCsState0, expectedCsState1;
	uint8_t expectedBufState0, expectedBufState1;
	uint8_t bufState0, bufState1;
	uint8_t linkStatus0, linkStatus1;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x64, 0x0, 0x7, &laneEna0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa2, 0x0, 0x7, &csState0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa6, 0x0, 0x7, &bufState0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	expectedCsState0 = 0;
	expectedBufState0 = 0;

	for (uint8_t i = 0; i < 4; i++)
	{
		if (((laneEna0 >> i) & 1) != 0)
		{
			expectedCsState0 = expectedCsState0 + (2 << (i << 1));
			expectedBufState0 = expectedBufState0 + (3 << (i << 1));
		}
	}

	if (expectedCsState0 == csState0)
	{
		if (expectedBufState0 == bufState0)
		{
			linkStatus0 = 2;
			afeLogInfo("%s", "DAC JESD RX AB. Link is Up.");
		}
		else
		{
			linkStatus0 = 1;
			afeLogErr("%s", "DAC JESD RX AB. Link Not Up. Passed CS State but failed EMB alignment.");
		}
	}
	else
	{
		linkStatus0 = 0;
		afeLogErr("%s", "DAC JESD RX AB. Link Not Up. Didn't pass CS State.");
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x64, 0x0, 0x7, &laneEna1));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa2, 0x0, 0x7, &csState1));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0xa6, 0x0, 0x7, &bufState1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	expectedCsState1 = 0;
	expectedBufState1 = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		if (((laneEna1 >> i) & 1) != 0)
		{
			expectedCsState1 = expectedCsState1 + (2 << (i << 1));
			expectedBufState1 = expectedBufState1 + (3 << (i << 1));
		}
	}

	if (expectedCsState1 == csState1)
	{
		if (expectedBufState1 == bufState1)
		{
			linkStatus1 = 2;
			afeLogInfo("%s", "DAC JESD RX CD. Link is Up.");
		}
		else
		{
			linkStatus1 = 1;
			afeLogErr("%s", "DAC JESD RX CD. Link Not Up. Passed CS State but failed EMB alignment.");
		}
	}
	else
	{
		linkStatus1 = 0;
		afeLogErr("%s", "DAC JESD RX CD. Link Not Up. Didn't pass CS State.");
	}

	*linkStatus = ((linkStatus1 << 2) + linkStatus0);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears DAC JESD JESD204 alarms going to the pin.
    @details Clears DAC JESD JESD204 alarms going to the pin of all lanes.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t clearJesdRxAlarms(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x0C, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x01, 0x0, 0x0)); /*clear_all_alarms=0x1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x00, 0x0, 0x0)); /*clear_all_alarms=0x0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears DAC JESD JESD204 alarms going to the PAP block.
    @details Clears DAC JESD JESD204 alarms going to the PAP block of all lanes.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t clearJesdRxAlarmsForPap(uint8_t afeId)
{
	/* Clearing JESD RX alarms*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x0C, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x04, 0x2, 0x2)); /*clear_all_alarms_to_pap=0x1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x00, 0x2, 0x2)); /*clear_all_alarms_to_pap=0x0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears the Sync Error counter for DAC JESD
    @details Clears the Sync Error counter for DAC JESD.
    @param afeId AFE ID
    @param jesdNo 0 for AB and 1 for CD.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdRxClearSyncErrorCnt(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x007d, 0xff, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x007e, 0xff, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x007d, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x007e, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the Sync Error counter for DAC JESD
    @details Reads the Sync Error counter for DAC JESD and returns it as a pointer.
    @param afeId AFE ID
    @param jesdNo 0 for AB and 1 for CD.
    @param linkErrorCount Pointer returns the counter value of the sync error. This denotes the number of times the resync request was given since the last time it is cleared.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdRxGetSyncErrorCnt(uint8_t afeId, uint8_t jesdNo, uint8_t *linkErrorCount)
{
	/*
		Prints the number of Sync Errors for DAC JESD. jesdNo=0 for AB and jesdNo=1 for CD.
	*/
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(linkErrorCount != NULL);
	uint8_t errorStatus = 0;
	uint8_t ulRegValue = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0084, 0x0, 0x7, &ulRegValue));
	afeLogErr("DAC JESD RX %d saw %d errors.", jesdNo, ulRegValue);
	*linkErrorCount = ulRegValue;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

//// ADC JESD

/**
    @brief Clears ADC JESD JESD204 alarms.
    @details Clears ADC JESD JESD204 alarms of all lanes.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t clearJesdTxAlarms(uint8_t afeId)
{
	/* Clearing JESD TX alarms*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x03, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00f0, 0x0f, 0x0, 0x7)); /*alarms_serdes_fifo_errors_clear=0xf*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00f0, 0x00, 0x0, 0x7)); /*alarms_serdes_fifo_errors_clear=0x0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the Sync Error counter for ADC JESD
    @details Reads the Sync Error counter for ADC JESD and returns it as a pointer.
    @param afeId AFE ID
    @param jesdLaneNo 0-7 is the lane number pre-lane mux (towards the AFE).
    @param linkErrorCount Pointer returns the counter value of the sync error. This denotes the number of times the resync request was given since the last time the JESD was reset. There is no clear counter for this.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdTxGetSyncErrorCnt(uint8_t afeId, uint8_t jesdLaneNo, uint8_t *linkErrorCount)
{
	/*
		Prints the number of Sync Errors for ADC JESD.
		jesdLaneNo 0-7 is the lane number pre-lane mux.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	uint8_t ulRegValue = 0;
	if (jesdLaneNo < 4)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x01, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x02, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0104 + (jesdLaneNo & 0x3), 0x0, 0x7, &ulRegValue));
	afeLogErr("ADC JESD TX lane %d saw %d errors.", jesdLaneNo, ulRegValue);
	*linkErrorCount = ulRegValue;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Send Ramp Test Pattern from ADC JESD.
    @details Send Ramp Test Pattern from ADC JESD. This test pattern is near the ADC-JESD interface.
    @param afeId AFE ID
    @param topno Select the JESD instance. 0-AB. 1-CD.
	@param chNo 0 for RXA/C; 1 for RX B/D; 2 for FB AB/CD
	@param enable 1 to enable the Ramp pattern. 0 to disable.
	@param rampIncr rampIncr+1 is the increment of the steps. 
	@return Returns if the function execution passed or failed.
*/
uint8_t adcRampTestPattern(uint8_t afeId, uint8_t topno, uint8_t chNo, uint8_t enable, uint8_t rampIncr)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topno < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(chNo < 3);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, ((1 << (topno))) & 0xff, 0x0, 0x7)); /*adc_jesd*/
	if (chNo == 0)
	{
		if (enable == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, (((rampIncr) << 3) + 0x2) & 0xff, 0x0, 0x7)); /*rx1_jesd_test_sig_gen_mode, rx1_jesd_ramptest_incr*/
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x2)); /*rx1_jesd_test_sig_gen_mode*/
		}
	}
	else if (chNo == 1)
	{
		if (enable == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, (((rampIncr) << 3) + 0x2) & 0xff, 0x0, 0x7)); /*rx2_jesd_test_sig_gen_mode, rx2_jesd_ramptest_incr*/
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x2)); /*rx2_jesd_test_sig_gen_mode*/
		}
	}
	else if (chNo == 2)
	{
		if (enable == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10b, (((rampIncr) << 3) + 0x2) & 0xff, 0x0, 0x7)); /*fb_jesd_test_sig_gen_mode, fb_jesd_ramptest_incr*/
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10b, 0x0, 0x0, 0x2)); /*fb_jesd_test_sig_gen_mode*/
		}
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Toggles the ADC JESD204B Sync Override
    @details Override the SyncIN override, forces K characters for 100ms and then sends the data. This is to be used only in software sync mode in JESD 204B.
    @param afeId AFE ID
    @param overrideValue Overrides the Sync Pin to this value during K characters mode. Bits 0-5 refer to syncin numbers 0-5. This can be made 0x3f to send K characters on all the links. To toggle sync of only a particular link, need to set only that bit to 1. For example, to toggle only link 2 using SyncIn2, need to set this value to 0x04.
	@return Returns if the function execution passed or failed.
*/
uint8_t toggleSync(uint8_t afeId, uint8_t overrideValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x10, 0x0, 0x7)); /*jesd_subchip*/
	/*Send data*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x59, overrideValue & 0x3f, 0x0, 0x7)); /*adc_jesd_sync_n0_spi_ovr*/ /*adc_jesd_sync_n1_spi_ovr*/ /*adc_jesd_sync_n2_spi_ovr*/ /*adc_jesd_sync_n3_spi_ovr*/ /*adc_jesd_sync_n4_spi_ovr*/ /*adc_jesd_sync_n5_spi_ovr*/

	/*Send K characters*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x58, 0x00, 0x0, 0x7)); /*adc_jesd_sync_n0_spi_val*/ /*adc_jesd_sync_n1_spi_val*/ /*adc_jesd_sync_n2_spi_val*/ /*adc_jesd_sync_n3_spi_val*/ /*adc_jesd_sync_n4_spi_val*/ /*adc_jesd_sync_n5_spi_val*/
	AFE_FUNC_EXEC(waitMs(100));

	/*Send data*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x58, 0x3f, 0x0, 0x7)); /*adc_jesd_sync_n0_spi_val*/ /*adc_jesd_sync_n1_spi_val*/ /*adc_jesd_sync_n2_spi_val*/ /*adc_jesd_sync_n3_spi_val*/ /*adc_jesd_sync_n4_spi_val*/ /*adc_jesd_sync_n5_spi_val*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Overrides the SyncIn of the ADC JESD204B
    @details Overrides the SyncIn of the ADC JESD204B.
    @param afeId AFE ID
    @param syncNo syncNo the sync value. 0-5
	@param overrideValue: 0- do not override. 1- override the SyncIn pin
	@param syncValue: 0- Send K characters. 1- Send Data
	@return Returns if the function execution passed or failed.
*/
uint8_t setJesdTxSyncOverride(uint8_t afeId, uint8_t syncNo, uint8_t overrideValue, uint8_t syncValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x10, 0x0, 0x7));																																													/*jesd_subchip*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x59, (overrideValue & 1) << syncNo, syncNo, syncNo)); /*adc_jesd_sync_n0_spi_ovr*/ /*adc_jesd_sync_n1_spi_ovr*/ /*adc_jesd_sync_n2_spi_ovr*/ /*adc_jesd_sync_n3_spi_ovr*/ /*adc_jesd_sync_n4_spi_ovr*/ /*adc_jesd_sync_n5_spi_ovr*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x58, (syncValue & 1) << syncNo, syncNo, syncNo)); /*adc_jesd_sync_n0_spi_val*/ /*adc_jesd_sync_n1_spi_val*/ /*adc_jesd_sync_n2_spi_val*/ /*adc_jesd_sync_n3_spi_val*/ /*adc_jesd_sync_n4_spi_val*/		/*adc_jesd_sync_n5_spi_val*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Overrides the SyncOut of the DAC JESD204B
    @details Overrides the SyncOut of the DAC JESD204B.
    @param afeId AFE ID
    @param syncNo syncNo the sync value. 0-3
	@param overrideValue: 0- do not override. 1- override the SyncIn pin
	@param syncValue: Pin state
	@return Returns if the function execution passed or failed.
*/
uint8_t setJesdRxSyncOverride(uint8_t afeId, uint8_t syncNo, uint8_t overrideValue, uint8_t syncValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x10, 0x0, 0x7)); /*jesd_subchip*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xC8, ((syncValue << 1) + overrideValue) << (syncNo * 2), syncNo * 2, (syncNo * 2) + 1)); /*dac_jesd_sync_n * _spi_val*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reads the ADC JESD Lane FIFO Errors.
    @details Reads the ADC JESD Lane FIFO Errors, logs their meaning and returns the alarm status as pointer
    @param afeId AFE ID
	@param jesdNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@param errors Pointer return of the status. Value of zero means there is no error and any non-zero value refers to an error.
	@return Returns if the function execution passed or failed.
*/
uint8_t getJesdTxFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t *errors)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(errors != NULL);
	uint8_t laneNo;
	uint8_t ulRegValue = 0;
	*errors = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x01 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00f4, 0x0, 0x7, &ulRegValue));

	for (laneNo = 0; laneNo < 4; laneNo++)
	{
		if (((ulRegValue >> laneNo) & 1) == 1)
		{
			afeLogErr("JESD TX Lane Serdes FIFO Error for lane %d. SerDes PLL isn't locked or the FIFO offset setting is incorrect.", laneNo);
			*errors |= (0x1 << laneNo);
		}
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets the DAC JESD Block
    @details Resets the DAC JESD Block. Sysref should be given to the complete AFE after doing this. It is recommended to always keep jesdNo as 3.
    @param afeId AFE ID
	@param jesdNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdRxFullResetToggle(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, (jesdNo << 2), 0, 7)); //dac_jesd=0x3; 	Address(0x16[7:2])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0020, 0x03, 0, 7));		   //link0-1_init_state=0x1(Meaning:   ));; 	Address(0x20[7:1])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0020, 0x00, 0, 7));		   //link0-1_init_state=0x0(Meaning:   ));; 	Address(0x20[7:1])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0, 7));		   //dac_jesd=0x0; 	Address(0x16[7:2])
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets the ADC JESD Block
    @details Resets the ADC JESD Block. Sysref should be given to the complete AFE after doing this. It is recommended to always keep jesdNo as 3.
    @param afeId AFE ID
	@param jesdNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdTxFullResetToggle(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, jesdNo, 0, 7)); //adc_jesd=0x3; 	Address(0x16[7:2])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x006d, 0x07, 0, 7));	//link0-2_init_state=0x1(Meaning:   ));; 	Address(0x6d[7:0])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x006d, 0x00, 0, 7));	//link0-1_init_state=0x0(Meaning:   ));; 	Address(0x6d[7:0])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0, 7));	//adc_jesd=0x0; 	Address(0x16[7:2])
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears the DAC JESD Data path
    @details Clears the DAC JESD Data Path by sending 0s to the AFE DAC JESD IP Layer and again sends data for the SerDes. Note that relink is needed after this.
    @param afeId AFE ID
	@param jesdNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdRxClearDataPath(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	if ((jesdNo & 1) != 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0, 7)); //dac_jesd=0x3; 	Address(0x16[7:2])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0064, 0xF0, 4, 7)); //jesd_clear_data=0xf; 	Address(0x64[7:4])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0064, 0x00, 4, 7)); //jesd_clear_data=0x0; 	Address(0x64[7:4])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0, 7)); //dac_jesd=0x0; 	Address(0x16[7:2])
	}
	if ((jesdNo & 2) != 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0, 7)); //dac_jesd=0x3; 	Address(0x16[7:2])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0064, 0xF0, 4, 7)); //jesd_clear_data=0xf; 	Address(0x64[7:4])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0064, 0x00, 4, 7)); //jesd_clear_data=0x0; 	Address(0x64[7:4])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0, 7)); //dac_jesd=0x0; 	Address(0x16[7:2])
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears the ADC JESD Data path
    @details Clears the ADC JESD Data Path by sending 0s on the SerDes and again sends data. Note that for some time (approximately time of one SPI write + time between consecutive SPI writes), the AFE SerDes TX transmits 0s and it may be needed for ASIC/FPGA SerDes RX to freeze during this time.
    @param afeId AFE ID
	@param jesdNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdTxClearDataPath(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, jesdNo, 0, 7)); //adc_jesd=0x3; 	Address(0x16[7:2])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0024, 0x01, 0, 7));	//jesd_clear_data=0xf; 	Address(0x64[7:4])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0024, 0x00, 0, 7));	//jesd_clear_data=0x0; 	Address(0x64[7:4])
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0, 7));	//adc_jesd=0x0; 	Address(0x16[7:2])
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets and relinks the AFE JESD
    @details This resets all the JESD blocks, gives sysref to the AFE and checks for the DAC link status.<br>
		Note the following:<br>
		1. Contents of the giveSingleSysrefPulse function should be replaced by host function to give Pin Sysref to AFE. This is used only in case of a single shot sysref.<br>
		2. For Continuous Syref mode, external Pin Sysref should be enabled before this function is called. Note that even in this case, only one pulse edge will be captured by the AFE. In this mode, giveSingleSysrefPulse needn't do any operation. <br>
		3. systemParams[afeId].spiInUseForPllAccess should be set before the function call to the appropriate value for selecting SPIA/SPIB. In Normal use-case SPIA is used and hence can be left at the default.<br>
		4. The selection between the single shot and continuous sysref mode should be done in Latte during generation of the configuration log.<br>
		5. systemParams[afeId].syncLoopBack and systemParams[afeId].jesdProtocol should be appropriately set according to what is there in the initialization.<br>
		6. This doesn't perform any SerDes operations<br>
		7. Any ASIC operations needed for the relink should be done as needed.
    @param afeId AFE ID
	@param pinSysref Chooses between pinSysref and internal copy of Sysref<br>
		0-Uses the internal copy of the Sysref to relink the JESD.<br>
		1-Uses Pin sysref for relink.<br>
		Note that when the pin sysref is made 0, the internal copy of the Sysref is used which is not same as the SPI override of the pin sysref. The internal copy of the sysref will be synchronous to the previous copy of the sysref the AFE received. And since the Sysref frequency is calculated accounts for it, deterministic latency will be achieved even in this mode, assuming the phase of the external sysref is not disturbed.
	@return Returns if the function execution passed or failed or if the relink is not successful.
*/
uint8_t adcDacSync(uint8_t afeId, uint8_t pinSysref)
{
	uint8_t errorStatus = 0;
	uint16_t linkStatus = 0;
	uint8_t linkError = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(jesdRxFullResetToggle(afeId, 3));
	AFE_FUNC_EXEC(jesdTxFullResetToggle(afeId, 3));
	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, systemParams[afeId].spiInUseForPllAccess));
	if (pinSysref == 0)
	{
		//START: Leaking Sysref To Dig
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x01, 0, 7)); //pll=0x1; 	Address(0x15[7:0])
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x08, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0082, 0x0c, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x0b, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0076, 0xff, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x0b, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x0f, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x07, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0082, 0x00, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0077, 0x04, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0076, 0x00, 0, 7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0, 7)); //pll=0x0; 	Address(0x15[7:0])
	}
	else
	{
		AFE_FUNC_EXEC(sendSysref(afeId, 0, 0));
	}

	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, 0));
	AFE_FUNC_EXEC(jesdRxClearDataPath(afeId, 3));
	AFE_FUNC_EXEC(jesdTxClearDataPath(afeId, 3));
	if (systemParams[afeId].syncLoopBack == 0 && systemParams[afeId].jesdProtocol == 0)
	{
		AFE_FUNC_EXEC(toggleSync(afeId, 0x3f));
	}

	AFE_FUNC_EXEC(clearAllAlarms(afeId));
	AFE_FUNC_EXEC(getJesdRxLinkStatus(afeId, &linkStatus));
	if (linkStatus != 10)
	{
		afeLogErr("%s", "AFE DAC-JESD-RX relink not Successful.");
		AFE_FUNC_EXEC(getJesdRxAlarms(afeId, &linkError));
		errorStatus |= 1;
	}
	else
	{
		afeLogInfo("%s", "AFE DAC-JESD-RX relink Successful.");
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Resets the DAC JESD State Machine
    @details Resets the DAC JESD State Machine. In this case no Sysref is needed to be given to the AFE. The LMFC boundary will remain aligned to the previous sysref AFE received.
    @param afeId AFE ID
	@param linkNo 
		1 for JESD AB Instance.<br>
		2 for JESD CD Instance.<br>
		3 for JESD AB & CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t jesdRxResetStateMachine(uint8_t afeId, uint8_t linkNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(linkNo < 4);
	if (linkNo == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	}
	else if (linkNo == 2)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	}
	else if (linkNo == 3)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x0C, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0040, 0x0F, 0x0, 0x7)); /*link0_comma_align_reset/link1_comma_align_reset*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0040, 0x0C, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks if the set RBD value is okay or not.
    @details Checks if the set RBD value is okay or not.
    @param afeId AFE ID
	@param jesdNo 
		0 for JESD AB Instance.<br>
		1 for JESD CD Instance.<br>
	@param rbdStatus Pointer return. Value will be 1 if the RBD set is good, else returns 0.
	@return Returns if the function execution passed or failed.
*/
uint8_t checkIfRbdIsGood(uint8_t afeId, uint8_t jesdNo, uint8_t *rbdStatus)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(rbdStatus != NULL);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (jesdNo))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
	uint8_t a, b;
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x98, 0x0, 0x3, &a)); /*_link0_sysref_cnt_on_release_opportunity*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x98, 0x4, 0x7, &b)); /*_link1_sysref_cnt_on_release_opportunity*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	if (a == 0 && b == 0)
	{
		afeLogInfo("%s", "RBD is okay");
		*rbdStatus = 1;
	}
	else
	{
		afeLogErr("%s", "RBD is not good. Please adjust.");
		*rbdStatus = 0;
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Returns the all lane ready counter.
    @details This function reads the all lane ready counter which is the offset between the internal LMFC boundary and the multiframe boundary (in JESD204B) or extended multi block boundary (in JESD204C) of the last lane of arrival. This value after an offset (of say, 2) with modulus of 64 should be writen to the RBD register.
    @param afeId AFE ID
	@param jesdNo 
		0 for JESD AB Instance.<br>
		1 for JESD CD Instance.<br>
	@param rbdOffset Pointer return. Value of the last all lane ready counter.
	@return Returns if the function execution passed or failed.
*/
uint8_t getAllLaneReady(uint8_t afeId, uint8_t jesdNo, uint8_t *rbdOffset)
{
	uint8_t errorStatus = 0;
	uint8_t value_msb, value_lsb;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_PARAMS_VALID(rbdOffset != NULL);

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (jesdNo))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x88, 0x0, 0x7, &value_lsb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x89, 0x0, 0x7, &value_msb));
	*rbdOffset = (value_msb << 8) + value_lsb; /*_lane0_f_counter_any_lane_ready*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set Good RBD of DAC JESD
    @details This function does the following:<br>
		1. Reads the internal counter between the internal LMFC counter to the received LMFC boundary (multi frame boundary in 204B and Extended Multi Block Boundary in 204C).<br>
		2. Sets the RBD by giving an offset of 4 to the LMFC Counter.<br>
		3. Relinks by calling the adcDacSync function with pinSysref=1. So all the related conditions of adcDacSync are to be satisfied here too.<br>
		Note that this sequence may not ensure deterministic latency across bring-ups and devices. For acheiving it the above 3 steps should be executed using the functions getAllLaneReady, setManualRbd, adcDacSync with external pin sysref. The getAllLaneReady should be done only once and the same value should be loaded each time during the intialization. This can be input as a parameter to Latte.

    @param afeId AFE ID
	@param jesdNo 
		0 for JESD AB Instance.<br>
		1 for JESD CD Instance.
	@return Returns if the function execution passed or failed.
*/
uint8_t setGoodRbd(uint8_t afeId, uint8_t jesdNo)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	uint16_t value = 0;
	uint8_t rbdStatus = 0;
	uint8_t value_msb, value_lsb;
	AFE_FUNC_EXEC(checkIfRbdIsGood(afeId, jesdNo, &rbdStatus));
	if (rbdStatus)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (jesdNo))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x88, 0x0, 0x7, &value_lsb));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x89, 0x0, 0x7, &value_msb));
		value = (value_msb << 8) + value_lsb; /*_lane0_f_counter_any_lane_ready*/
		value = (value + 4) % 64;

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x69, (value >> 8) & 0xff, 0x0, 0x7)); /*link0_rbd_m1*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x68, (value)&0xff, 0x0, 0x7));		   /*link0_rbd_m1*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6b, (value >> 8) & 0xff, 0x0, 0x7)); /*link1_rbd_m1*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6a, (value)&0xff, 0x0, 0x7));		   /*link1_rbd_m1*/
		afeLogInfo("Setting RBD to: %d", value);
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(adcDacSync(afeId, 0));
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD Lane Errors to Pin
    @details Mask DAC JESD Lane Errors to Pin.
    @param afeId AFE ID
	@param laneNo The laneNo is the post-laneMux lane number 0-7 (towards the AFE).
	@param maskValue The bits made 1 will be masked and not reflect on pin.<br>
		Bit No 7 = "multiframe alignment error";<br>
		Bit No 6 = "frame alignment error";<br>
		Bit No 5 = "link configuration error";<br>
		Bit No 4 = "elastic buffer overflow (bad RBD value)";<br>
		Bit No 3 = "elastic buffer match error. The first no-/K/ does not match 'match_ctrl' and 'match_data' programmed values";<br>
		Bit No 2 = "code synchronization error";<br>
		Bit No 1 = "JESD 204B: 8b/10b not-in-table code error. JESD 204C: sync_header_invalid_err";<br>
		Bit No 0 = "JESD 204B: 8b/10b disparty error. JESD 204C: sync_header_parity_err";<br>
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxLaneErrors(uint8_t afeId, uint8_t laneNo, uint8_t maskValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	if ((laneNo & 0x4) == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xfc + (laneNo & 0x3), maskValue, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD FIFO Errors to Pin
    @details Mask DAC JESD FIFO Errors to Pin
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param losMaskValue The bits made 1 will be masked and not reflect on pin. <br>
		Bits0-3 for SerDes Rx Lane Loss of lock error for lanes 0-3 when jesdNo=0 and lanes 4-7 when jesdNo=1.<br>
		These lane numbers are post lane mux, towards the AFE.
	@param fifoMaskValue The bits made 1 will be masked and not reflect on pin.<br>
		Bits0-3 for SerDes Rx Lane FIFO Error for for lanes 0-3 when jesdNo=0 and lanes 4-7 when jesdNo=1.<br>
		These lane numbers are post lane mux, towards the AFE.
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxLaneFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t losMaskValue, uint8_t fifoMaskValue)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00f9, losMaskValue + (fifoMaskValue << 4), 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD Miscellaneous Errors to Pin
    @details Mask DAC JESD Miscellaneous Errors to Pin
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param maskSerdesPllLock The bits made 1 will be masked and not reflect on pin. <br>
		Bit 0 for SRX1-4 and Bit 1 for SRX 5-8. These are Actual SerDes Lane numbers.<br>
		These lane numbers are post lane mux, towards the AFE.
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxMiscSerdesErrors(uint8_t afeId, uint8_t jesdNo, uint8_t maskSerdesPllLock)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00f8, maskSerdesPllLock << 6, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask ADC JESD FIFO Errors to Pin
    @details Mask ADC JESD FIFO Errors to Pin
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param maskValue The bits made 1 will be masked and not reflect on pin. <br>		
		bit 0 is for lane 0 errors, <br>
		bit 1 for lane 1 errors<br>
		bit 2 for lane 2 errors<br>
		bit 3 for lane 3 errors<br>
		The lane number is the lane number of this instance pre-lane mux, towards the AFE.
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdTxFifoErrors(uint8_t afeId, uint8_t jesdNo, uint8_t maskValue)
{
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, ((1 << (jesdNo))) & 0xff, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00f1, maskValue, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD Lane Errors to PAP
    @details Mask DAC JESD Lane Errors to PAP.
    @param afeId AFE ID
	@param laneNo The laneNo is the post-laneMux lane number 0-7 (towards the AFE).
	@param maskValue The bits made 1 will be masked and not reflect on PAP.<br>
		Bit No 7 = "multiframe alignment error";<br>
		Bit No 6 = "frame alignment error";<br>
		Bit No 5 = "link configuration error";<br>
		Bit No 4 = "elastic buffer overflow (bad RBD value)";<br>
		Bit No 3 = "elastic buffer match error. The first no-/K/ does not match 'match_ctrl' and 'match_data' programmed values";<br>
		Bit No 2 = "code synchronization error";<br>
		Bit No 1 = "JESD 204B: 8b/10b not-in-table code error. JESD 204C: sync_header_invalid_err";<br>
		Bit No 0 = "JESD 204B: 8b/10b disparty error. JESD 204C: sync_header_parity_err";<br>
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxLaneErrorsToPap(uint8_t afeId, uint8_t laneNo, uint8_t maskValue)
{
	/*
		The laneNo is the post-laneMux lane number 0-7.
		maskValue: The bits made 1 will be masked and will not go to PAP.
		Bit No 7 = "multiframe alignment error";
		Bit No 6 = "frame alignment error";
		Bit No 5 = "link configuration error";
		Bit No 4 = "elastic buffer overflow (bad RBD value)";
		Bit No 3 = "elastic buffer match error. The first no-/K/ does not match 'match_ctrl' and 'match_data' programmed values";
		Bit No 2 = "code synchronization error";
		Bit No 1 = "JESD 204B: 8b/10b not-in-table code error. JESD 204C: sync_header_invalid_err";
		Bit No 0 = "JESD 204B: 8b/10b disparty error. JESD 204C: sync_header_parity_err";
    */

	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(laneNo < AFE_NUM_SERDES_LANES);
	if ((laneNo & 0x4) == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x08, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10c + (laneNo & 0x3), maskValue, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD FIFO Errors to PAP
    @details Mask DAC JESD FIFO Errors to PAP
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param losMaskValue The bits made 1 will be masked and not reflect on PAP. <br>
		Bits0-3 for SerDes Rx Lane Loss of lock error for lanes 0-3 when jesdNo=0 and lanes 4-7 when jesdNo=1.<br>
		These lane numbers are post lane mux, towards the AFE.
	@param fifoMaskValue The bits made 1 will be masked and not reflect on PAP.<br>
		Bits0-3 for SerDes Rx Lane FIFO Error for for lanes 0-3 when jesdNo=0 and lanes 4-7 when jesdNo=1.<br>
		These lane numbers are post lane mux, towards the AFE.
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxLaneFifoErrorsToPap(uint8_t afeId, uint8_t jesdNo, uint8_t losMaskValue, uint8_t fifoMaskValue)
{
	/*
		The jesdNo is the 0 for AB and 1 for CD.	
		losMaskValue: The bits made 1 will be masked and will not go to PAP.
			Bits0-3 for SerDes Rx Lane LOSS of lock error for lanes 0-3 when jesdNo 0 and 4-7 when jesdNo 1.
		fifoMaskValue: The bits made 1 will be masked and not reflect on pin.
			Bits0-3 for SerDes Rx Lane FIFO Error for lanes 0-3 when jesdNo 0 and 4-7 when jesdNo 1.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0109, losMaskValue + (fifoMaskValue << 4), 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Mask DAC JESD Miscellaneous Errors to PAP
    @details Mask DAC JESD Miscellaneous Errors to PAP
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param maskSerdesPllLock The bits made 1 will be masked and not reflect on PAP. <br>
		Bit 0 for SRX1-4 and Bit 1 for SRX 5-8. These are Actual SerDes Lane numbers.<br>
		These lane numbers are post lane mux, towards the AFE.
	@return Returns if the function execution passed or failed.
*/
uint8_t maskJesdRxMiscSerdesErrorsToPap(uint8_t afeId, uint8_t jesdNo, uint8_t maskSerdesPllLock)
{
	/*
		The jesdNo is the 0 for AB and 1 for CD.	
		maskSerdesPllLock: Bit 0 for SRX1-4 and Bit 1 for SRX 5-7
		The bits made 1 will be masked and will not go to PAP.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x04 << jesdNo, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0108, maskSerdesPllLock << 6, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Sets the RBS valuw
    @details Mask DAC JESD Miscellaneous Errors to Pin
    @param afeId AFE ID
	@param jesdNo 0 for AB and 1 for CD.	
	@param value RBD Value.
	@return Returns if the function execution passed or failed.
*/
uint8_t setManualRbd(uint8_t afeId, uint8_t jesdNo, uint8_t value)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(jesdNo < AFE_NUM_JESD_INSTANCES);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, (((1 << (jesdNo))) << 2) & 0xff, 0x0, 0x7)); /*dac_jesd*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x69, (value >> 8) & 0xff, 0x0, 0x7));			   /*link0_rbd_m1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x68, (value)&0xff, 0x0, 0x7));					   /*link0_rbd_m1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6b, (value >> 8) & 0xff, 0x0, 0x7));			   /*link1_rbd_m1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6a, (value)&0xff, 0x0, 0x7));					   /*link1_rbd_m1*/
	afeLogInfo("Setting RBD to: %d", value);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x16, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(adcDacSync(afeId, 0));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
