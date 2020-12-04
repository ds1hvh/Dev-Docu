/** @file controls.c
 * 	@brief	This file has generic control related functions.<br>
 * 		<b> Version 2.2:</b> <br>
 * 		1. Fixed the bug in function getChipVersion.<br>
 * 		2. Updated description of checkPllLockStatus.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation and improved the parameter validity checks.<br>
 * 		2. Removed redundant writes in functions.<br>
 * 		3. Changed the C macros for all the spi wrapper function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
 * 		4. Checking if the sysref reached added to the sendSysref function. The function returns fail if the sysref fails to return.<br>
 * 		5. For all PLL Access, the selection between the SPIA and SPIB is changed to a system parameter to cut down the redundant need to pass it in all the functions, since in a typical use case, only one SPI (SPIA) is used for it.<br>
 * 		6. Added checkDeviceHealth function to return a overall status of the device.<br>
*/

#include <stdint.h>

#include <math.h>

#include "afe79xxLog.h"
#include "afe79xxTypes.h"

#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "basicFunctions.h"
#include "afeParameters.h"
#include "controls.h"
#include "hMacro.h"
#include "agc.h"
#include "jesd.h"
#include "pap.h"

/**
    @brief Reads the Chip version of the AFE
    @details This function Reads the Chip version, logs it and also updates the same in the System Params (systemParams[afeId].chipVersion).
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t getChipVersion(uint8_t afeId)
{
	AFE_ID_VALIDITY();
	uint8_t byteList[1];
	uint8_t numOfOperands = 0;
	uint8_t chipVersion, readValue;
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x003, 0x0, 0x7, &readValue));
	if (readValue == 0x10)
	{
		systemParams[afeId].chipVersion = 0x10;
		afeLogInfo("%s", "Chip Version is 0x10");
	}
	else
	{
		byteList[numOfOperands] = (2);
		numOfOperands++;
		AFE_FUNC_EXEC(executeMacro(afeId, byteList, numOfOperands, 0x1));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x20, 0, 7));
		AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x00fc, 0x0, 0x7, &chipVersion));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x18, 0x00, 0, 7));
		if (chipVersion == 0xe3)
		{
			systemParams[afeId].chipVersion = 0x12;
			afeLogInfo("%s", "Chip Version is 0x12");
		}
		else if (chipVersion == 0xf8)
		{
			systemParams[afeId].chipVersion = 0x13;
			afeLogInfo("%s", "Chip Version is 0x13");
		}
		else
		{
			afeLogErr("%s", "chip Version not recognised.");
		}
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Check if the Sysref Reached
    @details This function Checks if the Sysref is detected by the AFE.
    @param afeId AFE ID
    @param clearSysrefFlag Setting this to 1 clear the Sysref flag before reading.
    @param sysrefReceived Pointer return. Value of 1 means Sysref reached. 0 means Sysref reached. 0 means it didn't reach.
	@return Returns if the function execution passed or failed.
*/
uint8_t checkSysref(uint8_t afeId, uint8_t clearSysrefFlag, uint8_t *sysrefReceived)
{
	uint8_t errorStatus = 0;
	uint8_t readVal = 0;
	AFE_ID_VALIDITY();

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x01, 0x0, 0x7));
	if (clearSysrefFlag)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x08, 0x0, 0x7));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0128, 0x00, 0x0, 0x7));
	}
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x130, 0x0, 0x3, &readVal));
	*sysrefReceived = (readVal >> 3) & 1;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0016, 0x00, 0x0, 0x7));

	if (clearSysrefFlag == 0)
	{
		if (*sysrefReceived)
			afeLogDbg("%s", "AFE Sysref Received.");
		else
			afeLogErr("%s", "AFE Sysref Not Received.");
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Give a new Sysref to the AFE
    @details This function is used to send a new sysref to the AFE. This enables the latch and performs the required operations for AFE to accept a new Sysref. <br>
			Note the following:<br>
			1. Contents of the giveSingleSysrefPulse function should be replaced by host function to give Pin Sysref to AFE. This is used only in case of a single shot sysref.<br>
			2. For Continuous Syref mode, external Pin Sysref should be enabled before this function is called. Note that even in this case, only one pulse edge will be captured by the AFE. In this mode, giveSingleSysrefPulse needn't do any operation. <br>
			3. systemParams[afeId].spiInUseForPllAccess should be set before the function call to the appropriate value for selecting SPIA/SPIB. In Normal use-case SPIA is used and hence can be left at the default.<br>
			4. The selection between the single shot and continuous sysref mode should be done in Latte during generation of the configuration log.
    @param afeId AFE ID
    @param spiSysref If this is set to 0, external pin based Sysref is used. If this is set to 1, then the internal override of the Sysref pin will be used. Note that in this case, deterministic latency will not satisfied.
    @param getSpiAccess Setting this to 1 will take PLL SPI access. This should always be set 1.
	@return Returns if the function execution passed or failed.
*/

uint8_t sendSysref(uint8_t afeId, uint8_t spiSysref, uint8_t getSpiAccess)
{
	uint8_t errorStatus = 0;
	uint8_t sysrefReached = 0;
	AFE_ID_VALIDITY();

	AFE_FUNC_EXEC(checkSysref(afeId, 1, &sysrefReached));

	if (getSpiAccess == 1)
	{
		AFE_FUNC_EXEC(requestPllSpiAccess(afeId, systemParams[afeId].spiInUseForPllAccess));
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x80, 0x0, 0x7)); /*timing_controller*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85b, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85a, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x859, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x858, 0x0, 0x0, 0x7));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85b, 0x1, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85a, 0x1, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x859, 0x1, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x858, 0x1, 0x0, 0x7));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85b, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x85a, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x859, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x858, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x1, 0x0, 0x7)); /*pll*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6a, 0x0, 0x1, 0x1));

	if (spiSysref == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6e, 0x1, 0x0, 0x0)); /*LCMGEN_USE_SPI_SYSREF*/
	}

	else
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6e, 0x0, 0x0, 0x0)); /*LCMGEN_USE_SPI_SYSREF*/
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6a, 0x0, 0x1, 0x1));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6a, 0x2, 0x1, 0x1));

	AFE_FUNC_EXEC(waitMs(10));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0x0, 0x7));
	if (spiSysref == 0)
	{
		/*		Need to Enable the Sysref toggle here in case of single shot sysref. Host function to be called here.	*/
		AFE_FUNC_EXEC(giveSingleSysrefPulse(afeId));
		/* Sysref Enable Host function Call. */
	}
	else if (spiSysref == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x1, 0x0, 0x7)); /*pll*/
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6e, 0x0, 0x1, 0x1)); /*LCMGEN_SPI_SYSREF*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6e, 0x2, 0x1, 0x1)); /*LCMGEN_SPI_SYSREF*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x6e, 0x0, 0x1, 0x1)); /*LCMGEN_SPI_SYSREF*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0x0, 0x7));
	}

	if (getSpiAccess == 1)
	{
		AFE_FUNC_EXEC(requestPllSpiAccess(afeId, 0));
	}
	AFE_FUNC_EXEC(checkSysref(afeId, 0, &sysrefReached));
	if (sysrefReached != 1)
	{
		errorStatus |= 1;
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Override TDD Control Signals and set the SPI override value
    @details This function overrides SPI TDD Control Signals and set the SPI override value
    @param afeId AFE ID
    @param rx Override Value of the RX chain.<br>
		This is Bit wise channel select<br>
			Bit0 for RXA<br>
			Bit1 for RXB<br>
			Bit2 for RXC<br>
			Bit3 for RXD
    @param fb Override Value of the FB chain.<br>
		This is Bit wise channel select<br>
			Bit0 for FBAB<br>
			Bit1 for FBCD
    @param tx Override Value of the TX chain.<br>
		This is Bit wise channel select<br>
			Bit0 for TXA<br>
			Bit1 for TXB<br>
			Bit2 for TXC<br>
			Bit3 for TXD
	@param enableOverride Enables the Override.<br>
			if enableOverride=0, it disables the TDD override<br>
			if enableOverride=1, it enables the TDD override && also sets the TDD values<br>
			if enableOverride=2, it only sets the TDD values
	@return Returns if the function execution passed or failed.
*/
uint8_t overrideTdd(uint8_t afeId, uint8_t rx, uint8_t fb, uint8_t tx, uint8_t enableOverride)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(rx <= AFE_NUM_RX_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(fb <= AFE_NUM_FB_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(tx <= AFE_NUM_TX_CHANNELS_BITWISE);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x80, 0x0, 0x7)); /*timing_controller*/

	if (enableOverride == 1 || enableOverride == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xec, (enableOverride)&1, 0x0, 0x0)); /*Override Pins*/

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xf4, (enableOverride)&1, 0x0, 0x0));

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xe4, (enableOverride)&1, 0x0, 0x0));
	}

	if (enableOverride != 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xed, (rx)&0xff, 0x0, 0x3));

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xf5, (fb)&0xff, 0x0, 0x1));

		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xe5, (tx)&0xff, 0x0, 0x3));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Override TDD Control Signals
    @details This function Set the override values for each of RX,FB,TX TDD pins.
    @param afeId AFE ID
    @param rx Override enable Value of the RX chain. 1 sets the pin value in override state. 0 removes the override and gives control to pins.
    @param fb Override enable Value of the FB chain. 1 sets the pin value in override state. 0 removes the override and gives control to pins.
    @param tx Override enable Value of the TX chain. 1 sets the pin value in override state. 0 removes the override and gives control to pins.
	@return Returns if the function execution passed or failed.
