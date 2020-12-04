/** @file dsaAndNco.c
 * 	@brief	This file has DSA and NCO related functions. <br>
 * 		<b> Version 2.2:</b> <br>
 * 		1. Updated setTxDigGain and txDsaIdxGainSwap along with the description and parameter validity.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 * 		2. Removed redundant functions related to older device version.<br>
 * 		3. Fixed data types of parameters if function: updateTxGain<br>
 * 		4. Removed redundant writes in functions.<br>
 * 		5. Changed the function input definition of updateTxNco, updateRxNco and updateFbNco in FCW mode from KHz to FCW word. This is done to give finer control of frequency preventing rounding errors which is expected in FCW mode.<br>
 * 		6. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
 * 		7. Fixed bugs in readTxNco.<br>
*/

#include <stdint.h>
#include <math.h>

#include "afe79xxLog.h"
#include "afe79xxTypes.h"

#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeParameters.h"
#include "hMacro.h"

#include "dsaAndNco.h"

/**
    @brief Set the TX Analog DSA
    @details Sets the TX Analog DSA.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
	@param dsaSetting Analog DSA Index. Attenuation applied is dsaSetting*1dB
	@return Returns if the function execution passed or failed.
*/
uint8_t setTxDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting)
{
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(dsaSetting <= AFE_TX_DSA_MAX_ANA_DSA_INDEX);
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x10 << (chNo >> 1), 0x0, 0x7)); /*dsa_page0*/
	if (chNo % 2 == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc8, (dsaSetting)&0xff, 0x0, 0x5)); /*txa_dsa_index*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xc8, (dsaSetting)&0xff, 0x0, 0x5)); /*Writing Twice is needed for consistency.*/
	}

	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcc, (dsaSetting)&0xff, 0x0, 0x5)); /*txb_dsa_index*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xcc, (dsaSetting)&0xff, 0x0, 0x5)); /*Writing Twice is needed for consistency.*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the FB Analog DSA
    @details Sets the FB Analog DSA.
    @param afeId AFE ID
    @param chNo Select the FB Channel<br>
			0 for FBAB<br>
			1 for FBCD
	@param dsaSetting Analog DSA Index. Attenuation applied is dsaSetting*0.5dB
	@return Returns if the function execution passed or failed.
*/
uint8_t setFbDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_FB_CHANNELS);
	AFE_PARAMS_VALID(dsaSetting <= AFE_FB_DSA_MAX_ANA_DSA_INDEX);

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x10 << chNo, 0x0, 0x7));		 /*dsa_page0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6c, dsaSetting & 0xff, 0x0, 0x5)); /*spi_agc_dsa_fb*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the RX Analog DSA
    @details Sets the RX Analog DSA.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param dsaSetting Analog DSA Index. Attenuation applied is dsaSetting*0.5dB
	@return Returns if the function execution passed or failed.
*/
uint8_t setRxDsa(uint8_t afeId, uint8_t chNo, uint8_t dsaSetting)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(dsaSetting <= AFE_RX_DSA_MAX_ANA_DSA_INDEX);

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x40 << (chNo >> 1), 0x0, 0x7)); /*dsa_page1*/
	if (chNo % 2 == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x124, dsaSetting & 0xff, 0x0, 0x5)); /*spi_agc_dsa_A*/
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x174, dsaSetting & 0xff, 0x0, 0x5)); /*spi_agc_dsa_B*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the RX Digital DSA
    @details Sets the RX Digital DSA.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
    @param bandNo Select the RX Band. 0-Band0, 1-Band1<br>
	@param dsaSetting (dsaSetting*0.5-3)dB is the applied DSA gain (if positive) and attenuation (if negative). Range for dsaSetting is 0 to 47.
	@return Returns if the function execution passed or failed.
*/
uint8_t setRxDigGain(uint8_t afeId, uint8_t chNo, uint8_t bandNo, uint8_t dsaSetting)
{
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(dsaSetting <= AFE_RX_DSA_MAX_DIG_DSA_INDEX);
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 1 << chNo, 0x0, 0x7)); /*rxdig*/
	if (bandNo == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x188, dsaSetting & 0xff, 0x0, 0x5)); /*diggain_rx_b0_gain_idx*/
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x288, dsaSetting & 0xff, 0x0, 0x5)); /*diggain_rx_b1_gain_idx*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the RX DSA Mode
    @details Sets the RX DSA Control Mode.
    @param afeId AFE ID
    @param topNo Select the RX Channel<br>
			0 for RXAB<br>
			1 for RXCD
    @param mode DSA Control Mode Setting.<br>
		1-8-Pin Based DSA Control<br>
		2-Internal AGC<br>
		3-SPI AGC<br>
		4-4-Pin Based DSA Control
	@return Returns if the function execution passed or failed.
*/
uint8_t setRxDsaMode(uint8_t afeId, uint8_t topNo, uint8_t mode)
{
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topNo < (AFE_NUM_RX_CHANNELS / 2));
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x40 << topNo, 0x0, 0x7)); /*dsa_page1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd0, (mode)&0xff, 0x0, 0x2));	 /*gain_ctrl*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Configure Settings related to the 4-pin based DSA control mode.
    @details Configure Settings related to the 4-pin based DSA control mode. Effective DSA attenuation is ((pin_value * dsaStep) +dsaInit)*0.5dB.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param dsaInit Offset of the DSA.
	@param dsaStep DSA Step Value. 
	@param maxDelay This is the delay after the change of pin change to latch the values. This is to account for the latency variation between pins.<br>
			This should be the maximum latency variation between the earliest pin and the last pin.
			<br>This is the common control for 2RX. The unit is in cycles of FadcRx/8 clock. Supported values: 0<=maxDelay<=255.
	@return Returns if the function execution passed or failed.
*/
uint8_t setPinRxDsaSettings(uint8_t afeId, uint8_t chNo, uint8_t dsaInit, uint8_t dsaStep, uint8_t maxDelay)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(dsaInit < AFE_RX_DSA_MAX_ANA_DSA_INDEX);
	AFE_PARAMS_VALID(dsaStep < AFE_RX_DSA_MAX_ANA_DSA_INDEX);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x40 << (chNo >> 1), 0x0, 0x7)); /*dsa_page1*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xe0, (maxDelay)&0xff, 0x0, 0x2));	   /*fdsa_pin_uncert_cyc*/

	if (chNo % 2 == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12c, (dsaStep)&0xff, 0x0, 0x6)); /*fdsa_offset_val_A*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12d, (dsaInit)&0xff, 0x0, 0x5)); /*fdsa_init_val_A*/
	}

	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x17c, (dsaStep)&0xff, 0x0, 0x6)); /*fdsa_offset_val_B*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x17d, (dsaInit)&0xff, 0x0, 0x5)); /*fdsa_init_val_B*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX Digital DSA
    @details Sets the TX Digital DSA.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
    @param bandNo Select the TX Band. 0-Band0, 1-Band1<br>
	@param dig_gain dig_gain is integer value ranging from +24 to -167, that maps to +3dBfs to -20.875dBfs gain. (negative values refers to attenuation)<br>
					The needed attenuation*8 is the dig_gain value to be passed.
	@return Returns if the function execution passed or failed.
*/
uint8_t setTxDigGain(uint8_t afeId, uint8_t chNo, uint8_t bandNo, int16_t dig_gain)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(dig_gain <= 24);
	AFE_PARAMS_VALID(dig_gain >= -167);
	uint8_t dig_gain_val;
	dig_gain_val = 24 - dig_gain;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x10 << (chNo >> 1), 0x0, 0x7)); /*dsa_page0*/
	if (chNo % 2 == 0)
	{
		if (bandNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd0, (dig_gain_val)&0xff, 0x0, 0x7)); /*txa_dsa_dig0_gain*/
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd8, (dig_gain_val)&0xff, 0x0, 0x7)); /*txa_dsa_dig1_gain*/
		}
	}
	else
	{
		if (bandNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xd4, (dig_gain_val)&0xff, 0x0, 0x7)); /*txb_dsa_dig0_gain*/
		}

		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xdc, (dig_gain_val)&0xff, 0x0, 0x7)); /*txb_dsa_dig1_gain*/
		}
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX DSA Gain Swap Attenuation
    @details Set the TX DSA Gain Swap Attenuation. There are 2 Gain Swap settings possible which can be chosen using the pin.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
	@param anaAttn0	Analog Attenuation for Swap Attenuation 0, from 0 to 29. (1dB steps)
	@param anaAttn1	Analog Attenuation for Swap Attenuation 1, from 0 to 29. (1dB steps)
	@param digB0Gain0	Digital Attenuation*8 for Swap Attenuation 0 for band 0<br>
				Is integer value ranging from +24 to -167, that maps to +3dBfs to -20.875dBfs gain (negative values refers to attenuation)<br>
				The needed attenuation*8 is the dig_gain value to be passed
	@param digB0Gain1	Digital Attenuation*8 for Swap Attenuation 1 for band 0<br>
				Is integer value ranging from +24 to -167, that maps to +3dBfs to -20.875dBfs gain (negative values refers to attenuation)<br>
				The needed attenuation*8 is the dig_gain value to be passed
	@param digB1Gain0	Digital Attenuation*8 for Swap Attenuation 0 for band 1<br>
				Is integer value ranging from +24 to -167, that maps to +3dBfs to -20.875dBfs gain (negative values refers to attenuation)<br>
				The needed attenuation*8 is the dig_gain value to be passed
	@param digB1Gain1	Digital Attenuation*8 for Swap Attenuation 1 for band 1<br>
				Is integer value ranging from +24 to -167, that maps to +3dBfs to -20.875dBfs gain (negative values refers to attenuation)<br>
				The needed attenuation*8 is the dig_gain value to be passed
    @return Returns if the function execution passed or failed.
