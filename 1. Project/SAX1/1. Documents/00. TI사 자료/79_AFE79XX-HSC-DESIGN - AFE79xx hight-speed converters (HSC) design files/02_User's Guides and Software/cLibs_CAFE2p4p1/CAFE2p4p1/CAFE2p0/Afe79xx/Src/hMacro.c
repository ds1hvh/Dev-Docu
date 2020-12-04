/** @file hMacro.c
 * 	@brief	This file has Macros related functions.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 * 		2. Deleted redundant function: doPrepareTune<br>
 * 		3. Added TX Tone Gen function<br>
 * 		4. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
*/

#include <stdint.h>

#include "afe79xxLog.h"
#include "afe79xxTypes.h"

#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "basicFunctions.h"

#include "hMacro.h"

/**
    @brief Write the Macro Operands.
    @details Write the Macro Operands.
    @param afeId AFE ID
	@param operandList Byte-wise array of operands to be written.
	@param numOfOperands Size of operandList.
	@return Returns if the function execution passed or failed.
*/
uint8_t writeOperandList(uint8_t afeId, uint8_t *operandList, uint8_t numOfOperands)
{
	AFE_ID_VALIDITY();
	uint8_t errorStatus = 0;
	uint8_t operandNo = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	for (; operandNo < numOfOperands; operandNo++)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_OPERAND_START_REG_ADDR + operandNo, operandList[operandNo] & 0xff, 0x0, 0x7));
		/*MACRO_OPERAND_REG0*/
	}
	if (operandNo % 4 != 0)
	{
		for (uint8_t i = 0; i < 4 - (operandNo % 4); i++)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_OPERAND_START_REG_ADDR + operandNo + i, 0, 0x0, 0x7));
		}
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Converts a value into byte wise array.
    @details Converts a value into byte wise array.
    @param val Value to be converted
	@param numBytes Number of Bytes to convert it to.
	@param splitByteList Pointer return of the resultant array.
	@return Returns if the function execution passed or failed.
*/
uint8_t splitToByte(uint64_t val, uint8_t numBytes, uint8_t *splitByteList)
{
	uint8_t iter = 0;
	/*  Split the given value into Byte format for forming Macro byteList.  */
	for (iter = 0; iter < numBytes; iter++)
	{
		splitByteList[iter] = (((val) & (0xFF << 8 * iter)) >> (8 * iter));
	}
	return RET_OK;
}

/**
    @brief Read Macro Result Register
    @details Read Macro Result Register
    @param afeId AFE ID
	@param regNum Result register number.
	@param result Returns result register as a pointer.
	@return Returns if the function execution passed or failed.
*/
uint8_t readResultRegSpi(uint8_t afeId, uint8_t regNum, uint32_t *result)
{
	uint8_t errorStatus = 0;
	uint8_t readValue;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	for (uint8_t i = 0; i < 4; i++)
	{
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, (AFE_MACRO_RESULT_START_REG_ADDR + i + (regNum * 4)) << (i << 3), 0, 7, &readValue));
		*result = *result + readValue;
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Poll for Macro Ready
    @details Polls for Macro Ready
    @param afeId AFE ID
	@return Returns if the function execution passed or failed. It returns as failed even if the Macro_Ready doesn't become 1.