*/
uint8_t overrideTddPins(uint8_t afeId, uint8_t rx, uint8_t fb, uint8_t tx)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(rx <= AFE_NUM_RX_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(fb <= AFE_NUM_FB_CHANNELS_BITWISE);
	AFE_PARAMS_VALID(tx <= AFE_NUM_TX_CHANNELS_BITWISE);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x80, 0x0, 0x7));		/*timing_controller*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xec, (rx)&0x1, 0x0, 0x0)); /*use_reg_for_rxtdd*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xf4, (fb)&0x1, 0x0, 0x0)); /*use_reg_for_fbtdd*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xe4, (tx)&0x1, 0x0, 0x0)); /*use_reg_for_txtdd*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x15, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the PLL Lock Status
    @details This function checks the PLL Lock Status and returns it as a pointer. 
    @param afeId AFE ID
    @param pllLockStatus Pointer Return of the lock statud of the PLL. 3 is ideal good state.<br>
		 	0: LOCK is low and LOCK_LOST is high. PLL is currently not locked but locked some time in the past since the status clear bit was last toggled. <br>
		 	1: LOCK is high and LOCK_LOST is high. PLL is currently locked but lost lock since the status clear bit was last toggled. (since clearPllStickyLockStatus was called)<br>
		 	2: LOCK is low and LOCK_LOST is low. PLL is currently not locked and never locked since the status clear bit was last toggled.<br>
		 	3: LOCK is high and LOCK_LOST is low. PLL is currently locked and didn't lose lock since the status clear bit was last toggled. (since clearPllStickyLockStatus was called).
	@return Returns if the function execution passed or failed.
*/
uint8_t checkPllLockStatus(uint8_t afeId, uint8_t *pllLockStatus)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	uint8_t ulRegValue = 0;
	*pllLockStatus = 0;
	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, systemParams[afeId].spiInUseForPllAccess));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0066, 0x0, 0x7, &ulRegValue));
	if (((ulRegValue >> 4) & 1) == 0)
	{
		afeLogErr("%s", "PLL didn't lock");
	}
	else
	{
		*pllLockStatus |= 1;
		afeLogInfo("%s", "PLL locked.");
	}
	if (((ulRegValue >> 6) & 1) == 0)
	{
		*pllLockStatus |= 2;
		afeLogInfo("%s", "PLL LOCK_LOST_STICKY is low. That is, PLL didn't lose Lock. ");
	}
	else
	{
		afeLogInfo("%s", "PLL LOCK_LOST_STICKY is high. That is, PLL lost lock in between. ");
		//errorStatus |= 1;
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, 0));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears the PLL Lock Sticky Status
    @details This function clears the PLL Lock sticky Status.
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t clearPllStickyLockStatus(uint8_t afeId)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, systemParams[afeId].spiInUseForPllAccess));

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0063, 0x40, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0063, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(requestPllSpiAccess(afeId, 0));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the Alarm Pin Status.
    @details This function reads the Alarm Pin Status and returns it as a pointer.
    @param afeId AFE ID
    @param alarmNo Choose the Alarm Pin Number (0/1)
    @param status Pointer return Status of the alarm pin. 0 means there is no alarm and 1 means there is alarm.
	@return Returns if the function execution passed or failed.