*/
uint8_t txDsaIdxGainSwap(uint8_t afeId, uint8_t chNo, uint8_t anaAttn0, uint8_t anaAttn1, int8_t digB0Gain0, int8_t digB0Gain1, int8_t digB1Gain0, int8_t digB1Gain1)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(anaAttn0 <= AFE_TX_DSA_MAX_ANA_DSA_INDEX);
	AFE_PARAMS_VALID(anaAttn1 <= AFE_TX_DSA_MAX_ANA_DSA_INDEX);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x40 << (chNo >> 1), 0x0, 0x7)); /*dsa_page1*/
	if (chNo % 2 == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1c4, (anaAttn0)&0xff, 0x0, 0x5)); /*txa_dsa_index_swap0		*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1d0, (anaAttn1)&0xff, 0x0, 0x5)); /*txa_dsa_index_swap1		*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1c8, (24 - digB0Gain0) & 0xff, 0x0, 0x7)); /*txa_dsa_dig0_gain_swap0	*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1d4, (24 - digB0Gain1) & 0xff, 0x0, 0x7)); /*txa_dsa_dig0_gain_swap1	*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1cc, (24 - digB1Gain0) & 0xff, 0x0, 0x7)); /*txa_dsa_dig1_gain_swap0	*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1d8, (24 - digB1Gain1) & 0xff, 0x0, 0x7)); /*txa_dsa_dig1_gain_swap1	*/
	}

	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x214, (anaAttn0)&0xff, 0x0, 0x5)); /*txb_dsa_index_swap0		*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x220, (anaAttn1)&0xff, 0x0, 0x5)); /*txb_dsa_index_swap1*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x218, (24 - digB0Gain0) & 0xff, 0x0, 0x7)); /*txb_dsa_dig0_gain_swap0*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x224, (24 - digB0Gain1) & 0xff, 0x0, 0x7)); /*txb_dsa_dig0_gain_swap1*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x21c, (24 - digB1Gain0) & 0xff, 0x0, 0x7)); /*txb_dsa_dig1_gain_swap0*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x228, (24 - digB1Gain1) & 0xff, 0x0, 0x7)); /*txb_dsa_dig1_gain_swap1*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX DSA Update Mode
    @details This function sets the Params of applying TX DSA through Macro.
    @param afeId AFE ID
    @param mode Mode of TX DSA Update<br>
				0-oneshot	(Immediately update)<br>
				1-smoothening (Enable smooth transition of DSA)<br>
				2-TDD mode (Set DSA on TX TDD off state)
	@param transitTime	This value/8 us is the time taken for each step in smoothening mode.
	@param maxAnaDsa	This is the maximum analog DSA (in dB) beyond which the digital gain/attenuation will be applied. Maximum value of this is 29
	@return Returns if the function execution passed or failed.
*/
uint8_t updateTxGainParam(uint8_t afeId, uint8_t mode, uint8_t transitTime, uint8_t maxAnaDsa)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(maxAnaDsa <= AFE_TX_DSA_MAX_ANA_DSA_INDEX);
	uint8_t byteList[2];
	uint8_t numOfOperands = 0;
	byteList[numOfOperands] = (mode);
	numOfOperands++;
	byteList[numOfOperands] = (transitTime);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_TX_DIG_PARAM)); //MacroConsts.MACRO_OPCODE_UPDATE_TX_DIG_PARAM);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, 0x8, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x8, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x03ac, maxAnaDsa, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x03ad, maxAnaDsa, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX DSA.
    @details This function sets the TX DSA (analog+digital) through Macro.<br>
			When the value is less or equal to than the maxAnaDsa setting in updateTxGainParam function, the integer part of the value will be applied to analog and fractional part will be applied to digital.<br>
			When the value is more than the maxAnaDsa setting in updateTxGainParam function, maxAnaDsa will be applied to the analog and rest will be applied in digital.<br>
			For single band case, set same value as band0 to band1 and apply gain validity accordingly.
    @param afeId AFE ID
    @param txChainSel Selects if the DSA attenuation needs to be applied to AB or CD channels.<br>
				0-AB<br>
				1-CD
    @param gainValidity	Selects where all to set the DSA. This is a bit wise field.<br>
				bit 0- TXA/C Band0<br>
				bit 1- TXA/C Band1<br>
				bit 2- TXB/D Band0<br>
				bit 3- TXB/D Band1
	@param tx0B0Dsa TXA/C Band 0 DSA setting *8. For getting attenuation of 2.25dB, this value should be 18. Supported Range is 0-320.
	@param tx0B1Dsa TXA/C Band 1 DSA setting *8. For getting attenuation of 2.25dB, this value should be 18. Supported Range is 0-320.
	@param tx1B0Dsa TXB/D Band 0 DSA setting *8. For getting attenuation of 2.25dB, this value should be 18. Supported Range is 0-320.
	@param tx1B1Dsa TXB/D Band 1 DSA setting *8. For getting attenuation of 2.25dB, this value should be 18. Supported Range is 0-320.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateTxGain(uint8_t afeId, uint8_t txChainSel, uint8_t gainValidity, uint16_t tx0B0Dsa, uint16_t tx0B1Dsa, uint16_t tx1B0Dsa, uint16_t tx1B1Dsa)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(txChainSel < (AFE_NUM_TX_CHANNELS >> 1));
	AFE_PARAMS_VALID(tx0B0Dsa <= (AFE_TX_DSA_MAX_ANA_PLUS_DIG_DSA_DB * 8));
	AFE_PARAMS_VALID(tx0B1Dsa <= (AFE_TX_DSA_MAX_ANA_PLUS_DIG_DSA_DB * 8));
	AFE_PARAMS_VALID(tx1B0Dsa <= (AFE_TX_DSA_MAX_ANA_PLUS_DIG_DSA_DB * 8));
	AFE_PARAMS_VALID(tx1B1Dsa <= (AFE_TX_DSA_MAX_ANA_PLUS_DIG_DSA_DB * 8));
	AFE_PARAMS_VALID(gainValidity <= 0xf);

	uint8_t byteList[10];
	uint8_t numOfOperands = 0;

	byteList[numOfOperands] = (txChainSel);
	numOfOperands++;
	byteList[numOfOperands] = (gainValidity);
	numOfOperands++;

	byteList[numOfOperands] = (tx0B0Dsa & 0xff);
	numOfOperands++;
	byteList[numOfOperands] = (tx0B0Dsa >> 8);
	numOfOperands++;
	byteList[numOfOperands] = (tx0B1Dsa & 0xff);
	numOfOperands++;
	byteList[numOfOperands] = (tx0B1Dsa >> 8);
	numOfOperands++;
	byteList[numOfOperands] = (tx1B0Dsa & 0xff);
	numOfOperands++;
	byteList[numOfOperands] = (tx1B0Dsa >> 8);
	numOfOperands++;
	byteList[numOfOperands] = (tx1B1Dsa & 0xff);
	numOfOperands++;
	byteList[numOfOperands] = (tx1B1Dsa >> 8);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_TX_GAIN)); //MacroConsts.MACRO_OPCODE_UPDATE_TX_GAIN
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX NCO for single band.
    @details This function updates the TX NCO and should be used only single band of operation.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
	@param mixer Mixer frequency.<br>
				Should pass value in KHz in 1KHz ncoFreqMode and the frequency word value in FCW mode. The Mode is determined by the ncoFreqMode set in Latte while generating the bringup script.<br>
				In FCW mode, the value can be calculate using the equation: mixer =  (uint32_t) (2^32*mixerFrequency/Fdac).
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateTxNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t nco)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(nco < 2);
	uint64_t mixerVal;
	uint32_t Fdac = (uint32_t)(systemParams[afeId].Fdac * 1000);
	if (nco == 0)
	{
		systemParams[afeId].txNco[0][chNo][0] = mixer / (float)1000.0;
	}
	else if (nco == 1)
	{
		systemParams[afeId].txNco[1][chNo][0] = mixer / (float)1000.0;
	}
	mixer = mixer % Fdac;
	mixerVal = mixer & 0xffffffff;

	AFE_FUNC_EXEC(updateSystemTxChannelFreqConfig(afeId, chNo, nco, (mixerVal), 1, 1));
	chNo = 1 << chNo;
	AFE_FUNC_EXEC(doSystemTuneSelective(afeId, 0, 0, chNo, 0x20));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, chNo << 4, 0x0, 7)); /*txdig*/
	if (nco == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x0, 0x0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x1, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x0, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x0, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x0, 0x0, 0x0));
	}

	else if (nco == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x0, 0x0, 0x0));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x0, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x1, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x0, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, 0x0, 0x0, 0x7));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, chNo, 0x0, 0x7)); /*txdh*/
	if (mixer <= 900000)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0x10, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x1F, 0x0, 0x7));
	}
	else if (mixer < 2300000)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0xf0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x00, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x00, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the TX NCO for Dual band.
    @details This function updates the TX NCO and should be used only single band of operation.<br>
				For all the mixer frequency values, should pass value in KHz in 1KHz ncoFreqMode and the frequency word value in FCW mode. The Mode is determined by the ncoFreqMode set in Latte while generating the bringup script.<br>
				In FCW mode, the value can be calculate using the equation: mixer =  (uint32_t) (2^32*mixerFrequency/Fdac).<br>
				In case second NCO is not used, set the band1 parameters to same value as band0.
	@param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@param band0Nco0 Band0, NCO0 Mixer frequency.
	@param band1Nco0 Band1, NCO0 Mixer frequency.
	@param band0Nco1 Band0, NCO1 Mixer frequency.
	@param band1Nco1 Band1, NCO1 Mixer frequency.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateTxNcoDb(uint8_t afeId, uint8_t chNo, uint8_t nco, uint32_t band0Nco0, uint32_t band1Nco0, uint32_t band0Nco1, uint32_t band1Nco1)
{
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(nco < 2);
	uint8_t errorStatus = 0;
	uint32_t averageMixerFreq;
	uint32_t Fdac = (uint32_t)(systemParams[afeId].Fdac * 1000);
	uint8_t byteList[18];

	uint8_t numOfOperands = 0;
	uint8_t byteListTxNCO[4];
	systemParams[afeId].txNco[0][chNo][0] = band0Nco0 / (float)1000.0;
	systemParams[afeId].txNco[0][chNo][1] = band1Nco0 / (float)1000.0;
	systemParams[afeId].txNco[1][chNo][0] = band0Nco1 / (float)1000.0;
	systemParams[afeId].txNco[1][chNo][1] = band1Nco1 / (float)1000.0;
	averageMixerFreq = (band0Nco0 >> 2) + (band1Nco0 >> 2) + (band0Nco1 >> 2) + (band1Nco1 >> 2); //Average Mixer frequency
	band0Nco0 = band0Nco0 % Fdac;
	band1Nco0 = band1Nco0 % Fdac;
	band0Nco1 = band0Nco1 % Fdac;
	band1Nco1 = band1Nco1 % Fdac;
	/*
	if (systemParams[afeId].ncoFreqMode == 0)
	{
		band0Nco0 = ((uint32_t)ceil(((band0Nco0 / (double)Fdac) * (0x100000000)))) & 0xffffffff;
		band1Nco0 = ((uint32_t)ceil(((band1Nco0 / (double)Fdac) * (0x100000000)))) & 0xffffffff;
		band0Nco1 = ((uint32_t)ceil(((band0Nco1 / (double)Fdac) * (0x100000000)))) & 0xffffffff;
		band1Nco1 = ((uint32_t)ceil(((band1Nco1 / (double)Fdac) * (0x100000000)))) & 0xffffffff;
	}
	*/
	byteList[numOfOperands] = chNo;
	numOfOperands++;
	AFE_FUNC_EXEC(splitToByte(band0Nco0 & 0xffffffff, 4, byteListTxNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListTxNCO[i];
		numOfOperands++;
	}
	AFE_FUNC_EXEC(splitToByte(band1Nco0 & 0xffffffff, 4, byteListTxNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListTxNCO[i];
		numOfOperands++;
	}
	AFE_FUNC_EXEC(splitToByte(band0Nco1 & 0xffffffff, 4, byteListTxNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListTxNCO[i];
		numOfOperands++;
	}
	AFE_FUNC_EXEC(splitToByte(band1Nco1 & 0xffffffff, 4, byteListTxNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListTxNCO[i];
		numOfOperands++;
	}

	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_SYSTEM_TX_CHANNEL_FREQUENCY_CONFIGURATION_ALL_BANDS));
	chNo = 1 << chNo;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, chNo << 4, 0x0, 7)); /*txdig*/
	if (nco == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x0, 0x0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x1, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x130, 0x0, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x0, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x230, 0x0, 0x0, 0x0));
	}
	else if (nco == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x131, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x0, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x1, 0x0, 0x0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x231, 0x0, 0x0, 0x0));
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x0, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x1, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x773, 0x0, 0x0, 0x0)); /*config_fmixer_update_pulse*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, chNo, 0x0, 0x7)); /*txdh*/
	if (averageMixerFreq <= 900000)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0x10, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x1F, 0x0, 0x7));
	}
	else if (averageMixerFreq < 2300000)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0xf0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x00, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x107, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x105, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x104, 0x40, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10a, 0x0, 0x0, 0x3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x109, 0x0, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x108, 0x00, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x13, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the RX NCO Select.
    @details This function sets the override to the RX NCO select. This is useful only when more than 1 NCO is used.
	@param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param BandId NCO number. 0-NCO0, 1-NCO1.
	@param ovr 1 will override the pin. 0 will give control to the pin.
	@param NCOId NCO number which is to be selected. Supported range is 0 to numRxNco set in the initial configuration.
	@return Returns if the function execution passed or failed.
