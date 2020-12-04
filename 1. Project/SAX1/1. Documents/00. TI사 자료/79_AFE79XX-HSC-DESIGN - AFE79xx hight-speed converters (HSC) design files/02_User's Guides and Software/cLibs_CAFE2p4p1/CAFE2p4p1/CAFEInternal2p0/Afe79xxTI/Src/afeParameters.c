
#include <stdint.h>
#include <string.h>

#include "afe79xxTypes.h"
#include "afe79xxLog.h"


#include "afeCommonMacros.h"

#include "baseFunc.h"
#include "afeParameters.h"


struct afeSystemParamsStruct systemParams[NUM_OF_AFE] =
{
    {
        .X		        = 61440,
        .numTxNCO 		= 1,
        .numRxNCO 		= 1,
        .numFbNCO 		= 1,
        .FRef	        = 491.520,
        .FadcRx	        = 2949.120,
        .FadcFb	        = 2949.120,
        .Fdac	        = 2949.120 * 3,
        .useSpiSysref	= 0,
        .ncoFreqMode	= 1,
        .halfRateModeRx	= { 0, 0 },
        .halfRateModeFb	= { 0, 0 },
        .halfRateModeTx	= { 0, 0 },
        .syncLoopBack   = 1,
        .ddcFactorRx	= { 8, 8, 8, 8 },
        .rxNco	= 
        {
            {
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 }
            },
            {
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 },
                { 1800.0, 2600.0 }
            }
        },
        .numBandsRx	 = { 0, 0, 0, 0 },
        .ddcFactorFb = { 4, 4 },
        .fbNco	= 
        {
            { 1800.0, 1900.0 },
            { 1800.0, 1900.0 },
            { 1800.0, 1900.0 },
            { 1800.0, 1900.0 }
        },
        .ducFactorTx = { 24, 24, 24, 24 },
        .txNco	=
        {
            {
                { 180.0, 2600.0 },
                { 180.0, 2600.0 }, 
                { 180.0, 2600.0 },
                { 180.0, 2600.0 }
            },
            {
                {1800.0,2600.0},
                {1800.0,2600.0},
                {1800.0,2600.0},
                {1800.0,2600.0}
            }
        },
        .numBandsTx	= { 0, 0, 0, 0 },
        .enableDacInterleavedMode   = 0,
        .txToFbMode		            = 0,
        .chipId		                = 0x79,
        .chipVersion		        = 0x13,
        .agcMode		            = 0,
        .bigStepAttkEn       = { 0, 0, 0, 0 },
        .smallStepAttkEn     = { 0, 0, 0, 0 },
        .powerAttkEn         = { 0, 0, 0, 0 },
        .bigStepDecEn        = { 0, 0, 0, 0 },
        .smallStepDecEn	     = { 0, 0, 0, 0 },
        .powerDecEn	         = { 0, 0, 0, 0 },
        .bigStepAttkThresh   = { 4, 4, 4, 4 },
        .smallStepAttkThresh = { 12, 12, 12, 12 },
        .powerAttkThresh     = { 12, 12, 12, 12 },
        .bigStepDecThresh    = { 24, 24, 24, 24 },
        .smallStepDecThresh	 = { 32, 32, 32, 32 },
        .powerDecThresh	     = { 32, 32, 32, 32 },
        .bigStepAttkWinLen   = { 170, 170, 170, 170 },
        .miscStepAttkWinLen	 = { 170, 170, 170, 170 },
        .decayWinLen         = { 87380, 87380, 87380, 87380 },
        .jesdProtocol = 2,
         .spiInUseForPllAccess=1
    }
};
#include "paramsSetterGetter.h"
