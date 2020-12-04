

### Need to do below code to assign the SPIB1 GPIOs, if not done already

AFE.systemParams.gpioMapping["G12"]="SPIB1_SDO"
AFE.systemParams.gpioMapping["H11"]="INTBIPI_SPIB1_SDI"
AFE.systemParams.gpioMapping["H16"]="SPIB1_CSN"
AFE.systemParams.gpioMapping["G16"]="SPIB1_CLK"
AFE.IOWRAP.configureAllGpio()
device.currentPageSelected.setValue(0)


### Load the Wrapper
import resourceFiles.mAfeCTestWrapper
reload(resourceFiles.mAfeCTestWrapper)
from resourceFiles.mAfeCTestWrapper import afeCTestWrapper
CAFE=afeCTestWrapper(device,usbHandleName+" B",lmkregProg,"libCAFE.dll",AFE)

####### Log File
CAFE.fileName="CTest.txt"

## To overwrite the old file:
CAFE.rewriteFile=1

#### Levels of Error Logging
CAFE.setAfeLogLvl(4)

#AFE_LOG_LEVEL_ERROR 0   /* error conditions */
#AFE_LOG_LEVEL_WARNING 1 /*warning conditions */
#AFE_LOG_LEVEL_INFO 2    /* informational */
#AFE_LOG_LEVEL_SPILOG 3  /*SPI-level messages */
#AFE_LOG_LEVEL_DEBUG 4   /*debug-level messages */


### Function Call Examples

CAFE.overrideTdd(0,15,3,15,1)
CAFE.updateRxNco(0,0,3520000,0,0)
info(CAFE.readRxNco(0,0,0,0))

##### Function to update the System Params according to the current AFE system Params.
CAFE.updateCSystemParams()

info(CAFE.readTxPower(0,3,0xFF))

CAFE.dacJesdConstantTestPatternValue(0,0,1,0,1,16384,16384)
CAFE.dacJesdConstantTestPatternValue(0,0,1,0,0,16384,16384)
CAFE.dacJesdConstantTestPatternValue(0,0,1,0,1,8192,8192)
CAFE.dacJesdConstantTestPatternValue(0,0,1,0,0,8192,8192)

CAFE.readTxNco(0,0,0,0)
CAFE.readTxNco(0,0,1,0)
CAFE.readTxNco(0,1,0,0)
CAFE.readTxNco(0,1,1,0)
CAFE.readTxNco(0,2,0,0)
CAFE.readTxNco(0,2,1,0)
CAFE.readTxNco(0,3,0,0)
CAFE.readTxNco(0,3,1,0)

CAFE.getFbRmsPower(0,0)