*/
uint8_t rxNCOSel(uint8_t afeId, uint8_t chNo, uint8_t BandId, uint8_t ovr, uint8_t NCOId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(BandId < AFE_NUM_BANDS_PER_RX);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, (1 << chNo) & 0xff, 0x0, 0x7)); /*rxdig*/
	if (BandId == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2219, (ovr)&0xff, 0x0, 0x0));	  /*b0_nco_switch_ovr_en*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x221a, (NCOId)&0xff, 0x0, 0x3)); /*b0_nco_switch_ovr_val*/
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x221d, (ovr)&0xff, 0x0, 0x0));	  /*b1_nco_switch_ovr_en*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x221e, (NCOId)&0xff, 0x0, 0x0)); /*b1_nco_switch_ovr_val*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the FB NCO Select.
    @details This function sets the override to the FB NCO select. This is useful only when more than 1 NCO is used.
	@param afeId AFE ID
    @param topno Select the FB Channel<br>
			0 for FBAB<br>
			1 for FBCD
	@param ovr 1 will override the pin. 0 will give control to the pin.
	@param NCOId NCO number which is to be selected. Supported range is 0 to numFbNco set in the initial configuration.
	@return Returns if the function execution passed or failed.
*/
uint8_t fbNCOSel(uint8_t afeId, uint8_t topno, uint8_t ovr, uint8_t NCOId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(topno < AFE_NUM_FB_CHANNELS);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, (((1 << (topno))) << 4) & 0xff, 0x0, 0x7)); /*fbdig*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x2219, (ovr)&0xff, 0x0, 0x0));					  /*nco_switch_ovr_en*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x221a, (NCOId)&0xff, 0x0, 0x3));				  /*nco_switch_ovr_val*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the RX NCO.
    @details This function updates the RX NCO.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param mixer Mixer frequency.<br>
				Should pass value in KHz in 1KHz ncoFreqMode and the frequency word value in FCW mode. The Mode is determined by the ncoFreqMode set in Latte while generating the bringup script.<br>
				In FCW mode, the value can be calculate using the equation: mixer =  (uint32_t) (2^32*mixerFrequency/FadcRx).
	@param band Band number. 0-band0, 1-band1.
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateRxNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t band, uint8_t nco)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(band < AFE_NUM_BANDS_PER_RX);
	uint32_t mixerVal = 3000;
	uint32_t Fadc = (uint32_t)(systemParams[afeId].FadcRx * 1000);
	uint8_t byteList[7];
	uint8_t numOfOperands = 0;
	uint8_t byteListRxNCO[4];

	if (nco == 0)
	{
		systemParams[afeId].rxNco[0][chNo][band] = mixer / (float)1000.0;
	}
	else if (nco == 1)
	{
		systemParams[afeId].rxNco[1][chNo][band] = mixer / (float)1000.0;
	}
	mixer = mixer % Fadc;

	mixerVal = (mixer & 0xffffffff);
	if (systemParams[afeId].chipVersion <= 0x12)
	{
		byteList[numOfOperands] = (1 << chNo);
		numOfOperands++;
		byteList[numOfOperands] = (1 << (nco + (band << 1)));
		numOfOperands++;
	}
	else
	{
		byteList[numOfOperands] = (chNo);
		numOfOperands++;
		byteList[numOfOperands] = (nco + (band << 1));
		numOfOperands++;
	}
	AFE_FUNC_EXEC(splitToByte(mixerVal, 4, byteListRxNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListRxNCO[i];
		numOfOperands++;
	}
	byteList[numOfOperands] = (3);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_SYSTEM_RX_CHANNEL_FREQUENCY_CONFIGURATION)); //MacroConsts.MACRO_OPCODE_UPDATE_SYSTEM_RX_CHANNEL_FREQUENCY_CONFIGURATION);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the FB NCO.
    @details This function updates the FB NCO.
    @param afeId AFE ID
    @param chNo Select the FB Channel<br>
			0 for FBAB<br>
			1 for FBCD
	@param mixer Mixer frequency.<br>
				Should pass value in KHz in 1KHz ncoFreqMode and the frequency word value in FCW mode. The Mode is determined by the ncoFreqMode set in Latte while generating the bringup script.<br>
				In FCW mode, the value can be calculate using the equation: mixer =  (uint32_t) (2^32*mixerFrequency/FadcRx).<br>
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@return Returns if the function execution passed or failed.
*/
uint8_t updateFbNco(uint8_t afeId, uint8_t chNo, uint32_t mixer, uint8_t nco)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_FB_CHANNELS);
	uint32_t mixerVal = 3000;
	uint32_t Fadc = (uint32_t)(systemParams[afeId].FadcFb * 1000);
	uint8_t byteList[7];
	uint8_t byteListFbNCO[4];
	uint8_t numOfOperands = 0;

	if (nco == 0)
	{
		systemParams[afeId].fbNco[0][chNo] = mixer / (float)1000.0;
	}
	if (nco == 1)
	{
		systemParams[afeId].fbNco[1][chNo] = mixer / (float)1000.0;
	}
	if (nco == 2)
	{
		systemParams[afeId].fbNco[2][chNo] = mixer / (float)1000.0;
	}
	if (nco == 3)
	{
		systemParams[afeId].fbNco[3][chNo] = mixer / (float)1000.0;
	}

	mixer = mixer % Fadc;

	mixerVal = (mixer & 0xffffffff);
	if (systemParams[afeId].chipVersion <= 0x12)
	{
		byteList[numOfOperands] = (1 << chNo);
		numOfOperands++;
		byteList[numOfOperands] = (1 << nco);
		numOfOperands++;
	}
	else
	{
		byteList[numOfOperands] = (chNo);
		numOfOperands++;
		byteList[numOfOperands] = (nco);
		numOfOperands++;
	}

	AFE_FUNC_EXEC(splitToByte(mixerVal, 4, byteListFbNCO));
	for (uint8_t i = 0; i < 4; i++)
	{
		byteList[numOfOperands] = byteListFbNCO[i];
		numOfOperands++;
	}
	byteList[numOfOperands] = (3);
	numOfOperands++;
	AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, AFE_MACRO_OPCODE_UPDATE_SYSTEM_FB_CHANNEL_FREQUENCY_CONFIGURATION)); //MacroConsts.MACRO_OPCODE_UPDATE_SYSTEM_FB_CHANNEL_FREQUENCY_CONFIGURATION);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the RX NCO.
    @details This function reads the RX NCO and returns it as a pointer. systemParams[afeId].ncoFreqMode should be matched with the value set in the initial configuration.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param band Band number. 0-band0, 1-band1.
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@param ncoFreq Pointer Return. Returns the value of the NCO frequency read in MHz.
	@return Returns if the function execution passed or failed.