*/
uint8_t readAlarmPinStatus(uint8_t afeId, uint8_t alarmNo, uint8_t *status)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(status != NULL);
	AFE_PARAMS_VALID(alarmNo < 2);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x10, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x01417 + alarmNo, 0x0, 0x0, status));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Clears the SPI Alarm Status.
    @details This function clears the SPI Alarm Sticky Status. This is important when multiple SPIs are used and not critical when single SPI is being used.
    @param afeId AFE ID
    @return Returns if the function execution passed or failed.
*/
uint8_t clearSpiAlarms(uint8_t afeId)
{
	AFE_ID_VALIDITY();
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x001b, 0xff, 0x0, 0x7)); /*alarms_clear=0x1ff*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x001d, 0x01, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x001b, 0x00, 0x0, 0x7)); /*alarms_clear=0x0*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x001d, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the SPI Alarm Status.
    @details This function reads the Alarm Status and returns it as a pointer. It also prints the error description.
    @param afeId AFE ID
    @param alarmStatus Pointer return status of the SPI alarm. 0 means there are no alarms and 1 means there is a alarm.
	@return Returns if the function execution passed or failed.
*/
uint8_t readSpiAlarms(uint8_t afeId, uint8_t *alarmStatus)
{
	AFE_ID_VALIDITY();
	uint8_t errorStatus = 0;
	uint16_t alarmVal = 0;
	uint8_t readValue_lsb, readValue_msb;
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x01e, 0x0, 0x7, &readValue_lsb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x01f, 0x0, 0x0, &readValue_msb));
	alarmVal = readValue_lsb + (readValue_msb << 8);
	const char *spiAlarms[9];

	spiAlarms[0] = "Both SPI-A & B1 access Global page at the same time.";
	spiAlarms[1] = "Both SPI-A & B1 access same page at the same time.";
	spiAlarms[2] = "Both SPI-A & B2 access Global page at the same time.";
	spiAlarms[3] = "Both SPI-A & B2 access same page at the same time.";
	spiAlarms[4] = "Both SPI-B1 & B2 access Global page at the same time.";
	spiAlarms[5] = "Both SPI-B1 & B2 access same page at the same time.";
	spiAlarms[6] = "Invalid Address Accessed by SPI-A.";
	spiAlarms[7] = "Invalid Address Accessed by SPI-B1.";
	spiAlarms[8] = "Invalid Address Accessed by SPI-B2.";
	*alarmStatus = 0;
	for (uint8_t a = 0; a < 9; a++)
	{
		if (((alarmVal >> a) & 1) == 1)
		{
			afeLogErr("SPI Alarm seen: %s", spiAlarms[a]);
			*alarmStatus |= 1;
		}
	}
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the TX power.
    @details This function reads the TX Power.
    @param afeId AFE ID
    @param chNo Select the TX Channel<br>
			0 for TXA<br>
			1 for TXB<br>
			2 for TXC<br>
			3 for TXD
    @param windowLen Determines the window length for number of samples. <br>
		2^(windowLen+5) samples at the interface rate will be used for power measurement. Range of this is 0-0xfff
    @param powerReadB0 Pointer Return of Band 0 Power Read
    @param powerReadB1 Pointer Return of Band 1 Power Read
	@return Returns if the function execution passed or failed.
