/** @file calibrations.c
 * 	@brief	This file has Factory calibration related functions.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added this file only in version 2.1.<br>
 * 		2. Added documentation and improved the parameter validity checks.<br>
 * 		3. Modified the RX DSA calibration function to add placeholder function for channel inputs.<br>
 * 		4. Added TX DSA calibration function.<br>
*/

#include <stdint.h>

#include <math.h>

#include "afe79xxLog.h"
#include "afe79xxTypes.h"

#include "afeCommonMacros.h"

#include "afeParameters.h"
#include "baseFunc.h"
#include "basicFunctions.h"
#include "controls.h"
#include "calibrations.h"
#include "hMacro.h"

/**
    @brief Perform ADC DSA Calibration
    @details This function Performs the RX DSA calibration. giveAfeAdcInput function in baseFunc.c file contents should be coded by the user as needed.
	However, in a single band case, if all the channels can be given input at the same time, this function needn't do any operation and all the channels should be given input before calling this function.
    @param afeId AFE ID
    @param rxChainForCalib Bit Wise RX Channel Select.
			Bit0 for RXA<br>
			Bit1 for RXB<br>
			Bit2 for RXC<br>
			Bit3 for RXD
    @param fbChainForCalib Bit Wise FB Channel Select.
			Bit0 for FBAB<br>
			Bit1 for FBCD
    @param useTxForCalib When Set to 1, TX TDD will be kept on so that TX can be used for the calibration. The data should be still sent from the ASIC/FPGA through JESD.
    @param rxDsaBandCalibMode Sets the RX DSA Band Calibration Mode.<br>
		0 -One Band at a time<br>
		1 - both bands together
    @param readPacket Pointer returns Array of the Read packet. This should be stored in the host memory and be loaded post initialization in normal mode of operation.
    @param readPacketSize Pointer returns the size of the array.
	@return Returns if the function execution passed or failed.
*/
uint8_t doRxDsaCalib(uint8_t afeId, uint8_t rxChainForCalib, uint8_t fbChainForCalib, uint8_t useTxForCalib, uint8_t rxDsaBandCalibMode, uint8_t *readPacket, uint16_t *readPacketSize)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(rxChainForCalib <= AFE_NUM_RX_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(fbChainForCalib <= AFE_NUM_FB_CHANNELS_BITWISE);

	uint8_t byteList[16];
	uint8_t numOfOperands = 0;
	uint8_t calibType = 1;
	uint8_t minAnaDsa = 0;
	uint8_t maxAnaDsa = AFE_RX_DSA_MAX_ANA_DSA_DB;
	uint16_t packetSize = 0;
	uint8_t tempVal = 0;

	if (useTxForCalib == 1)
	{
		AFE_FUNC_EXEC(overrideTdd(afeId, 15, 0, 15, 1)); /*Overiding TDD Enables to remove dependency to pins.*/
	}
	else
	{
		AFE_FUNC_EXEC(overrideTdd(afeId, 15, 0, 0, 1)); /*Overiding TDD Enables to remove dependency to pins.*/
	}
	/*
		Starting the DSA Calibration.
	*/
	byteList[0] = 1;
	byteList[1] = calibType;
	numOfOperands = 2;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));
	AFE_FUNC_EXEC(overrideTddPins(afeId, 1, 1, 1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x08, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x709e, 0x7a, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x709f, 0x28, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x00, 0x0, 0x7));
	for (uint8_t chNo = 0; chNo < 4; chNo++)
	{
		if (((rxChainForCalib >> chNo) & 1) == 0)
		{
			continue;
		}
		/*Setting up the hardware for current channel.*/
		byteList[0] = 2;
		byteList[1] = calibType;
		byteList[2] = 0;
		byteList[3] = 0;
		byteList[4] = 1 << chNo;
		byteList[5] = 1;
		numOfOperands = 6;
		AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x01 << (chNo & 2), 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2333, 0x01, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2332, 0x01, 0x0, 0x7));
		AFE_FUNC_EXEC(waitMs(1));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2332, 0x00, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2333, 0x00, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x00, 0x0, 0x7));
		for (uint8_t bandNo = 0; bandNo < 2; bandNo++)
		{
			if ((systemParams[afeId].numBandsRx[chNo] == 1 && rxDsaBandCalibMode == 1 && bandNo == 1) || (systemParams[afeId].numBandsRx[chNo] == 0 && bandNo == 1))
			{
				continue;
			}
			afeLogInfo("Calibrating RX%d, Band%d. Give input on 128 point bin.", chNo, bandNo);

			/*Giving the Channel Select Information for Macro*/
			AFE_FUNC_EXEC(giveAfeAdcInput(afeId, chNo, bandNo));
			byteList[0] = 3;
			byteList[1] = calibType;
			byteList[2] = 0;
			byteList[3] = 0;
			byteList[4] = 1 << chNo;
			if (systemParams[afeId].numBandsRx[chNo] == 1 && rxDsaBandCalibMode == 1)
			{
				byteList[5] = 3;
			}
			else
			{
				byteList[5] = (1 << bandNo);
			}
			byteList[6] = 0;
			byteList[7] = 0;
			byteList[8] = 0x32;
			byteList[9] = 0;
			byteList[10] = 0;
			byteList[11] = 0;
			byteList[12] = 0;
			byteList[13] = 0;
			byteList[14] = 0;
			byteList[15] = 0;
			numOfOperands = 16;
			AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));
		}
	}

	if (fbChainForCalib != 0)
	{
		if (useTxForCalib == 1)
		{
			AFE_FUNC_EXEC(overrideTdd(afeId, 0, fbChainForCalib & 3, 15, 1)); /*Overiding TDD Enables to remove dependency to pins.*/
		}
		else
		{
			AFE_FUNC_EXEC(overrideTdd(afeId, 0, fbChainForCalib & 3, 0, 1)); /*Overiding TDD Enables to remove dependency to pins.*/
		}

		for (uint8_t chNo = 0; chNo < 2; chNo++)
		{
			if (((fbChainForCalib >> chNo) & 1) == 0)
			{
				continue;
			}

			/*Setting up the hardware for current channel.*/
			byteList[0] = 2;
			byteList[1] = calibType;
			byteList[2] = 0;
			byteList[3] = 0;
			byteList[4] = 1 << (chNo + 4);
			byteList[5] = 1;
			numOfOperands = 6;
			AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x01 << chNo, 0x0, 0x7));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2333, 0x01, 0x0, 0x7));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2332, 0x01, 0x0, 0x7));
			AFE_FUNC_EXEC(waitMs(1));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2332, 0x00, 0x0, 0x7));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2333, 0x00, 0x0, 0x7));
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x00, 0x0, 0x7));
			afeLogInfo("Calibrating FB %d. Give input on 128 point bin.", chNo);
			AFE_FUNC_EXEC(giveAfeAdcInput(afeId, chNo + AFE_NUM_RX_CHANNELS, 0));
			/*Giving the Channel Select Information for Macro*/

			byteList[0] = 3;
			byteList[1] = calibType;
			byteList[2] = 0;
			byteList[3] = 0;
			byteList[4] = 1 << (chNo + 4);
			byteList[5] = 1;
			byteList[6] = 0;
			byteList[7] = 0;
			byteList[8] = 0x32;
			byteList[9] = 0;
			byteList[10] = 0;
			byteList[11] = 0;
			byteList[12] = 0;
			byteList[13] = 0;
			byteList[14] = 0;
			byteList[15] = 0;
			numOfOperands = 16;
			AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));
		}
	}

	byteList[0] = 4;
	byteList[1] = calibType;
	byteList[2] = 0;
	byteList[3] = 0;
	byteList[4] = 0;
	byteList[5] = 0xff;
	byteList[6] = 0;
	byteList[7] = minAnaDsa;
	byteList[8] = maxAnaDsa;
	byteList[9] = 0x3;
	byteList[10] = 0x0;
	byteList[11] = 0x0;
	numOfOperands = 12;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_RX_DSA_GAIN_PHASE_CALIBRATION));

	/*Read the packet Size*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x20, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00FC, 0x0, 0x7, &tempVal));
	packetSize = tempVal;
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00FD, 0x0, 0x7, &tempVal));
	packetSize = packetSize + (tempVal << 8);

	/*Read the Packet. This packet should be saved.*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0144, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x01, 0x0, 0x7));
	*readPacketSize = packetSize;
	for (uint16_t i = 0; i < packetSize; i++)
	{
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0020 + i, 0x0, 0x7, &tempVal));
		readPacket[i] = tempVal;
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x00, 0x0, 0x7));

	/*Apply the calibrated packet.*/
	byteList[0] = 1;
	numOfOperands = 1;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_APPLY_DSA_GAIN_PHASE_COMPENSATION));

	/*Toggling the DSA Change Pulses to apply the new packet.*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0013, 0xc0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0320, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0320, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0320, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0370, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0370, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0370, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x02c8, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x02c8, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x02c8, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0013, 0x00, 0x0, 0x7));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x30, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0d90, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2014, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0012, 0x00, 0x0, 0x7));
	/* Release TDD Pin Controls to Pins */
	AFE_FUNC_EXEC(overrideTddPins(afeId, 0, 0, 0));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Perform DAC DSA Calibration
    @details This function Performs the TX DSA calibration. connectAfeTxToFb function in baseFunc.c file contents should be coded by the user as needed.
	However, in a single band case, if all the channels can be given input at the same time, this function needn't do any operation and all the channels should be given input before calling this function.
    @param afeId AFE ID
    @param txChainForCalib Bit Wise TX Channel Select.<br>
			Bit0 for TXA<br>
			Bit1 for TXB<br>
			Bit2 for TXC<br>
			Bit3 for TXD
    @param txDsaCalibMode DSA Calibration Mode.<br>0 -Single Fb Mode FB AB ; 1 -Single Fb Mode FB CD ; 2- Dual Fb_Mode
    @param txDsaBandCalibMode Sets the TX DSA Band Calibration Mode.<br>
		0 -One Band at a time<br>
		1 - both bands together
    @param readPacket Pointer returns Array of the Read packet. This should be stored in the host memory and be loaded post initialization in normal mode of operation.
    @param readPacketSize Pointer returns the size of the array.
	@return Returns if the function execution passed or failed.