*/
uint8_t waitForMacroReady(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	uint8_t count = 0;
	uint8_t readValue;
	/*  Wait for Macro Ready.   */
	AFE_ID_VALIDITY();
	count = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	while (((readValue & 1) == 0) && (count <= 200))
	{
		count++;
		AFE_FUNC_EXEC(waitMs(1));
		if (count > 200)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
			/*macro*/
			return RET_EXEC_FAIL;
		}
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
	/*macro*/
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Poll for Macro Done
    @details Polls for Macro Done
    @param afeId AFE ID
	@return Returns if the function execution passed or failed. It returns as failed even if the Macro_Done doesn't become 1.
*/
uint8_t waitForMacroDone(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	uint8_t count = 0;
	uint8_t readValue;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	while (((readValue & 4) == 0) && (count <= 200))
	{
		count++;
		AFE_FUNC_EXEC(waitMs(1));
		if (count > 200)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
			/*macro*/
			return RET_EXEC_FAIL;
		}
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
	/*macro*/

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Poll for Macro Acknowledgement
    @details Polls for Macro Acknowledgement
    @param afeId AFE ID
	@return Returns if the function execution passed or failed. It returns as failed even if the Macro_ACK doesn't become 1.
*/
uint8_t waitForMacroAck(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	uint8_t count = 0;
	uint8_t readValue = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	while (((readValue & 2) == 0) && (count <= 200))
	{
		count++;
		AFE_FUNC_EXEC(waitMs(1));
		if (count > 200)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
			/*macro*/
			return RET_EXEC_FAIL;
		}
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &readValue));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
	/*macro*/

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks if there is a Macro Error
    @details Checks if there is a Macro Error and returns the error status as pointer.
    @param afeId AFE ID
    @param macroErrorStatus Macro Error Status return as pointer.
	@return Returns if the function execution passed or failed.
*/
uint8_t checkForMacroError(uint8_t afeId, uint8_t *macroErrorStatus)
{
	uint8_t errorStatus = 0;
	uint8_t errorReadReg = 0;
	uint8_t errorExtendedCodeReg = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_STATUS_REG_ADDR, 0, 7, &errorReadReg));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, AFE_MACRO_EXTENDED_ERROR_CODE_REG_ADDR, 0, 7, &errorExtendedCodeReg));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x000, 0x0, 0x7));
	*macroErrorStatus = AFE_MACRO_NO_ERROR;
	if ((((errorReadReg) >> 3) & 1) == 1)
	{
		afeLogErr("Macro Error Execution Failed with the error 0X%0X: ", errorExtendedCodeReg);
		if ((((errorReadReg) >> 4) & 0x1) == 1)
		{
			afeLogErr("%s", "MACRO_ERROR_IN_OPCODE");
			*macroErrorStatus |= AFE_MACRO_ERROR_IN_OPCODE;
		}
		if ((((errorReadReg) >> 5) & 0x1) == 1)
		{
			afeLogErr("%s", "MACRO_ERROR_OPCODE_NOT_ALLOWED");
			*macroErrorStatus |= AFE_MACRO_ERROR_OPCODE_NOT_ALLOWED;
		}
		if ((((errorReadReg) >> 6) & 0x1) == 1)
		{
			afeLogErr("%s", "MACRO_ERROR_IN_OPERAND");
			*macroErrorStatus |= AFE_MACRO_ERROR_IN_OPERAND;
		}
		if ((((errorReadReg) >> 7) & 0x1) == 1)
		{
			afeLogErr("%s", "MACRO_ERROR_IN_EXECUTION");
			*macroErrorStatus |= AFE_MACRO_ERROR_IN_EXECUTION;
		}
	}
	/*Returning only the macro Error*/
	*macroErrorStatus = ((errorReadReg >> 3) & 1);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Writes Opcode and triggers the Macro.
    @details Writes Opcode and triggers the Macro.
    @param afeId AFE ID
    @param opcode Opcode of the Macro.
	@return Returns if the function execution passed or failed.
*/
uint8_t triggerMacro(uint8_t afeId, uint8_t opcode)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	/*  Triggers the Macro by writing the Macro Opcode.   */
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL, 0x0, 0x7));
	/*macro*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_OPCODE_REG_ADDR, (opcode)&0xff, 0x0, 0x7));
	/*MACRO_OPCODE*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, AFE_MACRO_PAGE_REG_ADDR, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Execute a Macro
    @details Executes the Macro by calling other sub functions.
    @param afeId AFE ID
	@param byteList Byte-wise array of operands to be written.
	@param numOfOperands Size of operandList.
    @param opcode Opcode of the Macro.
	@return Returns if the function execution passed or failed.
*/
uint8_t executeMacro(uint8_t afeId, uint8_t *byteList, uint8_t numOfOperands, uint8_t opcode)
{
	/*  Execute a Macro.   */
	uint8_t macroErrorStatus = 0;
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_MACRO_READY_POLL_FAIL(waitForMacroReady(afeId));
	AFE_FUNC_EXEC(writeOperandList(afeId, byteList, numOfOperands));
	AFE_FUNC_EXEC(triggerMacro(afeId, opcode));
	AFE_MACRO_DONE_POLL_FAIL(waitForMacroDone(afeId));
	AFE_FUNC_EXEC(checkForMacroError(afeId, &macroErrorStatus));
	AFE_MACRO_EXEC_ERROR(macroErrorStatus);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Enables MCU Memory Access for SPI.
    @details Enables MCU Memory Access for SPI. Note that this should be relinquished after the access is complete.
    @param afeId AFE ID
    @param en 1 enable MCU memory access for SPI.<br>
			0 disable MCU memory access for SPI
	@return Returns if the function execution passed or failed.
*/
uint8_t enableMemAccess(uint8_t afeId, uint8_t en)
{
	uint8_t errorStatus = 0;
	uint8_t byteList[1];
	uint8_t numOfOperands = 0;
	AFE_ID_VALIDITY();

	if (en == 1)
	{
		byteList[numOfOperands] = 0;
		numOfOperands++;
	}
	else
	{
		byteList[numOfOperands] = 1;
		numOfOperands++;
	}
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_SYSTEM_TUNE)); //MacroConsts.MACRO_OPCODE_SYSTEM_TUNE);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reconfigures the selected Chains.
    @details Reconfigures the selected Chains. This function is called in updateTxNco function and is not recommended to be called independently.
	@return Returns if the function execution passed or failed.
