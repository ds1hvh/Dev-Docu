
#ifndef HAFEPARAMETERS_H
#define HAFEPARAMETERS_H

/** @struct afeSystemParamsStruct
 *  @brief This structure contains the System Parameters used in the intialization script of the AFE.<br>
 * 		Some of the system parameters, which are static for a use case, like sampling and interface rates, are captured in this structure, systemParams. This is to prevent passing these redundantly for related functions. For some variables this may act as a state variable to capture current state.<br>
 * 		An array of structures of size NUM_OF_AFE, one per each AFE, should be defined in /Afe79xxUser/Src/afeParameters.c similar to the sample provided.<br>
 * 		This can be generated for each AFE configuration by running AFE.saveCAfeParamsFile() in Latte after generating the initial configuration.<br>
 * 		<b> Version 2.1:</b> <br>
 * 		1. Added documentation.
 */

struct afeSystemParamsStruct
{
	/// Multiplier Constant
	uint32_t X;
	/// Number of TX NCOs
	uint8_t numTxNCO;
	/// Number of RX NCOs
	uint8_t numRxNCO;
	/// Number of FB NCOs
	uint8_t numFbNCO;
	/// Input Reference Clock (MHz)
	float FRef;
	/// RX ADC Sampling clock. (MHz)
	float FadcRx;
	/// FB ADC Sampling clock. (MHz)
	float FadcFb;
	/// DAC Sampling clock. (MHz)
	float Fdac;

	/// When this is 0, the Pin based Sysref will be used by the AFE. When this is set to 1, AFE uses internal Sysref override in AFE and pin sysref is not used. This can be used in cases where there is no need for deterministic latency or phase consistency.<br>
	uint8_t useSpiSysref;

	/// NCO Frequency Mode. 0- FCW mode. 1-1KHz mode
	uint8_t ncoFreqMode;

	/// Enabling Half Rate Mode for RX [AB,CD]. This will make the sampling rate half of FadcRx.
	uint8_t halfRateModeRx[2];
	/// Enabling Half Rate Mode for FB [AB,CD]. This will make the sampling rate half of FadcFb.
	uint8_t halfRateModeFb[2];
	/// Enabling Half Rate Mode for TX [AB,CD]. This will make the sampling rate half of Fdac.
	uint8_t halfRateModeTx[2];

	/// 0-	Software JESD Sync 1-	Hardware JESD Sync loopback
	uint8_t syncLoopBack;

	/// DDC decimation factor for RX [A,B,C,D]. FadcRx/ddcFactorRx for the channel will be output data rate.
	uint8_t ddcFactorRx[4];
	/// RX NCO Frequencies in MHz. rxNco[NCO Number][Channel Number][Band Number]
	float rxNco[2][4][2];
	/// Number of bands per channel for RX A, B, C, D. 0-Single Band. 1-Dual Band
	uint8_t numBandsRx[4];

	/// DDC decimation factor for FB [AB, CD]. FadcFb/ddcFactorFb for the channel will be output data rate.
	uint8_t ddcFactorFb[2]; // DDC decimation factor for FB AB and FB CD;
	/// FB NCO Frequencies in MHz. fbNco[Channel Number][NCO Number]
	float fbNco[4][2];

	/// DUC interpolation factor for TX [A,B, C, D]. Fdac/ducFactorTx for the channel will be output data rate.
	uint8_t ducFactorTx[4];
	/// TX NCO Frequencies in MHz. txNco[NCO Number][Channel Number][Band Number]
	float txNco[2][4][2];

	/// Number of bands per channel for TX A, B, C, D. 0-Single Band. 1-Dual Band
	uint8_t numBandsTx[4]; // Number of bands for TX AB and TX CD. SingleBand/DualBand
	/// Operates DAC in interleaved mode when this is set to 1.
	uint8_t enableDacInterleavedMode;
	/** Sets the Mux mode for the Pin based FB DSA control.<br>
		0 -Single Fb Mode FB AB. Only FBAB DSA should be controlled by pins.<br>
		1 -Single Fb Mode FB CD. Only FBCD DSA should be controlled by pins.<br>
		2- Dual Fb_Mode. LSB 2 pins Control to FBAB DSA and MSB 2 pins control FBCD DSA.
	*/
	uint8_t txToFbMode;
	/// Chip ID of the Device
	uint32_t chipId;
	/// Chip Version of the device
	uint8_t chipVersion;

	/**  Mode of operation of the AGC. <br>
	 * 	0- disabled	<br>
		1- Internal AGC<br>
		2- External AGC SPI control<br>
		3- External AGC 3-Pin control<br>
		4- External AGC 8-Pin control<br>
	*/
	uint8_t agcMode;


	/// Per RX channel Big Step Attack Detector
	uint8_t bigStepAttkEn[4];
	/// Per RX channel Small Step Attack Detector
	uint8_t smallStepAttkEn[4];
	/// Per RX channel Power Attack Detector
	uint8_t powerAttkEn[4];
	/// Per RX channel Big Step Decay Detector
	uint8_t bigStepDecEn[4];
	/// Per RX channel Small Step Decay Detector
	uint8_t smallStepDecEn[4];
	/// Per RX channel Power Decay Detector
	uint8_t powerDecEn[4];
	/// Per RX channel Big Step Attack Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t bigStepAttkThresh[4];
	/// Per RX channel Small Step Attack Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t smallStepAttkThresh[4];
	/// Per RX channel Power Attack Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t powerAttkThresh[4];
	/// Per RX channel Big Step Decay Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t bigStepDecThresh[4];
	/// Per RX channel Small Step Decay Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t smallStepDecThresh[4];
	/// Per RX channel Power Decay Threshold. <br>. This -threshValue/4 is the threshold in dbfs set. That is, to set the a threshold of -2.25dbfs, this value should be 2.25*4=9.
	uint8_t powerDecThresh[4];
	/// ADC Digital Detector Window Length configuration per RX channel. Window length for the detectors is the corresponding WinLen value *10ns. bigStepAttkWinLen is the window length of all big step detectors.
	uint32_t bigStepAttkWinLen[4];
	/// miscStepAttkWinLen is Window Length configuration per RX channel used for all other attack detectors. Window length for the detectors is the corresponding WinLen value *10ns.
	uint32_t miscStepAttkWinLen[4];
	/// decayWinLen is Window Length configuration per RX channel common for all the decay detectors. Window length for the detectors is the corresponding WinLen value *10ns.
	uint32_t decayWinLen[4];
	/// JESD Protocol. JESD Protocol for ADC/DAC JESD instance [0, 1]. 0- 204B. <br> 2- 204C 64/66. <br> 3- 204B 64/80
	uint8_t jesdProtocol;
	/// SPI used to access PLL Pages. <br>1- SPIA.<br>2-SPIB
	uint8_t spiInUseForPllAccess;
};

uint8_t getSystemParam(uint32_t afeId, struct afeSystemParamsStruct *pstParam);

extern struct afeSystemParamsStruct systemParams[NUM_OF_AFE];
#endif