*/
uint8_t doTxDsaCalib(uint8_t afeId, uint8_t txChainForCalib, uint8_t txDsaCalibMode, uint8_t txDsaBandCalibMode, uint8_t *readPacket, uint16_t *readPacketSize)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(txChainForCalib <= AFE_NUM_TX_CHANNELS_BITWISE);

	uint8_t byteList[16];
	uint8_t numOfOperands = 0;
	uint8_t minAnaDsa = 0;
	uint8_t maxAnaDsa = AFE_TX_DSA_MAX_ANA_DSA_DB;
	uint16_t packetSize = 0;
	uint8_t tempVal = 0;
	uint8_t currentTxCalib = 0;
	uint8_t currentFbConnection = 0;
	uint8_t currentBandSel = 0;

	AFE_FUNC_EXEC(overrideTdd(afeId, 0, 3, 15, 1)); /*Overiding TDD Enables to remove dependency to pins.*/
	//	Starting the DSA Calibration.
	AFE_FUNC_EXEC(enableMemAccess(afeId, 1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x20, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0144, 0x04, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x08, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x70be, 0xff, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x70bf, 0x00, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x70be, 0x0c, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x70bf, 0x04, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x00, 0, 7));
	AFE_FUNC_EXEC(enableMemAccess(afeId, 0));

	byteList[0] = 1;
	byteList[1] = 0;
	numOfOperands = 2;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_TX_DSA_GAIN_PHASE_CALIBRATION));
	AFE_FUNC_EXEC(overrideTddPins(afeId, 1, 1, 1));
	for (uint8_t chNo = 0; chNo < 4; chNo++)
	{
		if ((((txChainForCalib >> chNo) & 1) == 0) || ((txDsaCalibMode == 2) && (txChainForCalib >= AFE_NUM_FB_CHANNELS)))
		{
			continue;
		}
		if (txDsaCalibMode == 0)
		{
			currentTxCalib = 1 << chNo;
			currentFbConnection = 1;
		}
		else if (txDsaCalibMode == 1)
		{
			currentTxCalib = 1 << chNo;
			currentFbConnection = 2;
		}
		else if (txDsaCalibMode == 2)
		{
			currentTxCalib = 5 << chNo;
			currentFbConnection = 3;
		}
		else
		{
			afeLogErr("%s", "Invalid txDsaCalibMode passed.");
			return RET_EXEC_FAIL;
		}
		for (uint8_t i = 0; i < AFE_NUM_TX_CHANNELS; i++) // To Prevent coupling for other channels.
		{
			if (systemParams[afeId].ncoFreqMode == 0)
			{
				AFE_FUNC_EXEC(txCalibSiggen(afeId, i, 1, ((uint32_t)ceil(((systemParams[afeId].txNco[0][chNo][0] / (double)systemParams[afeId].Fdac) * (0x100000000)))) & 0xffffffff, 100));
			}
			else
			{
				AFE_FUNC_EXEC(txCalibSiggen(afeId, i, 1, (uint32_t)(1000 * systemParams[afeId].txNco[0][chNo][0]), 100));
			}
		}

		for (uint8_t bandNo = 0; bandNo <= systemParams[afeId].numBandsTx[chNo]; bandNo++)
		{
			if ((txDsaBandCalibMode == 0) || (systemParams[afeId].numBandsTx[chNo] == 0))
				currentBandSel = 1 << bandNo;
			else if (systemParams[afeId].numBandsTx[chNo] == 1)
				currentBandSel = 3;
			afeLogInfo("Calibrating TX Enables %d, Band %d.", currentTxCalib, bandNo);
			AFE_FUNC_EXEC(connectAfeTxToFb(afeId, currentTxCalib, currentFbConnection, currentBandSel));

			byteList[0] = 3;
			byteList[1] = 0;
			byteList[2] = currentTxCalib;

			if (txDsaCalibMode == 0)
			{
				byteList[3] = 0;
			}
			else if (txDsaCalibMode == 1)
			{
				byteList[3] = 0xf;
			}
			else if (txDsaCalibMode == 2)
			{
				byteList[3] = 0xC;
			}
			byteList[4] = currentBandSel + (currentBandSel << 2) + (currentBandSel << 4) + (currentBandSel << 6);
			byteList[5] = 0;
			byteList[6] = 1;
			byteList[7] = 0;
			byteList[8] = AFE_TX_DSA_MAX_ANA_DSA_DB;
			byteList[9] = 3;
			byteList[10] = 0;
			byteList[11] = 0;
			numOfOperands = 12;
			AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_TX_DSA_GAIN_PHASE_CALIBRATION));
		}
	}
	// Generate the Calibration Packet
	byteList[0] = 4;
	byteList[1] = 0;
	byteList[2] = 0;
	byteList[3] = 0;
	byteList[4] = 0;
	byteList[5] = 0;
	byteList[6] = 1;
	byteList[7] = minAnaDsa;
	byteList[8] = maxAnaDsa;
	byteList[9] = 0x3;
	byteList[10] = 0x0;
	byteList[11] = 0x0;
	numOfOperands = 12;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_FACTORY_TX_DSA_GAIN_PHASE_CALIBRATION));

	/*Read the packet Size*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x20, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00FC, 0x0, 0x7, &tempVal));
	packetSize = tempVal;
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00FD, 0x0, 0x7, &tempVal));
	packetSize = packetSize + (tempVal << 8);

	/*Read the Packet. This packet should be saved.*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0144, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x01, 0x0, 0x7));
	*readPacketSize = packetSize;
	for (uint16_t i = 0; i < packetSize; i++)
	{
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0020 + i, 0x0, 0x7, &tempVal));
		readPacket[i] = tempVal;
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0018, 0x00, 0x0, 0x7));
	/*Apply the calibrated packet.*/
	byteList[0] = 0;
	numOfOperands = 1;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_APPLY_DSA_GAIN_PHASE_COMPENSATION));

	/* Release TDD Pin Controls to Pins */
	AFE_FUNC_EXEC(overrideTddPins(afeId, 0, 0, 0));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