*/
uint8_t doSystemTuneSelective(uint8_t afeId, uint8_t rxChList, uint8_t fbChList, uint8_t txChList, uint8_t sectionEnable)
{
	/*  Does the System Configuration for selected Chains depending upon the Config Params set by other Macros. Section Enables can be used for Selective Tune.   */
	uint8_t errorStatus = 0;
	uint8_t byteList[4];
	uint8_t numOfOperands = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(rxChList <= AFE_NUM_RX_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(fbChList <= AFE_NUM_FB_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(txChList <= AFE_NUM_TX_CHANNELS_BITWISE);
	byteList[numOfOperands] = (rxChList + (fbChList << 4));
	numOfOperands++;
	byteList[numOfOperands] = (txChList);
	numOfOperands++;
	byteList[numOfOperands] = ((0x3FFF ^ sectionEnable) & 0xff);
	numOfOperands++;
	byteList[numOfOperands] = ((0x3FFF ^ sectionEnable) >> 8);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_SYSTEM_TUNE_SELECTIVE)); //MacroConsts.MACRO_OPCODE_SYSTEM_TUNE);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Reconfigures the TX NCO info to the MCU.
    @details Reconfigures the TX NCO info to the MCU. This function is called in updateTxNco function and is not recommended to be called independently.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateSystemTxChannelFreqConfig(uint8_t afeId, uint8_t txChList, uint8_t listNCO, uint32_t txNCO, uint8_t immUpdt, uint8_t reload)
{
	/*  Update the Tx Channel Frequency Config Params for selected Chains/Bands/NCOs.   */
	uint8_t errorStatus = 0;
	uint8_t byteList[7];
	uint8_t byteListTxNco[4];
	uint8_t numOfOperands = 0;
	uint8_t updateNcoByte = 0;

	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(txChList <= AFE_NUM_TX_CHANNELS_BITWISE);
	byteList[numOfOperands] = (txChList);
	numOfOperands++;
	byteList[numOfOperands] = (listNCO);
	numOfOperands++;
	AFE_FUNC_EXEC(splitToByte(txNCO, 4, byteListTxNco));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListTxNco[i];
		numOfOperands++;
	}
	updateNcoByte = immUpdt | (reload << 1);
	byteList[numOfOperands] = (updateNcoByte);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_SYSTEM_TX_CHANNEL_FREQUENCY_CONFIGURATION)); //MacroConsts.MACRO_OPCODE_UPDATE_SYSTEM_TX_CHANNEL_FREQUENCY_CONFIGURATION);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks for MCU Health.
    @details Checks for MCU Health and returns the status as a pointer.
    @param afeId AFE ID
    @param healthOk Return Pointer of the status of the MCU. This value is 1 if the MCU is working properly and 0 if MCU is stuck.
	@return Returns if the function execution passed or failed.
*/
uint8_t checkMcuHealth(uint8_t afeId, uint8_t *healthOk)
{

	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(healthOk != NULL);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x10, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00F0, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x00A0, 0x02, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0193, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x00, 0x0, 0x7));
	if (waitForMacroReady(afeId) == RET_EXEC_FAIL)
	{
		*healthOk = 0;
	}
	else
	{
		*healthOk = 1;
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief TX Tone Generator
    @details This function sends a single tone to the TX.
    @param afeId AFE ID
    @param chNo TX Channel Select.
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
    @param configOption Tone Generation Command<br>
			0 → RESERVED<br>
			1 → The current mixer configuration will be saved and the mixers will be configured to give the new tone frequency. <br>
			2 → The mixers will be configured to give the new tone but the saved configuration will not be modified. <br>
			3 → RESERVED<br>
			4 → Restore saved configuration. This can be called to restore the last saved mixer configuration.
    @param freq0 RF tone Frequency<br>
    			Should pass value in KHz in 1KHz ncoFreqMode and the frequency word value in FCW mode. The Mode is determined by the ncoFreqMode set in Latte while generating the bringup script.<br>
				In FCW mode, the value can be calculate using the equation: mixer =  (uint32_t) (2^32*mixerFrequency/Fdac).
	@param freq0Amp Tone Backoff in dB<br>
	@return Returns if the function execution passed or failed.
*/
uint8_t txCalibSiggen(uint8_t afeId, uint8_t chNo, uint8_t configOption, uint32_t freq0, uint8_t freq0Amp)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo <= AFE_NUM_TX_CHANNELS_BITWISE);
	uint8_t byteList[16];
	uint8_t numOfOperands = 0;
	uint8_t byteListFreq[4];
	byteList[numOfOperands] = chNo;
	numOfOperands++;
	byteList[numOfOperands] = 1 << configOption;
	numOfOperands++;
	byteList[numOfOperands] = 0;
	numOfOperands++;
	AFE_FUNC_EXEC(splitToByte(freq0 & 0xffffffff, 4, byteListFreq));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListFreq[i];
		numOfOperands++;
	}
	byteList[numOfOperands] = freq0Amp;
	numOfOperands++;

	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = 0;
		numOfOperands++;
	}
	byteList[numOfOperands] = 0;
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_CONFIG_SIGGEN_FOR_CAL));

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