*/
uint8_t readRxNco(uint8_t afeId, uint8_t chNo, uint8_t band, uint8_t nco, double *ncoFreq)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(band < AFE_NUM_BANDS_PER_RX);
	AFE_PARAMS_VALID(ncoFreq != NULL);
	uint16_t addr = 0;
	uint32_t fcwVal = 0;
	uint8_t fcwVal_lsb, fcwVal_midlow, fcwVal_midhigh, fcwVal_msb;

	AFE_FUNC_EXEC(enableMemAccess(afeId, 1));
	if (systemParams[afeId].chipVersion <= 0x12)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, 0x04, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x08, 0x0, 0x7));
		addr = 0x61c4 + 4 * (chNo * 4 + band * 2 + nco);
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr, 0, 7, &fcwVal_lsb));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 1, 0, 7, &fcwVal_midlow));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 2, 0, 7, &fcwVal_midhigh));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 3, 0, 7, &fcwVal_msb));
		fcwVal = fcwVal_lsb + (fcwVal_midlow << 8) + (fcwVal_midhigh << 16) + (fcwVal_msb << 24);
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, 0x08, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x08, 0x0, 0x7));
		addr = 0x220 + 4 * (chNo * 4 + band * 2 + nco);
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr, 0, 7, &fcwVal_lsb));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 1, 0, 7, &fcwVal_midlow));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 2, 0, 7, &fcwVal_midhigh));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 3, 0, 7, &fcwVal_msb));
		fcwVal = fcwVal_lsb + (fcwVal_midlow << 8) + (fcwVal_midhigh << 16) + (fcwVal_msb << 24);
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0x0, 0x7));
	}
	if (systemParams[afeId].ncoFreqMode == 0)
	{
		*ncoFreq = (fcwVal * systemParams[afeId].FadcRx) / (0x100000000);
	}
	else
	{
		*ncoFreq = fcwVal / 1000.0;
	}
	AFE_FUNC_EXEC(enableMemAccess(afeId, 0));
	afeLogInfo("RX NCO frequency for chNo: %d, Band: %d and nco number: %d is %f ", chNo, band, nco, *ncoFreq);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the FB NCO.
    @details This function reads the FB NCO.
    @param afeId AFE ID
    @param chNo Select the FB Channel<br>
			0 for FBAB<br>
			1 for FBCD
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@param ncoFreq Pointer Return. Returns the value of the NCO frequency read in MHz.
	@return Returns if the function execution passed or failed.