*/
uint8_t readTxPower(uint8_t afeId, uint8_t chNo, uint16_t windowLen, double *powerReadB0, double *powerReadB1)
{
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_TX_CHANNELS);
	AFE_PARAMS_VALID(windowLen <= 0xfff);
	AFE_PARAMS_VALID(powerReadB0 != NULL);
	AFE_PARAMS_VALID(powerReadB1 != NULL);
	uint32_t powerRead = 0;
	uint8_t errorStatus = 0;

	uint8_t pap_blk_status, alarm_mask_status, single_status, combined_status, readValue_lsb, readValue_msb, readValue_mid;
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0019, (1 << (chNo + 4)), 0, 7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0528, 0, 0, &pap_blk_status));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x052d, 0, 3, &alarm_mask_status));
	if (pap_blk_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x052d, 0xf, 0, 3));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0528, 0x01, 0, 0));
	}

	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0525, 0, 0, &single_status));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0524, 0, 0, &combined_status));
	if (combined_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0525, 0x01, 0, 0));
	}
	if (single_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0524, 0x01, 0, 0));
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0571, (windowLen >> 8) & 0xf, 0, 3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0570, windowLen & 0xff, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x056c, 0x01, 0, 0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0521, (windowLen >> 8) & 0xf, 0, 3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0520, windowLen & 0xff, 0, 7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x051c, 0x01, 0, 0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x056d, 0x01, 0, 0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x056d, 0x00, 0, 0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x051d, 0x01, 0, 0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x051d, 0x00, 0, 0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05a0, 0, 7, &readValue_lsb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05a1, 0, 7, &readValue_mid));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05a2, 0, 0, &readValue_msb));
	powerRead = readValue_lsb + (readValue_mid << 8) + (readValue_msb << 16);
	*powerReadB0 = 10 * log10(powerRead * 1.0 / 0x10000);
	afeLogInfo("Band 0 Power Read %lfdBFS", *powerReadB0);
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05b0, 0, 7, &readValue_lsb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05b1, 0, 7, &readValue_mid));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x05b2, 0, 0, &readValue_msb));
	powerRead = readValue_lsb + (readValue_mid << 8) + (readValue_msb << 16);
	*powerReadB1 = 10 * log10(powerRead * 1.0 / 0x10000);
	afeLogInfo("Band 1 Power Read %lfdBFS", *powerReadB1);
	
	if (combined_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0525, 0x00, 0, 0));
	}
	if (single_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0524, 0x00, 0, 0));
	}
	if (pap_blk_status == 0)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0528, 0x00, 0, 0));
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x052d, alarm_mask_status, 0, 3));
	}

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0019, 0, 0, 7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the RX power.
    @details This function reads the RX Power.<br>
			Note that this detector is near the ADC-DDC interface and needs the RX TDD to be ON.<br>
			For reading FB power needed in ADC shared case, it should be operated in RX Mode and correponding RX channel should be read.
    @param afeId AFE ID
    @param chNo Select the RX Channel<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
    @param avg_pwrdb Pointer Return of RX Power Read
	@return Returns if the function execution passed or failed.
