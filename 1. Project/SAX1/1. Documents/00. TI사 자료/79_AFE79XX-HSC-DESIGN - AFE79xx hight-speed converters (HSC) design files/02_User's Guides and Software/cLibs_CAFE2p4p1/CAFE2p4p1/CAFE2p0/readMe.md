@page ReadMe ReadMe

@section VersionHistory VersionHistory

@subsection Version2p2 Version:2.2
	agc.c:<br>
		1. Updated the agcStateControlMacro description<br>
		2. Fixed macro opcode bug in agcDigDetRelativeNumCrossingConfig.<br>
		
	basicFunctions.c:<br>
		1. Updated the log comment in serdesRawWrite function.<br>
		
	controls.c:<br>
		1. Fixed the bug in function getChipVersion.<br>
		2. Updated description of checkPllLockStatus.<br>
		
	dsaAndNco.c:<br>
		1. Updated setTxDigGain and txDsaIdxGainSwap along with the description and parameter validity.<br>
		
	jesd.c:<br>
		1. Fixed bug in adcDacSync.<br>
		
	serDes.c:<br>
		1. Fixed a bug in getSerDesEye function.<br>
		
@subsection Version2p1p1 Version:2.1.1
	agc.c:<br>
		1. Added more functions. agcDigDetRelativeNumCrossingConfig, externalAgcConfig, internalAgcConfig, rfAnalogDetConfig, extLnaConfig, extLnaGainConfig<br>
		2. Removed Automatically calling State Control Macro to giver user better flexibility.<br>
		
@subsection Version2p1 Version:2.1
	afe79xxLog.h:<br>
		1. Added documentation Level for SPI Log.
		
	afeParameters.h:<br>
		1. Added documentation.
		
	agc.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Changed the C macro for executing the executeMacro function to AFE_FUNC_EXEC.<br>
		3. Changed hard coded OPCODES to #defines in afe79xxTypes.h.<br>
		4. Changed the C macros for all the spi wrapper function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		
	basicFunctions.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Changed the C macros for all the spi wrapper function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		3. Added closeAllPages function.<br>
		
	calibrations.c:<br>
		1. Added this file only in version 2.1.<br>
		2. Added documentation and improved the parameter validity checks.<br>
		3. Modified the RX DSA calibration function to add placeholder function for channel inputs.<br>
		4. Added TX DSA calibration function.<br>
		
	controls.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Removed redundant writes in functions.<br>
		3. Changed the C macros for all the spi wrapper function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		4. Checking if the sysref reached added to the sendSysref function. The function returns fail if the sysref fails to return.<br>
		5. For all PLL Access, the selection between the SPIA and SPIB is changed to a system parameter to cut down the redundant need to pass it in all the functions, since in a typical use case, only one SPI (SPIA) is used for it.<br>
		6. Added checkDeviceHealth function to return a overall status of the device.<br>
		
	dsaAndNco.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Removed redundant functions related to older device version.<br>
		3. Fixed data types of parameters if function: updateTxGain<br>
		4. Removed redundant writes in functions.<br>
		5. Changed the function input definition of updateTxNco, updateRxNco and updateFbNco in FCW mode from KHz to FCW word. This is done to give finer control of frequency preventing rounding errors which is expected in FCW mode.<br>
		6. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		7. Fixed bugs in readTxNco.<br>
		
	hMacro.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Deleted redundant function: doPrepareTune<br>
		3. Added TX Tone Gen function<br>
		4. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		
	jesd.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Added function getAllLaneReady which is useful for setting the RBD.<br>
		3. Fixed dacJesdConstantTestPatternValue function for higher DAC interface rates.<br>
		4. Removed redundant writes in functions.<br>
		5. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		6. Added functions jesdRxFullResetToggle, jesdTxFullResetToggle, jesdRxClearDataPath and jesdTxClearDataPath. Cleaned up adcDacSync by calling these sub functions.<br>
		
	pap.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		
	serDes.c:<br>
		1. Added documentation and improved the parameter validity checks.<br>
		2. Updated function definition of: getSerdesEye.<br>
		3. Changed the C macros for all the spi wrapper and executeMacro function calls to AFE_FUNC_EXEC from AFE_SPI_EXEC.<br>
		4. Added functions resetSerDesDfeLane, reAdaptSerDesLane, resetSerDesDfeAllLanes and reAdaptSerDesAllLanes.<br>
		