*/
uint8_t readFbNco(uint8_t afeId, uint8_t chNo, uint8_t nco, double *ncoFreq)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_FB_CHANNELS);
	AFE_PARAMS_VALID(ncoFreq != NULL);
	uint16_t addr = 0;
	uint32_t fcwVal = 0;
	uint8_t fcwVal_lsb, fcwVal_midlow, fcwVal_midhigh, fcwVal_msb;
	AFE_FUNC_EXEC(enableMemAccess(afeId, 1));
	if (systemParams[afeId].chipVersion <= 0x12)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, 0x04, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x08, 0x0, 0x7));
		addr = 0x6204 + 4 * (chNo * 4 + nco);
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr, 0, 7, &fcwVal_lsb));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 1, 0, 7, &fcwVal_midlow));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 2, 0, 7, &fcwVal_midhigh));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 3, 0, 7, &fcwVal_msb));
		fcwVal = fcwVal_lsb + (fcwVal_midlow << 8) + (fcwVal_midhigh << 16) + (fcwVal_msb << 24);
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0x0, 0x7));
	}
	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x144, 0x08, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x08, 0x0, 0x7));
		addr = 0x260 + 4 * (chNo * 4 + nco);
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr, 0, 7, &fcwVal_lsb));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 1, 0, 7, &fcwVal_midlow));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 2, 0, 7, &fcwVal_midhigh));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, addr + 3, 0, 7, &fcwVal_msb));
		fcwVal = fcwVal_lsb + (fcwVal_midlow << 8) + (fcwVal_midhigh << 16) + (fcwVal_msb << 24);
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0x0, 0x7));
	}

	if (systemParams[afeId].ncoFreqMode == 0)
	{
		*ncoFreq = (fcwVal * systemParams[afeId].FadcFb) / (0x100000000);
	}
	else
	{
		*ncoFreq = fcwVal / 1000.0;
	}
	AFE_FUNC_EXEC(enableMemAccess(afeId, 0));
	afeLogInfo("FB NCO frequency for chNo:%d and nco number:%d is %f ", chNo, nco, *ncoFreq);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the TX NCO.
    @details This function reads the RX NCO and returns it as a pointer. systemParams[afeId].ncoFreqMode should be matched with the value set in the initial configuration.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
	@param band Band number. 0-band0, 1-band1.
	@param nco NCO number. 0-NCO0, 1-NCO1.
	@param val Pointer Return. Returns the value of the NCO frequency read in MHz.
	@return Returns if the function execution passed or failed.