*/
uint8_t getRxRmsPower(uint8_t afeId, uint8_t chNo, double *avg_pwrdb)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(avg_pwrdb != NULL);
	uint16_t avg_pwr;
	uint8_t pwr_det_on_state = 0;
	uint8_t readValue, avg_pwr_lsb, avg_pwr_msb;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 1 << chNo, 0x0, 0x7)); /*rxdig*/
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0773, 0x0, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x404, 0x5, 0x5, &readValue));
	if (readValue == 0x0)
	{
		pwr_det_on_state = 1;
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x404, 0x1 << 5, 0x5, 0x5));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0xb04, 1, 0x0, 0x0)); /*pwr_det_read_sel_agc*/

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x5c4, 0x0, 0x0, 0x0));

	AFE_FUNC_EXEC(waitMs(1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x5c4, 0x1, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x5d1, 0x0, 0x7, &avg_pwr_msb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x5d0, 0x0, 0x7, &avg_pwr_lsb));
	avg_pwr = (avg_pwr_msb << 8) + avg_pwr_lsb;

	*avg_pwrdb = 10 * log10(avg_pwr / 65536.0);
	afeLogInfo("RX channel %d, Average Power read in dbfs %lf", chNo, avg_pwrdb);
	if (pwr_det_on_state == 1)
	{
		AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x404, 0x0 << 5, 0x5, 0x5));
	}
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Read the FB power.
    @details This function reads the FB Power.<br>
			Note that this detector is near the ADC-DDC interface and needs the RX TDD to be ON.<br>
			For reading FB power needed in ADC shared case, it should be operated in RX Mode and correponding RX channel should be read.
    @param afeId AFE ID
    @param chNo Select the FB Channel<br>
			0 for FB1<br>
			1 for FB2<br>
    @param avg_pwrdb Pointer Return of FB Power Read
	@return Returns if the function execution passed or failed.