/**
    @brief Load the TX DSA Calibration Packet
    @details This function loads the TX DSA Calibration Packet
    @param afeId AFE ID
    @param array Pointer of array of the packet which was stored in host after calibration.
    @param arraySize Value of the size of the array.
	@return Returns if the function execution passed or failed.
*/
uint8_t loadTxDsaPacket(uint8_t afeId, uint8_t *array, uint16_t arraySize)
{
	/*
	Pass the packet read back during calibration to apply it for TX DSA.
	*/
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	uint8_t byteList[1];
	uint8_t numOfOperands = 0;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x20, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0144, 0x00, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x01, 0, 7));
	for (uint16_t i = 0; i < arraySize; i++)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x020 + i, array[i], 0, 7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x00, 0, 7));
	byteList[numOfOperands] = (0);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_APPLY_DSA_GAIN_PHASE_COMPENSATION));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Load the ADC DSA Calibration Packet
    @details This function loads the ADC DSA Calibration Packet
    @param afeId AFE ID
    @param array Pointer of array of the packet which was stored in host after calibration.
    @param arraySize Value of the size of the array.
	@return Returns if the function execution passed or failed.
*/
uint8_t loadRxDsaPacket(uint8_t afeId, uint8_t *array, uint16_t arraySize)
{
	/*
	Pass the packet read back during calibration to apply it for TX DSA.
	*/
	uint8_t errorStatus = 0;
	uint8_t byteList[1];
	uint8_t numOfOperands = 0;

	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x20, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0144, 0x00, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x01, 0, 7));
	for (uint16_t i = 0; i < arraySize; i++)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x020 + i, array[i], 0, 7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x018, 0x00, 0, 7));
	byteList[numOfOperands] = (1);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_APPLY_DSA_GAIN_PHASE_COMPENSATION));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