*/
uint8_t readTxNco(uint8_t afeId, uint8_t chNo, uint8_t band, uint8_t nco, int64_t *val)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(band < AFE_NUM_BANDS_PER_TX);
	AFE_PARAMS_VALID(nco < 2);
	AFE_PARAMS_VALID(val != NULL);

	/* The below function reads various internal variables and calculates the TX NCO programmed.*/

	float I2 = 0;		 /* Interpolation2 (back Stage) interpolation factor	*/
	float i1Fout = 0;	 /* Interpolation1 (front stage) output sampling rate (equals DAC rate divided by Interpolation 2 factor).	*/
	uint64_t M1Mode = 0; /* Front (stage 1) mixer mode: 0 --> bypassed; 1/2/3/4 --> impact how overall frequency is calculated */
	uint64_t M2Mode = 0; /* Back (stage 2) mixer mode, needs to be accounted for in calculation of frequencies. */
	uint8_t m2fcw_lsb, m2fcw_midlow, m2fcw_midhigh, m2fcw_msb;
	int64_t m1fcw = 0;
	int64_t m2fcw = 0;
	uint8_t readValuetemp = 0;
	uint8_t fcwMode = systemParams[afeId].ncoFreqMode;
	uint64_t readValue64;
	AFE_FUNC_EXEC(enableMemAccess(afeId, 1));
	AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 4 + chNo, &readValue64, 1));
	if ((readValue64 & 1) == 0)
	{
		I2 = (float)(readValue64 >> 1); // readValue64/2.0
	}
	else
	{
		I2 = (float)(readValue64 >> 1) + (float)0.5; // readValue64/2.0
	}

	i1Fout = (float)(systemParams[afeId].Fdac * 1000.0 / I2);

	AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 8 + chNo, &M1Mode, 1));
	AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 12 + chNo, &M2Mode, 1));

	AFE_FUNC_EXEC(readTopMem(afeId, 0x10281, &readValue64, 1));
	fcwMode = readValue64 & 0xff;
	if (fcwMode == 1)
	{ /*# FCW Mode*/
		if (M1Mode == 0)
		{
			m1fcw = 0;
		}

		else if (M1Mode % 2 == 1)
		{
			AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 208 + (band << 6) + (chNo << 4) + (nco << 3), &readValue64, 4));
			m1fcw = readValue64 & 0xffffffff;
		}

		else if (M1Mode % 2 == 0)
		{
			AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 336 + 32 * chNo, &readValue64, 4));
			if (band == 1)
			{
				m1fcw = 0x100000000 - (readValue64 & 0xffffffff);
			}
			else
			{
				m1fcw = readValue64 & 0xffffffff;
			}
		}

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, (1 << (chNo + 4)) & 0xff, 0x0, 0x7)); /*txdig*/
		if (M2Mode == 1)
		{
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x304, 0x0, 0x3, &readValuetemp));
			m2fcw = (readValuetemp << 28);
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x353, 0x0, 0x7, &m2fcw_msb));
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x352, 0x0, 0x7, &m2fcw_midhigh));
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x351, 0x0, 0x7, &m2fcw_midlow));
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x350, 0x0, 0x7, &m2fcw_lsb));
			m2fcw = (m2fcw_msb << 24) + (m2fcw_midhigh << 16) + (m2fcw_midlow << 8) + m2fcw_lsb;
		}
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, 0x0, 0x0, 0x7));

		if (m1fcw > 0x80000000)
		{
			m1fcw = (m1fcw - 0x100000000);
		}

		*val = (int64_t)ceil(m1fcw / I2) + m2fcw;
	}

	else
	{ /*# 1KHz Mode*/
		if (M1Mode == 0)
		{
			m1fcw = 0;
		}
		else if (M1Mode % 2 == 1)
		{
			AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 16 + (band << 5) + (chNo << 3) + (nco << 2), &readValue64, 4));
			m1fcw = readValue64;
		}
		else if (M1Mode % 2 == 0)
		{
			if (band == 0)
			{
				AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 80 + 16 * chNo, &readValue64, 4));
				m1fcw = readValue64;
			}

			else
			{
				AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 80 + (chNo << 4), &readValue64, 4));
				m1fcw = (int64_t)i1Fout - (int64_t)readValue64;
			}
		}

		if (m1fcw > (i1Fout / 2))
		{
			m1fcw = m1fcw - i1Fout;
		}
		if (M2Mode == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, (1 << (chNo + 4)) & 0xff, 0x0, 0x7)); /*txdig*/
			AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x304, 0x0, 0x3, &readValuetemp));
			m2fcw = (int64_t)((readValuetemp * systemParams[afeId].Fdac * 1000) / 16);
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x19, 0x0, 0x0, 0x7));
		}
		else
		{
			AFE_FUNC_EXEC(readTopMem(afeId, 0xfbd4 + 144 + 16 * chNo, &readValue64, 4));
			m2fcw = readValue64 & 0xffffffff;
		}
		*val = m1fcw + m2fcw;
	}

	AFE_FUNC_EXEC(enableMemAccess(afeId, 0));
	afeLogInfo("TX NCO Frequency Read: %lld", *val);
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Set the FB Analog DSA for pin select mode.
    @details AFE has a feature to select the FB DSA value from a set of pre-programmed values using pins. This function sets the FB Analog DSA index. systemParams[afeId].txToFbMode should be set as needed in the initialization.
    @param afeId AFE ID
    @param pinNo Select the pin value for which to program the DSA. The range of this is 0-3.
	@param dsaSetting Analog dsaSetting is FB DSA for the corresponding pin value. dsaSetting*0.5 is the attenuation in dB applied when the pin value is pinNo.
	@return Returns if the function execution passed or failed.