*/
uint8_t getFbRmsPower(uint8_t afeId, uint8_t chNo, double *avg_pwrdb)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_FB_CHANNELS);
	AFE_PARAMS_VALID(avg_pwrdb != NULL);
	uint16_t avg_pwr;
	uint8_t readValue, avg_pwr_lsb, avg_pwr_msb;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x10 << chNo, 0x0, 0x7)); /*fbdig*/
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x0773, 0x0, 0x0, &readValue));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0773, 0x0, 0x0, 0x7));	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1015, 0x04, 0x2, 0x2));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x00, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x00, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0408, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0416, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0415, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0414, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x042d, 0x0e, 0x0, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x042c, 0x42, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0452, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0451, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0450, 0x08, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x00, 0x1, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x00, 0x1, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0409, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x041a, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0419, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0418, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0431, 0x0b, 0x0, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0430, 0x53, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0456, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0455, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0454, 0x08, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x00, 0x4, 0x4));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x00, 0x4, 0x4));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x040c, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0424, 0x12, 0x0, 0x4));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x043d, 0x40, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x043c, 0x4e, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x00, 0x2, 0x2));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x00, 0x2, 0x2));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x040a, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x041e, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x041d, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x041c, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0435, 0x0e, 0x0, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0434, 0x42, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x045a, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0459, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0458, 0x08, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x00, 0x3, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x00, 0x3, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x040b, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0422, 0x04, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0421, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0420, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0439, 0x0b, 0x0, 0x3));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0438, 0x53, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x045e, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x045d, 0x00, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x045c, 0x08, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0400, 0x20, 0x5, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0404, 0x20, 0x5, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x040d, 0x02, 0x0, 0x5));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0425, 0x12, 0x0, 0x4));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x043f, 0x40, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x043e, 0x4e, 0x0, 0x7));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0498, 0x00, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0498, 0x00, 0x1, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1015, 0x00, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0b04, 0x01, 0x0, 0x0));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x5c4, 0x0, 0x0, 0x0));
	AFE_FUNC_EXEC(waitMs(1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x5c4, 0x1, 0x0, 0x0));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x5d1, 0x0, 0x7, &avg_pwr_msb));
	AFE_FUNC_EXEC(afeSpiReadWrapper(afeId, 0x5d0, 0x0, 0x7, &avg_pwr_lsb));
	avg_pwr = (avg_pwr_msb << 8) + avg_pwr_lsb;

	*avg_pwrdb = 10 * log10(avg_pwr / 65536.0);
	afeLogInfo("FB channel %d, Average Power read in dbfs %lf", chNo, avg_pwrdb);
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0773, readValue, 0x2, 0x2));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x1015, 0x00, 0x2, 0x2));
	
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x12, 0x0, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}