*/
uint8_t setFbDsaPerTx(uint8_t afeId, uint8_t pinNo, uint8_t dsaSetting)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(pinNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(dsaSetting <= AFE_FB_DSA_MAX_ANA_DSA_INDEX);
	AFE_PARAMS_VALID(pinNo <= 3);
	if (systemParams[afeId].txToFbMode == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x10, 0, 7)); /*dsa_page0*/
		if (pinNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x078, dsaSetting, 0, 7)); /*spi_agc_dsa_fb_0*/
		}
		else if (pinNo == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x07c, dsaSetting, 0, 7)); /*spi_agc_dsa_fb_1*/
		}
		else if (pinNo == 2)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x080, dsaSetting, 0, 7)); /*spi_agc_dsa_fb_2*/
		}
		else if (pinNo == 3)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x084, dsaSetting, 0, 7)); /*spi_agc_dsa_fb_3*/
		}
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x00, 0, 7));
	}
	else if (systemParams[afeId].txToFbMode == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x20, 0, 7));
		if (pinNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x078, dsaSetting, 0, 7));
		}
		else if (pinNo == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x07c, dsaSetting, 0, 7));
		}
		else if (pinNo == 2)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x080, dsaSetting, 0, 7));
		}
		else if (pinNo == 3)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x084, dsaSetting, 0, 7));
		}
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x00, 0, 7));
	}
	else
	{
		if (pinNo == 0 || pinNo == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x10, 0, 7));
		}
		else
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x20, 0, 7));
		}
		if (pinNo == 0)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x078, dsaSetting, 0, 7));
		}
		else if (pinNo == 1)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x07c, dsaSetting, 0, 7));
		}
		else if (pinNo == 2)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x080, dsaSetting, 0, 7));
		}
		else if (pinNo == 3)
		{
			AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x084, dsaSetting, 0, 7));
		}
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x00, 0, 7));
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Enable the pin select based Mode for FB DSA.
    @details AFE has a feature to select the FB DSA value from a set of pre-programmed values using pins. This function sets the FB Analog DSA index.
    @param afeId AFE ID
	@param en en as 1 will enable the feature to set FB DSA per TX based on the GPIO.
	@return Returns if the function execution passed or failed.
*/
uint8_t fbDsaPerTxEn(uint8_t afeId, uint8_t en)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x30, 0, 7));   /*dsa_page0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x079, en & 1, 0, 7)); /*enable_fbmuxsel_for_fbdsa*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x013, 0x00, 0, 7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