/**
    @brief Clear all the alarms
    @details Clears all the AFE alarms
    @param afeId AFE ID
	@return Returns if the function execution passed or failed.
*/
uint8_t clearAllAlarms(uint8_t afeId)
{
	AFE_ID_VALIDITY();
	uint8_t errorStatus = 0;
	AFE_FUNC_EXEC(clearSpiAlarms(afeId));
	AFE_FUNC_EXEC(clearJesdRxAlarms(afeId));
	AFE_FUNC_EXEC(clearJesdRxAlarmsForPap(afeId));
	AFE_FUNC_EXEC(clearJesdTxAlarms(afeId));
	AFE_FUNC_EXEC(clearPllStickyLockStatus(afeId));
	AFE_FUNC_EXEC(clearPapAlarms(afeId, 0xf));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Override Alarm Pin output and set the SPI override value
    @details This function overrides Alarm Pin output and sets it to SPI override value
    @param afeId AFE ID
    @param alarmNo Select the Alarm number, Alarm Pin Number 0/1.
	@param overrideSel 0-Don't override. 1-Override the pin output.
	@param overrideVal When overrideSel is 1, this is the value sent out onto pin (0/1).
	@return Returns if the function execution passed or failed.
*/
uint8_t overrideAlarmPin(uint8_t afeId, uint8_t alarmNo, uint8_t overrideSel, uint8_t overrideVal)
{
	AFE_ID_VALIDITY();
	uint8_t errorStatus = 0;

	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x10, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10bd + (alarmNo * 4), (overrideSel << 1) + overrideVal, 0x0, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Override RX Reliability Pin output and set the SPI override value
    @details This function overrides the RX Reliability Pin output and sets it to SPI override value
    @param afeId AFE ID
    @param chNo Select the RX channel number.<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param overrideSel 0-Don't override. 1-Override the pin output.
	@param overrideVal When overrideSel is 1, this is the value sent out onto pin (0/1).
	@return Returns if the function execution passed or failed.
*/
uint8_t overrideRelDetPin(uint8_t afeId, uint8_t chNo, uint8_t overrideSel, uint8_t overrideVal)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x10, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x10fd + (chNo * 8), (overrideSel << 1) + overrideVal, 0x0, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Override RX Peak Detector Pin output and set the SPI override value
    @details This function RX Peak Detector Pin output and sets it to SPI override value
    @param afeId AFE ID
    @param chNo Select the RX channel number.<br>
			0 for RXA<br>
			1 for RXB<br>
			2 for RXC<br>
			3 for RXD
	@param pinNo Pin Number to be overridden. Supported values are 0-3.
	@param overrideSel 0-Don't override. 1-Override the pin output.
	@param overrideVal When overrideSel is 1, this is the value sent out onto pin (0/1).
	@return Returns if the function execution passed or failed.
*/
uint8_t overrideDigPkDetPin(uint8_t afeId, uint8_t chNo, uint8_t pinNo, uint8_t overrideSel, uint8_t overrideVal)
{
	uint8_t errorStatus = 0;
	AFE_ID_VALIDITY();
	AFE_PARAMS_VALID(chNo < AFE_NUM_RX_CHANNELS);
	AFE_PARAMS_VALID(pinNo < 2);
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x10, 0x0, 0x7));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x106d + (chNo * 0x10) + (pinNo * 4), (overrideSel << 1) + overrideVal, 0x0, 0x1));
	AFE_FUNC_EXEC(afeSpiWriteWrapper(afeId, 0x0015, 0x00, 0x0, 0x7));
	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}

/**
    @brief Checks the Device Health
    @details This function Reads the complete device health and returns as a pointer.
    @param afeId AFE ID
	@param allOk Pointer return of the device health status.<br>
		If there is no error, allOk will be 0.<br>
			If it is non-zero, below is the interpretation<br>
			Bit 0: PLL Not Okay<br>
			Bit 1: DAC JESD Not Okay<br>
			Bit 2: ADC JESD Not Okay<br>
			Bit 3: SPI Not Okay<br>
			Bit 4: MCU Not Okay<br>
			Bit 5: PAP Triggered
	@return Returns if the function execution passed or failed.
*/
uint8_t checkDeviceHealth(uint8_t afeId, uint16_t *allOk)
{
	uint8_t errorStatus = 0;
	uint8_t linkError = 0;
	uint8_t pllStatus = 0;
	uint8_t spiAlarm = 0;
	uint8_t mcuHealth = 0;
	uint8_t papStatus = 0;
	uint16_t linkStatus = 0;

	*allOk = 1;

	/*	#### PLL Status	#### */
	AFE_FUNC_EXEC(checkPllLockStatus(afeId, &pllStatus));
	if (pllStatus != 3)
	{
		*allOk = 0;
		afeLogErr("%s", "PLL lost lock currently or in the past.");
	}
	else
	{
		afeLogInfo("%s", "AFE DAC-JESD-RX relink Successful.");
	}

	/*	#### JESD Status	#### */
	AFE_FUNC_EXEC(getJesdRxLinkStatus(afeId, &linkStatus));
	if (linkStatus != 10)
	{
		afeLogErr("%s", "AFE DAC-JESD-RX Link Down.");
		*allOk = 0;
	}
	else
	{
		afeLogInfo("%s", "AFE DAC-JESD-RX Link is good.");
	}

	AFE_FUNC_EXEC(getJesdRxAlarms(afeId, &linkError));
	if (linkError != 0)
	{
		afeLogErr("%s", "AFE DAC-JESD-RX has some errors triggered.");
		*allOk = 0;
	}
	else
	{
		afeLogInfo("%s", "AFE DAC-JESD-RX has No Errors triggered.");
	}

	/* ADC JESD */
	AFE_FUNC_EXEC(getJesdTxFifoErrors(afeId, 0, &linkError));
	if (linkError != 0)
	{
		afeLogErr("%s", "AFE ADC-JESD-TX AB has some errors triggered.");
		*allOk = 0;
	}
	else
	{
		afeLogInfo("%s", "AFE ADC-JESD-TX AB has no Errors triggered.");
	}
	AFE_FUNC_EXEC(getJesdTxFifoErrors(afeId, 1, &linkError));
	if (linkError != 0)
	{
		afeLogErr("%s", "AFE ADC-JESD-TX CD has some errors triggered.");
		*allOk = 0;
	}
	else
	{
		afeLogInfo("%s", "AFE ADC-JESD-TX CD has no Errors triggered.");
	}

	/* SPI Alarms */
	AFE_FUNC_EXEC(readSpiAlarms(afeId, &spiAlarm));
	if (spiAlarm != 0)
	{
		*allOk = 0;
		afeLogErr("%s", "SPI Alarm triggered.");
	}
	else
	{
		afeLogInfo("%s", "SPI Alarm Not triggered.");
	}

	/* MCU Health */
	AFE_FUNC_EXEC(checkMcuHealth(afeId, &mcuHealth));
	if (mcuHealth != 0)
	{
		*allOk = 0;
		afeLogErr("%s", "MCU Not Running.");
	}
	else
	{
		afeLogInfo("%s", "MCU Okay.");
	}

	/* PAP Status */
	for (uint8_t chNo = 0; chNo < 4; chNo++)
	{
		AFE_FUNC_EXEC(papAlarmStatus(afeId, chNo, &papStatus));
		if (papStatus == 1)
		{
			afeLogErr("PAP triggered for TX Channel Number: %d", chNo);
			*allOk = 0;
		}
	}

	if (errorStatus)
		return RET_EXEC_FAIL;
	else
		return RET_OK;
}
