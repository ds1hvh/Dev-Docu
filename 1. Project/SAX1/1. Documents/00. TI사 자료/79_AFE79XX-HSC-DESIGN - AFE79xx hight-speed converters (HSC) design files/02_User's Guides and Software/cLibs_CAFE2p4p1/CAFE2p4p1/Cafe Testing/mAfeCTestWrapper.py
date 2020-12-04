try:
	import globalDefs as Globals
except:
	pass
from ctypes import *
import d2xx
from functools import wraps
import numpy
import matplotlib.pyplot

def cWrapperFuncDecorator(func):
	@wraps(func)
	def inDecorator(*args,**kwargs):
		CAFE=args[0]
			
		if CAFE.cafeInterfaceOpen==False:
			CAFE.startFuncCall()
			reopeningTheInterface=True
		else:
			reopeningTheInterface=False
		if CAFE.rewriteFile==1:
			CAFE.startLogFile()
			CAFE.rewriteFile=0
		else:
			CAFE.appendLogFile()
		ret=func(*args,**kwargs)
		CAFE.closeLogFile()
		functionRunPass=True
		if func.__name__ not in ("writeReg", "readReg", "setAfeLogLvl", "getAfeLogLvl"):
			if type(ret) in (list,tuple):
				if ret[0]!=0:
					Globals.error('Function Execution Failed: '+ func.__name__)
					functionRunPass=False
				ret=ret[1:]
			elif ret!=0:
				Globals.error('Function Execution Failed: '+ func.__name__)
				functionRunPass=False
			#if functionRunPass=True and CAFE.closeAllPagesRunOnFailCount==0:
			
		if reopeningTheInterface==True:
			CAFE.endFuncCall()
		return ret
	#inDecorator
	return inDecorator
#funcDecorator


class afeCTestWrapper():
	def __init__(self,device,cUsbHandleName,additionalQPortHandle=None,cafeDllName="libCAFE.dll",AFE=None):
		self.device=device
		self.cafeInterface=CDLL(r"libinterface.dll")
		self.cafeInterfaceOpen=False
		self.cafe=CDLL(cafeDllName)
		self.cUsbHandleName=cUsbHandleName
		self.additionalQPortHandle=additionalQPortHandle
		self.fileName="Afe79xx_Clog.txt"
		self.rewriteFile=1
		self.AFE=AFE
		self.closeAllPagesRunOnFailCount=0
		#__init__
	
	def startFuncCall(self):
		if self.device not in (None,""):
			if self.device.currentPageSelected!=None:
				self.device.currentPageSelected.setValue(0)
		if self.additionalQPortHandle not in (None,""):
			self.additionalQPortHandle._controller.instrument.close()
		self.cafeInterface.ftdi_open(self.cUsbHandleName)
		self.cafeInterfaceOpen=True
		print "Opened the USB FTDI Handle."
	#startFuncCall
	
	def endFuncCall(self):
		self.cafeInterface.ftdi_close()
		self.cafeInterfaceOpen=False
		print "Closing the USB FTDI Handle."

		if self.additionalQPortHandle not in (None,""):
			try:
				self.additionalQPortHandle._controller.reopen()
			except:
				self.additionalQPortHandle._controller.instrument=d2xx.openEx(self.cUsbHandleName, d2xx.OPEN_BY_DESCRIPTION)

	@cWrapperFuncDecorator
	def writeReg(self,addr,val):
		self.cafeInterface.ftdi_writeReg(c_int(addr),c_int(val))

	@cWrapperFuncDecorator
	def readReg(self,addr):
		val=self.cafeInterface.ftdi_readReg(c_int(0x8000|addr))
		return val


	def startLogFile(self):
		self.cafe.openLogFile(self.fileName, "w")

	def appendLogFile(self):
		self.cafe.openLogFile(self.fileName, "a")

	def closeLogFile(self):
		self.cafe.closeLogFile()

	@cWrapperFuncDecorator
	def configAfeFromFile(self, afeId, logFormat, fileName, breakAtPollFail, breakAtReadCheckFail):
		returnVar = self.cafe.configAfeFromFile(afeId, logFormat, fileName, breakAtPollFail, breakAtReadCheckFail)
		return returnVar
		
	@cWrapperFuncDecorator
	def getSystemParam(self,afeId):
		returnVar = self.cafe.getSystemParam(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def agcStateControlConfig(self,afeId, chNo, agcstate):
		returnVar = self.cafe.agcStateControlConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(agcstate)))
		return returnVar

	@cWrapperFuncDecorator
	def agcDigDetConfig(self,afeId, chNo, bigStepAttkEn, smallStepAttkEn, bigStepDecEn, smallStepDecEn, powerAttkEn, powerDecEn, bigStepAttkThresh, smallStepAttkThresh, bigStepDecThresh, smallStepDecThresh, powerAttkThresh, powerDecThresh):
		returnVar = self.cafe.agcDigDetConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bigStepAttkEn)), c_uint(int(smallStepAttkEn)), c_uint(int(bigStepDecEn)), c_uint(int(smallStepDecEn)), c_uint(int(powerAttkEn)), c_uint(int(powerDecEn)), c_uint(int(bigStepAttkThresh)), c_uint(int(smallStepAttkThresh)), c_uint(int(bigStepDecThresh)), c_uint(int(smallStepDecThresh)), c_uint(int(powerAttkThresh)), c_uint(int(powerDecThresh)))
		return returnVar

	@cWrapperFuncDecorator
	def agcDigDetTimeConstantConfig(self,afeId, chNo, bigStepAttkWinLen, miscStepAttkWinLen, decayWinLen):
		returnVar = self.cafe.agcDigDetTimeConstantConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bigStepAttkWinLen)), c_uint(int(miscStepAttkWinLen)), c_uint(int(decayWinLen)))
		return returnVar

	@cWrapperFuncDecorator
	def agcDigDetAbsoluteNumCrossingConfig(self,afeId, chNo, bigStepAttkNumHits, smallStepAttkNumHits, bigStepDecNumHits, smallStepDecNumHits):
		returnVar = self.cafe.agcDigDetAbsoluteNumCrossingConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bigStepAttkNumHits)), c_uint(int(smallStepAttkNumHits)), c_uint(int(bigStepDecNumHits)), c_uint(int(smallStepDecNumHits)))
		return returnVar

	@cWrapperFuncDecorator
	def agcDigDetRelativeNumCrossingConfig(self,afeId, chNo, bigStepAttkNumHits, smallStepAttkNumHits, bigStepDecNumHits, smallStepDecNumHits):
		returnVar = self.cafe.agcDigDetRelativeNumCrossingConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bigStepAttkNumHits)), c_uint(int(smallStepAttkNumHits)), c_uint(int(bigStepDecNumHits)), c_uint(int(smallStepDecNumHits)))
		return returnVar

	@cWrapperFuncDecorator
	def externalAgcConfig(self,afeId, chNo, pin0sel, pin1sel, pin2sel, pin3sel, pkDetPinLsbSel, pulseExpansionCount, noLsbsToSend):
		returnVar = self.cafe.externalAgcConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(pin0sel)), c_uint(int(pin1sel)), c_uint(int(pin2sel)), c_uint(int(pin3sel)), c_uint(int(pkDetPinLsbSel)), c_uint(int(pulseExpansionCount)), c_uint(int(noLsbsToSend)))
		return returnVar

	@cWrapperFuncDecorator
	def minMaxDsaAttnConfig(self,afeId, chNo, minDsaAttn, maxDsaAttn):
		returnVar = self.cafe.minMaxDsaAttnConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(minDsaAttn)), c_uint(int(maxDsaAttn)))
		return returnVar

	@cWrapperFuncDecorator
	def agcGainStepSizeConfig(self,afeId, chNo, bigStepAttkStepSize, smallStepAttkStepSize, bigStepDecayStepSize, smallStepDecayStepSize):
		returnVar = self.cafe.agcGainStepSizeConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bigStepAttkStepSize)), c_uint(int(smallStepAttkStepSize)), c_uint(int(bigStepDecayStepSize)), c_uint(int(smallStepDecayStepSize)))
		return returnVar

	@cWrapperFuncDecorator
	def internalAgcConfig(self,afeId, chNo, tdd_freeze_agc, blank_time_extcomp, en_agcfreeze_pin, extCompControlEn):
		returnVar = self.cafe.internalAgcConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(tdd_freeze_agc)), c_uint(int(blank_time_extcomp)), c_uint(int(en_agcfreeze_pin)), c_uint(int(extCompControlEn)))
		return returnVar

	@cWrapperFuncDecorator
	def rfAnalogDetConfig(self,afeId, chNo, rfdeten, rfDetMode, rfDetNumHitsMode, rfdetnumhits, rfdetThreshold, rfdetstepsize):
		returnVar = self.cafe.rfAnalogDetConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(rfdeten)), c_uint(int(rfDetMode)), c_uint(int(rfDetNumHitsMode)), c_uint(int(rfdetnumhits)), c_uint(int(rfdetThreshold)), c_uint(int(rfdetstepsize)))
		return returnVar

	@cWrapperFuncDecorator
	def extLnaConfig(self,afeId, chNo, singleDualBandMode, lnaGainMargin, enBandDet, tapOffPoint):
		returnVar = self.cafe.extLnaConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(singleDualBandMode)), c_uint(int(lnaGainMargin)), c_uint(int(enBandDet)), c_uint(int(tapOffPoint)))
		return returnVar

	@cWrapperFuncDecorator
	def extLnaGainConfig(self,afeId, chNo, lnaGainB0, lnaPhaseB0, lnaGainB1, lnaPhaseB1):
		returnVar = self.cafe.extLnaGainConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(lnaGainB0)), c_uint(int(lnaPhaseB0)), c_uint(int(lnaGainB1)), c_uint(int(lnaPhaseB1)))
		return returnVar

	@cWrapperFuncDecorator
	def alcConfig(self,afeId, chNo, alcMode, totalGainRange, minAttnAlc, useMinAttnAgc):
		returnVar = self.cafe.alcConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(alcMode)), c_uint(int(totalGainRange)), c_uint(int(minAttnAlc)), c_uint(int(useMinAttnAgc)))
		return returnVar

	@cWrapperFuncDecorator
	def fltPtConfig(self,afeId, chNo, fltPtMode, fltPtFmt):
		returnVar = self.cafe.fltPtConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(fltPtMode)), c_uint(int(fltPtFmt)))
		return returnVar

	@cWrapperFuncDecorator
	def coarseFineConfig(self,afeId, chNo, stepSize, nBitIndex, indexInvert, indexSwapIQ, sigBackOff, gainChangeIndEn):
		returnVar = self.cafe.coarseFineConfig(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(stepSize)), c_uint(int(nBitIndex)), c_uint(int(indexInvert)), c_uint(int(indexSwapIQ)), c_uint(int(sigBackOff)), c_uint(int(gainChangeIndEn)))
		return returnVar

	@cWrapperFuncDecorator
	def dev_spi_write(self,afeId, addr, data):
		returnVar = self.cafe.dev_spi_write(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(data)))
		return returnVar

	@cWrapperFuncDecorator
	def dev_spi_read(self,afeId, addr):
		readVal = c_uint()
		
		returnVar = self.cafe.dev_spi_read(c_uint(int(afeId)), c_uint(int(addr)), byref(readVal))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def wait(self,wait_s):
		returnVar = self.cafe.wait(c_uint(int(wait_s)))
		return returnVar

	@cWrapperFuncDecorator
	def waitMs(self,wait_ms):
		returnVar = self.cafe.waitMs(c_uint(int(wait_ms)))
		return returnVar

	@cWrapperFuncDecorator
	def afeLogmsg(self,level):
		returnVar = self.cafe.afeLogmsg(c_uint(int(level)))
		return

	@cWrapperFuncDecorator
	def setAfeLogLvl(self,level):
		returnVar = self.cafe.setAfeLogLvl(c_uint(int(level)))
		return

	@cWrapperFuncDecorator
	def getAfeLogLvl(self,):
		returnVar = self.cafe.getAfeLogLvl()
		return returnVar

	@cWrapperFuncDecorator
	def giveSingleSysrefPulse(self,afeId):
		returnVar = self.cafe.giveSingleSysrefPulse(c_uint(int(afeId)))
		return returnVar


	@cWrapperFuncDecorator
	def giveAfeAdcInput(self,afeId, rxChNo, bandNo):
		returnVar = self.cafe.giveAfeAdcInput(c_uint(int(afeId)), c_uint(int(rxChNo)), c_uint(int(bandNo)))
		return returnVar

	@cWrapperFuncDecorator
	def connectAfeTxToFb(self,afeId, txChNo, fbChNo, bandNo):
		returnVar = self.cafe.connectAfeTxToFb(c_uint(int(afeId)), c_uint(int(txChNo)), c_uint(int(fbChNo)), c_uint(int(bandNo)))
		return returnVar

	@cWrapperFuncDecorator
	def serdesRawRead(self,afeId, addr):
		readVal = c_uint()
		
		returnVar = self.cafe.serdesRawRead(c_uint(int(afeId)), c_uint(int(addr)), byref(readVal))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def serdesRawWrite(self,afeId, addr, data):
		returnVar = self.cafe.serdesRawWrite(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(data)))
		return returnVar

	@cWrapperFuncDecorator
	def afeSpiWriteWrapper(self,afeId, addr, data, lsb, msb):
		returnVar = self.cafe.afeSpiWriteWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(data)), c_uint(int(lsb)), c_uint(int(msb)))
		return returnVar

	@cWrapperFuncDecorator
	def afeSpiReadWrapper(self,afeId, addr, lsb, msb):
		readVal = c_uint()
		
		returnVar = self.cafe.afeSpiReadWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(lsb)), c_uint(int(msb)), byref(readVal))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def serdesWriteWrapper(self,afeId, addr, data, lsb, msb):
		returnVar = self.cafe.serdesWriteWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(data)), c_uint(int(lsb)), c_uint(int(msb)))
		return returnVar

	@cWrapperFuncDecorator
	def serdesReadWrapper(self,afeId, addr, lsb, msb):
		readVal = c_uint()
		
		returnVar = self.cafe.serdesReadWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(lsb)), c_uint(int(msb)), byref(readVal))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def serdesLaneWriteWrapper(self,afeId, addr, laneNo, data, lsb, msb):
		returnVar = self.cafe.serdesLaneWriteWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(laneNo)), c_uint(int(data)), c_uint(int(lsb)), c_uint(int(msb)))
		return returnVar

	@cWrapperFuncDecorator
	def serdesLaneReadWrapper(self,afeId, addr, laneNo, lsb, msb):
		readVal = c_uint()
		
		returnVar = self.cafe.serdesLaneReadWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(laneNo)), c_uint(int(lsb)), c_uint(int(msb)), byref(readVal))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def afeSpiCheckWrapper(self,afeId, addr, lsb, msb, data):
		pbSame = c_uint()
		
		returnVar = self.cafe.afeSpiCheckWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(lsb)), c_uint(int(msb)), c_uint(int(data)), byref(pbSame))
		return returnVar, pbSame.value

	@cWrapperFuncDecorator
	def afeSpiPollWrapper(self,afeId, addr, expectedData, lsb, msb):
		returnVar = self.cafe.afeSpiPollWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(expectedData)), c_uint(int(lsb)), c_uint(int(msb)))
		return returnVar

	@cWrapperFuncDecorator
	def afeSpiPollLogWrapper(self,afeId, addr, lsb, msb, expectedData):
		returnVar = self.cafe.afeSpiPollLogWrapper(c_uint(int(afeId)), c_uint(int(addr)), c_uint(int(lsb)), c_uint(int(msb)), c_uint(int(expectedData)))
		return returnVar

	@cWrapperFuncDecorator
	def requestPllSpiAccess(self,afeId, regType):
		returnVar = self.cafe.requestPllSpiAccess(c_uint(int(afeId)), c_uint(int(regType)))
		return returnVar

	@cWrapperFuncDecorator
	def readTopMem(self,afeId, addr, noBytes):
		readVal = c_uint()
		
		returnVar = self.cafe.readTopMem(c_uint(int(afeId)), c_uint(int(addr)), byref(readVal), c_uint(int(noBytes)))
		return returnVar, readVal.value

	@cWrapperFuncDecorator
	def closeAllPages(self,afeId):
		returnVar = self.cafe.closeAllPages(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def doRxDsaCalib(self,afeId, rxChainForCalib, fbChainForCalib, useTxForCalib, rxDsaBandCalibMode):
		readPacket = (c_uint8*2000)()
		readPacketSize = c_uint16()
		returnVar = self.cafe.doRxDsaCalib(c_uint(int(afeId)), c_uint(int(rxChainForCalib)), c_uint(int(fbChainForCalib)), c_uint(int(useTxForCalib)), c_uint(int(rxDsaBandCalibMode)), readPacket, byref(readPacketSize))
		return returnVar, list(readPacket), readPacketSize.value
		
	@cWrapperFuncDecorator
	def doTxDsaCalib(self,afeId, txChainForCalib, txDsaCalibMode, txDsaBandCalibMode):
		readPacket = (c_uint8*2000)()
		readPacketSize = c_uint16()
		returnVar = self.cafe.doTxDsaCalib(c_uint(int(afeId)), c_uint(int(txChainForCalib)), c_uint(int(txDsaCalibMode)), c_uint(int(txDsaBandCalibMode)), readPacket, byref(readPacketSize))
		return returnVar, list(readPacket), readPacketSize.value


	@cWrapperFuncDecorator
	def loadTxDsaPacket(self,afeId, packetArray, arraySize):
		arraySize=len(packetArray)
		array = (c_uint8*arraySize)()
		for i in range(arraySize):
			array[i] = packetArray[i]
		returnVar = self.cafe.loadTxDsaPacket(c_uint(int(afeId)), array, c_uint(int(arraySize)))
		return returnVar

	@cWrapperFuncDecorator
	def loadRxDsaPacket(self,afeId, packetArray, arraySize):
		array = (c_uint8*arraySize)()
		for i in range(arraySize):
			array[i] = c_uint8(list[i])
		returnVar = self.cafe.loadRxDsaPacket(c_uint(int(afeId)), cast(array, POINTER(c_uint8)), c_uint(int(arraySize)))
		return returnVar


	@cWrapperFuncDecorator
	def getChipVersion(self,afeId):
		returnVar = self.cafe.getChipVersion(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def checkSysref(self,afeId, clearSysrefFlag):
		sysrefReceived = c_uint()
		
		returnVar = self.cafe.checkSysref(c_uint(int(afeId)), c_uint(int(clearSysrefFlag)), byref(sysrefReceived))
		return returnVar, sysrefReceived.value
	
	@cWrapperFuncDecorator
	def sendSysref(self,afeId, spiSysref, getSpiAccess):
		returnVar = self.cafe.sendSysref(c_uint(int(afeId)), c_uint(int(spiSysref)), c_uint(int(getSpiAccess)))
		return returnVar

	@cWrapperFuncDecorator
	def overrideTdd(self,afeId, rx, fb, tx, enableOverride):
		returnVar = self.cafe.overrideTdd(c_uint(int(afeId)), c_uint(int(rx)), c_uint(int(fb)), c_uint(int(tx)), c_uint(int(enableOverride)))
		return returnVar

	@cWrapperFuncDecorator
	def overrideTddPins(self,afeId, rx, fb, tx):
		returnVar = self.cafe.overrideTddPins(c_uint(int(afeId)), c_uint(int(rx)), c_uint(int(fb)), c_uint(int(tx)))
		return returnVar

	@cWrapperFuncDecorator
	def checkPllLockStatus(self,afeId):
		pllLockStatus = c_uint()
		
		returnVar = self.cafe.checkPllLockStatus(c_uint(int(afeId)), byref(pllLockStatus))
		return returnVar, pllLockStatus.value

	@cWrapperFuncDecorator
	def clearPllStickyLockStatus(self,afeId):
		returnVar = self.cafe.clearPllStickyLockStatus(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def readAlarmPinStatus(self,afeId, alarmNo):
		status = c_uint()
		
		returnVar = self.cafe.readAlarmPinStatus(c_uint(int(afeId)), c_uint(int(alarmNo)), byref(status))
		return returnVar, status.value

	@cWrapperFuncDecorator
	def clearSpiAlarms(self,afeId):
		returnVar = self.cafe.clearSpiAlarms(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def readSpiAlarms(self,afeId):
		alarmStatus = c_uint()
		
		returnVar = self.cafe.readSpiAlarms(c_uint(int(afeId)), byref(alarmStatus))
		return returnVar, alarmStatus.value

	@cWrapperFuncDecorator
	def readTxPower(self,afeId, chNo, windowLen):
		powerReadB0 = c_double()
		powerReadB1 = c_double()
		combinedRead = c_double()
		
		returnVar = self.cafe.readTxPower(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(windowLen)), byref(powerReadB0), byref(powerReadB1), byref(combinedRead))
		return returnVar, powerReadB0.value, powerReadB1.value, combinedRead.value

	@cWrapperFuncDecorator
	def getRxRmsPower(self,afeId, chNo):
		avg_pwrdb = c_double()
		
		returnVar = self.cafe.getRxRmsPower(c_uint(int(afeId)), c_uint(int(chNo)), byref(avg_pwrdb))
		return returnVar, avg_pwrdb.value
	
	@cWrapperFuncDecorator
	def getFbRmsPower(self,afeId, chNo):
		avg_pwrdb = c_double()
		
		returnVar = self.cafe.getFbRmsPower(c_uint(int(afeId)), c_uint(int(chNo)), byref(avg_pwrdb))
		return returnVar, avg_pwrdb.value

	@cWrapperFuncDecorator
	def clearAllAlarms(self,afeId):
		returnVar = self.cafe.clearAllAlarms(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def overrideAlarmPin(self,afeId, alarmNo, overrideSel, overrideVal):
		returnVar = self.cafe.overrideAlarmPin(c_uint(int(afeId)), c_uint(int(alarmNo)), c_uint(int(overrideSel)), c_uint(int(overrideVal)))
		return returnVar

	@cWrapperFuncDecorator
	def overrideRelDetPin(self,afeId, chNo, overrideSel, overrideVal):
		returnVar = self.cafe.overrideRelDetPin(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(overrideSel)), c_uint(int(overrideVal)))
		return returnVar

	@cWrapperFuncDecorator
	def overrideDigPkDetPin(self,afeId, chNo, pinNo, overrideSel, overrideVal):
		returnVar = self.cafe.overrideDigPkDetPin(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(pinNo)), c_uint(int(overrideSel)), c_uint(int(overrideVal)))
		return returnVar

	@cWrapperFuncDecorator
	def checkDeviceHealth(self,afeId):
		allOk = c_uint()
		
		returnVar = self.cafe.checkDeviceHealth(c_uint(int(afeId)), byref(allOk))
		return returnVar, allOk.value
		
	@cWrapperFuncDecorator
	def setTxDsa(self,afeId, chNo, dsaSetting):
		returnVar = self.cafe.setTxDsa(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(dsaSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def setFbDsa(self,afeId, chNo, dsaSetting):
		returnVar = self.cafe.setFbDsa(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(dsaSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def setRxDsa(self,afeId, chNo, dsaSetting):
		returnVar = self.cafe.setRxDsa(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(dsaSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def setRxDigGain(self,afeId, chNo, bandNo, dsaSetting):
		returnVar = self.cafe.setRxDigGain(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bandNo)), c_uint(int(dsaSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def setRxDsaMode(self,afeId, topNo, mode):
		returnVar = self.cafe.setRxDsaMode(c_uint(int(afeId)), c_uint(int(topNo)), c_uint(int(mode)))
		return returnVar

	@cWrapperFuncDecorator
	def setPinRxDsaSettings(self,afeId, chNo, dsaInit, dsaStep, maxDelay):
		returnVar = self.cafe.setPinRxDsaSettings(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(dsaInit)), c_uint(int(dsaStep)), c_uint(int(maxDelay)))
		return returnVar

	@cWrapperFuncDecorator
	def setTxDigGain(self,afeId, chNo, bandNo, dig_gain):
		returnVar = self.cafe.setTxDigGain(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(bandNo)), c_int(dig_gain))
		return returnVar

	@cWrapperFuncDecorator
	def txDsaIdxGainSwap(self,afeId, chNo, anaAttn0, anaAttn1, digB0Gain0, digB0Gain1, digB1Gain0, digB1Gain1):
		returnVar = self.cafe.txDsaIdxGainSwap(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(anaAttn0)), c_uint(int(anaAttn1)), c_int(digB0Gain0), c_int(digB0Gain1), c_int(digB1Gain0), c_int(digB1Gain1))
		return returnVar

	@cWrapperFuncDecorator
	def updateTxGainParam(self,afeId, mode, transitTime, maxAnaDsa):
		returnVar = self.cafe.updateTxGainParam(c_uint(int(afeId)), c_uint(int(mode)), c_uint(int(transitTime)), c_uint(int(maxAnaDsa)))
		return returnVar

	@cWrapperFuncDecorator
	def updateTxGain(self,afeId, txChainSel, gainValidity, tx0B0Dsa, tx0B1Dsa, tx1B0Dsa, tx1B1Dsa):
		returnVar = self.cafe.updateTxGain(c_uint(int(afeId)), c_uint(int(txChainSel)), c_uint(int(gainValidity)), c_uint(int(tx0B0Dsa)), c_uint(int(tx0B1Dsa)), c_uint(int(tx1B0Dsa)), c_uint(int(tx1B1Dsa)))
		return returnVar

	@cWrapperFuncDecorator
	def updateTxNco(self,afeId, chNo, mixer, nco):
		returnVar = self.cafe.updateTxNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(mixer)), c_uint(int(nco)))
		return returnVar

	@cWrapperFuncDecorator
	def updateTxNcoDb(self,afeId, chNo, nco, band0Nco0, band1Nco0, band0Nco1, band1Nco1):
		returnVar = self.cafe.updateTxNcoDb(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(nco)), c_uint(int(band0Nco0)), c_uint(int(band1Nco0)), c_uint(int(band0Nco1)), c_uint(int(band1Nco1)))
		return returnVar

	@cWrapperFuncDecorator
	def rxNCOSel(self,afeId, chno, BandId, ovr, NCOId):
		returnVar = self.cafe.rxNCOSel(c_uint(int(afeId)), c_uint(int(chno)), c_uint(int(BandId)), c_uint(int(ovr)), c_uint(int(NCOId)))
		return returnVar

	@cWrapperFuncDecorator
	def fbNCOSel(self,afeId, topno, ovr, NCOId):
		returnVar = self.cafe.fbNCOSel(c_uint(int(afeId)), c_uint(int(topno)), c_uint(int(ovr)), c_uint(int(NCOId)))
		return returnVar

	@cWrapperFuncDecorator
	def updateRxNco(self,afeId, chNo, mixer, band, nco):
		returnVar = self.cafe.updateRxNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(mixer)), c_uint(int(band)), c_uint(int(nco)))
		return returnVar

	@cWrapperFuncDecorator
	def updateFbNco(self,afeId, chNo, mixer, nco):
		returnVar = self.cafe.updateFbNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(mixer)), c_uint(int(nco)))
		return returnVar

	@cWrapperFuncDecorator
	def readRxNco(self,afeId, chNo, band, nco):
		ncoFreq = c_double()
		
		returnVar = self.cafe.readRxNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(band)), c_uint(int(nco)), byref(ncoFreq))
		return returnVar, ncoFreq.value

	@cWrapperFuncDecorator
	def readFbNco(self,afeId, chNo, nco):
		ncoFreq = c_double()
		
		returnVar = self.cafe.readFbNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(nco)), byref(ncoFreq))
		return returnVar, ncoFreq.value

	@cWrapperFuncDecorator
	def readTxNco(self,afeId, chNo, band, nco):
		val = c_int()
		
		returnVar = self.cafe.readTxNco(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(band)), c_uint(int(nco)), byref(val))
		return returnVar, val.value

	@cWrapperFuncDecorator
	def setFbDsaPerTx(self,afeId, pinNo, dsaSetting):
		returnVar = self.cafe.setFbDsaPerTx(c_uint(int(afeId)), c_uint(int(pinNo)), c_uint(int(dsaSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def fbDsaPerTxEn(self,afeId, en):
		returnVar = self.cafe.fbDsaPerTxEn(c_uint(int(afeId)), c_uint(int(en)))
		return returnVar

	@cWrapperFuncDecorator
	def splitToByte(self,val, numBytes):
		splitByteList = c_uint()
		
		returnVar = self.cafe.splitToByte(c_uint(int(val)), c_uint(int(numBytes)), byref(splitByteList))
		return returnVar, splitByteList.value

	@cWrapperFuncDecorator
	def writeOperandList(self,afeId, numOfOperands):
		operandList = c_uint()
		
		returnVar = self.cafe.writeOperandList(c_uint(int(afeId)), byref(operandList), c_uint(int(numOfOperands)))
		return returnVar, operandList.value

	@cWrapperFuncDecorator
	def readResultRegSpi(self,afeId, regNum):
		result = c_uint()
		
		returnVar = self.cafe.readResultRegSpi(c_uint(int(afeId)), c_uint(int(regNum)), byref(result))
		return returnVar, result.value

	@cWrapperFuncDecorator
	def waitForMacroReady(self,afeId):
		returnVar = self.cafe.waitForMacroReady(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def waitForMacroDone(self,afeId):
		returnVar = self.cafe.waitForMacroDone(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def waitForMacroAck(self,afeId):
		returnVar = self.cafe.waitForMacroAck(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def checkForMacroError(self,afeId):
		errorReg = c_uint()
		
		returnVar = self.cafe.checkForMacroError(c_uint(int(afeId)), byref(errorReg))
		return returnVar, errorReg.value

	@cWrapperFuncDecorator
	def executeMacro(self,afeId, numOfOperands, opcode):
		byteList = c_uint()
		
		returnVar = self.cafe.executeMacro(c_uint(int(afeId)), byref(byteList), c_uint(int(numOfOperands)), c_uint(int(opcode)))
		return returnVar, byteList.value

	@cWrapperFuncDecorator
	def triggerMacro(self,afeId, opcode):
		returnVar = self.cafe.triggerMacro(c_uint(int(afeId)), c_uint(int(opcode)))
		return returnVar

	@cWrapperFuncDecorator
	def enableMemAccess(self,afeId, en):
		returnVar = self.cafe.enableMemAccess(c_uint(int(afeId)), c_uint(int(en)))
		return returnVar

	@cWrapperFuncDecorator
	def doSystemTuneSelective(self,afeId, rxChList, fbChList, txChList, sectionEnable):
		returnVar = self.cafe.doSystemTuneSelective(c_uint(int(afeId)), c_uint(int(rxChList)), c_uint(int(fbChList)), c_uint(int(txChList)), c_uint(int(sectionEnable)))
		return returnVar

	@cWrapperFuncDecorator
	def updateSystemTxChannelFreqConfig(self,afeId, txChList, listNCO, txNCO, immUpdt, reload):
		returnVar = self.cafe.updateSystemTxChannelFreqConfig(c_uint(int(afeId)), c_uint(int(txChList)), c_uint(int(listNCO)), c_uint(int(txNCO)), c_uint(int(immUpdt)), c_uint(int(reload)))
		return returnVar

	@cWrapperFuncDecorator
	def checkMcuHealth(self,afeId):
		healthOk = c_uint()
		
		returnVar = self.cafe.checkMcuHealth(c_uint(int(afeId)), byref(healthOk))
		return returnVar, healthOk.value

	@cWrapperFuncDecorator
	def txCalibSiggen(self,afeId, chNo, configOption, freq0, freq0Amp):
		returnVar = self.cafe.txCalibSiggen(c_uint(int(afeId)), c_uint(int(chNo)), c_uint(int(configOption)), c_uint(int(freq0)), c_uint(int(freq0Amp)))
		return returnVar
	
	@cWrapperFuncDecorator
	def dacJesdSendData(self,afeId, topno):
		returnVar = self.cafe.dacJesdSendData(c_uint(int(afeId)), c_uint(int(topno)))
		return returnVar

	@cWrapperFuncDecorator
	def dacJesdConstantTestPatternValue(self,afeId, topno, enable, chNo, bandNo, valueI, valueQ):
		returnVar = self.cafe.dacJesdConstantTestPatternValue(c_uint(int(afeId)), c_uint(int(topno)), c_uint(int(enable)), c_uint(int(chNo)), c_uint(int(bandNo)), c_uint(int(valueI)), c_uint(int(valueQ)))
		return returnVar

	@cWrapperFuncDecorator
	def dacJesdSendRampTestPattern(self,afeId, topno, increment):
		returnVar = self.cafe.dacJesdSendRampTestPattern(c_uint(int(afeId)), c_uint(int(topno)), c_uint(int(increment)))
		return returnVar

	@cWrapperFuncDecorator
	def getJesdRxLaneErrors(self,afeId, laneNo):
		error = c_uint()
		
		returnVar = self.cafe.getJesdRxLaneErrors(c_uint(int(afeId)), c_uint(int(laneNo)), byref(error))
		return returnVar, error.value

	@cWrapperFuncDecorator
	def getJesdRxLaneFifoErrors(self,afeId, laneNo):
		error = c_uint()
		
		returnVar = self.cafe.getJesdRxLaneFifoErrors(c_uint(int(afeId)), c_uint(int(laneNo)), byref(error))
		return returnVar, error.value

	@cWrapperFuncDecorator
	def getJesdRxMiscSerdesErrors(self,afeId, jesdNo):
		errorValue = c_uint()
		
		returnVar = self.cafe.getJesdRxMiscSerdesErrors(c_uint(int(afeId)), c_uint(int(jesdNo)), byref(errorValue))
		return returnVar, errorValue.value

	@cWrapperFuncDecorator
	def getJesdRxAlarms(self,afeId):
		error = c_uint()
		
		returnVar = self.cafe.getJesdRxAlarms(c_uint(int(afeId)), byref(error))
		return returnVar, error.value

	@cWrapperFuncDecorator
	def getJesdRxLinkStatus(self,afeId):
		linkStatus = c_uint()
		
		returnVar = self.cafe.getJesdRxLinkStatus(c_uint(int(afeId)), byref(linkStatus))
		return returnVar, linkStatus.value
	
	@cWrapperFuncDecorator
	def getJesdRxLinkStatus204B(self,afeId):
		linkStatus = c_uint()
		
		returnVar = self.cafe.getJesdRxLinkStatus204B(c_uint(int(afeId)), byref(linkStatus))
		return returnVar, linkStatus.value

	@cWrapperFuncDecorator
	def getJesdRxLinkStatus204C(self,afeId):
		linkStatus = c_uint()
		
		returnVar = self.cafe.getJesdRxLinkStatus204C(c_uint(int(afeId)), byref(linkStatus))
		return returnVar, linkStatus.value

	@cWrapperFuncDecorator
	def clearJesdTxAlarms(self,afeId):
		returnVar = self.cafe.clearJesdTxAlarms(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def clearJesdRxAlarms(self,afeId):
		returnVar = self.cafe.clearJesdRxAlarms(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def clearJesdRxAlarmsForPap(self,afeId):
		returnVar = self.cafe.clearJesdRxAlarmsForPap(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def jesdRxClearSyncErrorCnt(self,afeId, jesdNo):
		returnVar = self.cafe.jesdRxClearSyncErrorCnt(c_uint(int(afeId)), c_uint(int(jesdNo)))
		return returnVar

	@cWrapperFuncDecorator
	def jesdRxGetSyncErrorCnt(self,afeId, jesdNo):
		linkErrorCount = c_uint()
		
		returnVar = self.cafe.jesdRxGetSyncErrorCnt(c_uint(int(afeId)), c_uint(int(jesdNo)), byref(linkErrorCount))
		return returnVar, linkErrorCount.value

	@cWrapperFuncDecorator
	def jesdTxGetSyncErrorCnt(self,afeId, jesdLaneNo):
		linkErrorCount = c_uint()
		
		returnVar = self.cafe.jesdTxGetSyncErrorCnt(c_uint(int(afeId)), c_uint(int(jesdLaneNo)), byref(linkErrorCount))
		return returnVar, linkErrorCount.value

	@cWrapperFuncDecorator
	def adcRampTestPattern(self,afeId, topno, chNo, enable, rampIncr):
		returnVar = self.cafe.adcRampTestPattern(c_uint(int(afeId)), c_uint(int(topno)), c_uint(int(chNo)), c_uint(int(enable)), c_uint(int(rampIncr)))
		return returnVar

	@cWrapperFuncDecorator
	def toggleSync(self,afeId, overrideValue):
		returnVar = self.cafe.toggleSync(c_uint(int(afeId)), c_uint(int(overrideValue)))
		return returnVar

	@cWrapperFuncDecorator
	def setJesdTxSyncOverride(self,afeId, syncNo, overrideValue, syncValue):
		returnVar = self.cafe.setJesdTxSyncOverride(c_uint(int(afeId)), c_uint(int(syncNo)), c_uint(int(overrideValue)), c_uint(int(syncValue)))
		return returnVar

	@cWrapperFuncDecorator
	def setJesdRxSyncOverride(self,afeId, syncNo, overrideValue, syncValue):
		returnVar = self.cafe.setJesdRxSyncOverride(c_uint(int(afeId)), c_uint(int(syncNo)), c_uint(int(overrideValue)), c_uint(int(syncValue)))
		return returnVar

	@cWrapperFuncDecorator
	def getJesdTxFifoErrors(self,afeId, jesdNo):
		errors = c_uint()
		
		returnVar = self.cafe.getJesdTxFifoErrors(c_uint(int(afeId)), c_uint(int(jesdNo)), byref(errors))
		return returnVar, errors.value

	@cWrapperFuncDecorator
	def jesdRxFullResetToggle(self,afeId, jesdNo):
		returnVar = self.cafe.jesdRxFullResetToggle(c_uint(int(afeId)), c_uint(int(jesdNo)))
		return returnVar

	@cWrapperFuncDecorator
	def jesdTxFullResetToggle(self,afeId, jesdNo):
		returnVar = self.cafe.jesdTxFullResetToggle(c_uint(int(afeId)), c_uint(int(jesdNo)))
		return returnVar

	@cWrapperFuncDecorator
	def adcDacSync(self,afeId, pinSysref):
		returnVar = self.cafe.adcDacSync(c_uint(int(afeId)), c_uint(int(pinSysref)))
		return returnVar

	@cWrapperFuncDecorator
	def jesdRxResetStateMachine(self,afeId, linkNo):
		returnVar = self.cafe.jesdRxResetStateMachine(c_uint(int(afeId)), c_uint(int(linkNo)))
		return returnVar

	@cWrapperFuncDecorator
	def getAllLaneReady(self,afeId, jesdNo):
		rbdOffset = c_uint()
		
		returnVar = self.cafe.getAllLaneReady(c_uint(int(afeId)), c_uint(int(jesdNo)), byref(rbdOffset))
		return returnVar, rbdOffset.value
	
	@cWrapperFuncDecorator
	def checkIfRbdIsGood(self,afeId, jesdNo):
		rbdStatus = c_uint()
		
		returnVar = self.cafe.checkIfRbdIsGood(c_uint(int(afeId)), c_uint(int(jesdNo)), byref(rbdStatus))
		return returnVar, rbdStatus.value

	@cWrapperFuncDecorator
	def setGoodRbd(self,afeId, jesdNo):
		returnVar = self.cafe.setGoodRbd(c_uint(int(afeId)), c_uint(int(jesdNo)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxLaneErrors(self,afeId, laneNo, maskValue):
		returnVar = self.cafe.maskJesdRxLaneErrors(c_uint(int(afeId)), c_uint(int(laneNo)), c_uint(int(maskValue)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxLaneFifoErrors(self,afeId, jesdNo, losMaskValue, fifoMaskValue):
		returnVar = self.cafe.maskJesdRxLaneFifoErrors(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(losMaskValue)), c_uint(int(fifoMaskValue)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxMiscSerdesErrors(self,afeId, jesdNo, maskSerdesPllLock):
		returnVar = self.cafe.maskJesdRxMiscSerdesErrors(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(maskSerdesPllLock)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdTxFifoErrors(self,afeId, jesdNo, maskValue):
		returnVar = self.cafe.maskJesdTxFifoErrors(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(maskValue)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxLaneErrorsToPap(self,afeId, laneNo, maskValue):
		returnVar = self.cafe.maskJesdRxLaneErrorsToPap(c_uint(int(afeId)), c_uint(int(laneNo)), c_uint(int(maskValue)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxLaneFifoErrorsToPap(self,afeId, jesdNo, losMaskValue, fifoMaskValue):
		returnVar = self.cafe.maskJesdRxLaneFifoErrorsToPap(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(losMaskValue)), c_uint(int(fifoMaskValue)))
		return returnVar

	@cWrapperFuncDecorator
	def maskJesdRxMiscSerdesErrorsToPap(self,afeId, jesdNo, maskSerdesPllLock):
		returnVar = self.cafe.maskJesdRxMiscSerdesErrorsToPap(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(maskSerdesPllLock)))
		return returnVar

	@cWrapperFuncDecorator
	def setManualRbd(self,afeId, jesdNo, value):
		returnVar = self.cafe.setManualRbd(c_uint(int(afeId)), c_uint(int(jesdNo)), c_uint(int(value)))
		return returnVar

	@cWrapperFuncDecorator
	def configurePapMaDet(self,afeId, chno, maEnable, maNumSample, maWindowCntr, maWindowCntrTh, maThreshB0, maThreshB1, maThreshComb):
		returnVar = self.cafe.configurePapMaDet(c_uint(int(afeId)), c_uint(int(chno)), c_uint(int(maEnable)), c_uint(int(maNumSample)), c_uint(int(maWindowCntr)), c_uint(int(maWindowCntrTh)), c_uint(int(maThreshB0)), c_uint(int(maThreshB1)), c_uint(int(maThreshComb)))
		return returnVar

	@cWrapperFuncDecorator
	def configurePapHpfDet(self,afeId, chno, hpfEnable, hpfNumSample, hpfWindowCntr, hpfWindowCntrTh, hpfThreshB0, hpfThreshB1, hpfThreshComb):
		returnVar = self.cafe.configurePapHpfDet(c_uint(int(afeId)), c_uint(int(chno)), c_uint(int(hpfEnable)), c_uint(int(hpfNumSample)), c_uint(int(hpfWindowCntr)), c_uint(int(hpfWindowCntrTh)), c_uint(int(hpfThreshB0)), c_uint(int(hpfThreshB1)), c_uint(int(hpfThreshComb)))
		return returnVar

	@cWrapperFuncDecorator
	def configurePap(self,afeId, chno, enable, multMode, rampDownStartVal, attnStepSize, gainStepSize, detectInWaitState, triggerToRampDown, waitCounter, triggerClearToRampUp, amplUpdateCycles, alarmPulseGPIO, alarmMask, alarmChannelMask, alarmPinDynamicMode, rampStickyMode):
		returnVar = self.cafe.configurePap(c_uint(int(afeId)), c_uint(int(chno)), c_uint(int(enable)), c_uint(int(multMode)), c_uint(int(rampDownStartVal)), c_uint(int(attnStepSize)), c_uint(int(gainStepSize)), c_uint(int(detectInWaitState)), c_float(triggerToRampDown), c_float(waitCounter), c_float(triggerClearToRampUp), c_float(amplUpdateCycles), c_float(alarmPulseGPIO), c_uint(int(alarmMask)), c_uint(int(alarmChannelMask)), c_uint(int(alarmPinDynamicMode)), c_uint(int(rampStickyMode)))
		return returnVar

	@cWrapperFuncDecorator
	def rampStickyClear(self,afeId, chno):
		returnVar = self.cafe.rampStickyClear(c_uint(int(afeId)), c_uint(int(chno)))
		return returnVar

	@cWrapperFuncDecorator
	def papAlarmStatus(self,afeId, chno):
		alarmTriggered = c_uint()
		
		returnVar = self.cafe.papAlarmStatus(c_uint(int(afeId)), c_uint(int(chno)), byref(alarmTriggered))
		return returnVar, alarmTriggered.value

	@cWrapperFuncDecorator
	def clearPapAlarms(self,afeId, chno):
		returnVar = self.cafe.clearPapAlarms(c_uint(int(afeId)), c_uint(int(chno)))
		return returnVar

	@cWrapperFuncDecorator
	def configLaneErrorsForTxPap(self,afeId, chno, laneMask):
		returnVar = self.cafe.configLaneErrorsForTxPap(c_uint(int(afeId)), c_uint(int(chno)), c_uint(int(laneMask)))
		return returnVar

	@cWrapperFuncDecorator
	def serdesTx1010Pattern(self,afeId, laneNo):
		returnVar = self.cafe.serdesTx1010Pattern(c_uint(int(afeId)), c_uint(int(laneNo)))
		return returnVar

	@cWrapperFuncDecorator
	def serdesTxSendData(self,afeId, laneNo):
		returnVar = self.cafe.serdesTxSendData(c_uint(int(afeId)), c_uint(int(laneNo)))
		return returnVar

	@cWrapperFuncDecorator
	def SetSerdesTxCursor(self,afeId, laneNo, mainCursorSetting, preCursorSetting, postCursorSetting):
		returnVar = self.cafe.SetSerdesTxCursor(c_uint(int(afeId)), c_uint(int(laneNo)), c_uint(int(mainCursorSetting)), c_uint(int(preCursorSetting)), c_uint(int(postCursorSetting)))
		return returnVar

	@cWrapperFuncDecorator
	def getSerdesRxPrbsError(self,afeId, laneNo):
		errorRegValue = c_uint()
		
		returnVar = self.cafe.getSerdesRxPrbsError(c_uint(int(afeId)), c_uint(int(laneNo)), byref(errorRegValue))
		return returnVar, errorRegValue.value

	@cWrapperFuncDecorator
	def clearSerdesRxPrbsErrorCounter(self,afeId, laneNo):
		returnVar = self.cafe.clearSerdesRxPrbsErrorCounter(c_uint(int(afeId)), c_uint(int(laneNo)))
		return returnVar

	@cWrapperFuncDecorator
	def enableSerdesRxPrbsCheck(self,afeId, laneNo, prbsMode, enable):
		returnVar = self.cafe.enableSerdesRxPrbsCheck(c_uint(int(afeId)), c_uint(int(laneNo)), c_uint(int(prbsMode)), c_uint(int(enable)))
		return returnVar

	@cWrapperFuncDecorator
	def sendSerdesTxPrbs(self,afeId, laneNo, prbsMode, enable):
		returnVar = self.cafe.sendSerdesTxPrbs(c_uint(int(afeId)), c_uint(int(laneNo)), c_uint(int(prbsMode)), c_uint(int(enable)))
		return returnVar

	@cWrapperFuncDecorator
	def getSerdesRxLaneEyeMarginValue(self,afeId, laneNo):
		regValue = c_uint()
		
		returnVar = self.cafe.getSerdesRxLaneEyeMarginValue(c_uint(int(afeId)), c_uint(int(laneNo)), byref(regValue))
		return returnVar, regValue.value

	@cWrapperFuncDecorator
	def resetSerDesDfeLane(self,afeId, laneNo):
		returnVar = self.cafe.resetSerDesDfeLane(c_uint(int(afeId)), c_uint(int(laneNo)))
		return returnVar

	@cWrapperFuncDecorator
	def resetSerDesDfeAllLanes(self,afeId):
		returnVar = self.cafe.resetSerDesDfeAllLanes(c_uint(int(afeId)))
		return returnVar

	@cWrapperFuncDecorator
	def reAdaptSerDesAllLanes(self,afeId):
		returnVar = self.cafe.reAdaptSerDesAllLanes(c_uint(int(afeId)))
		return returnVar

	def plotSerdesEyeData(self,eyeReadVals,extent):
		ber=[]
		for i in range(95):
			ber+=eyeReadVals[i::95][:]
		# ber is the array printed and extent is the extent printed in the c function.
		ber=numpy.array(ber).reshape((95,33))
		f=matplotlib.pyplot.figure()
		x=numpy.arange(-16.5, 17.5)*0.0232
		y=numpy.arange(-47.5, 48.5)*extent/47
		c=matplotlib.pyplot.pcolor(x, y, ber/-16, cmap='jet', vmin=-8, vmax=0, figure=f)
		cax=f.add_axes([0.9, 0.1, 0.02, 0.8])
		f.colorbar(mappable=c, cax=cax)
		matplotlib.pyplot.show()
		
	@cWrapperFuncDecorator
	def getSerdesEye(self,afeId, laneNo):
		extent = c_uint()
		ber = (c_uint*3135)()
		returnVar = self.cafe.getSerdesEye(c_uint(int(afeId)), c_uint(int(laneNo)), ber, byref(extent))
		ber = list(ber)
		extent=extent.value
		self.plotSerdesEyeData(ber, extent)
		return returnVar, ber, extent

	############################## Setting - Getting SystemParams
	
	def set_X(self,afeId, X):
		returnVar = self.cafe.set_X(c_uint(int(afeId)), c_uint(int(X)))
		return returnVar

	
	def get_X(self,afeId):
		X = c_uint()
		
		returnVar = self.cafe.get_X(c_uint(int(afeId)), byref(X))
		return X.value

	
	def set_numTxNCO(self,afeId, numTxNCO):
		returnVar = self.cafe.set_numTxNCO(c_uint(int(afeId)), c_uint(int(numTxNCO)))
		return returnVar

	
	def get_numTxNCO(self,afeId):
		numTxNCO = c_uint()
		
		returnVar = self.cafe.get_numTxNCO(c_uint(int(afeId)), byref(numTxNCO))
		return numTxNCO.value

	
	def set_numRxNCO(self,afeId, numRxNCO):
		returnVar = self.cafe.set_numRxNCO(c_uint(int(afeId)), c_uint(int(numRxNCO)))
		return returnVar

	
	def get_numRxNCO(self,afeId):
		numRxNCO = c_uint()
		
		returnVar = self.cafe.get_numRxNCO(c_uint(int(afeId)), byref(numRxNCO))
		return numRxNCO.value

	
	def set_numFbNCO(self,afeId, numFbNCO):
		returnVar = self.cafe.set_numFbNCO(c_uint(int(afeId)), c_uint(int(numFbNCO)))
		return returnVar

	
	def get_numFbNCO(self,afeId):
		numFbNCO = c_uint()
		
		returnVar = self.cafe.get_numFbNCO(c_uint(int(afeId)), byref(numFbNCO))
		return returnVar

	
	def set_FRef(self,afeId, FRef):
		returnVar = self.cafe.set_FRef(c_uint(int(afeId)), c_float(FRef))
		return returnVar

	
	def get_FRef(self,afeId):
		FRef = c_float()
		
		returnVar = self.cafe.get_FRef(c_uint(int(afeId)), byref(FRef))
		return FRef.value()

	
	def set_FadcRx(self,afeId, FadcRx):
		returnVar = self.cafe.set_FadcRx(c_uint(int(afeId)), c_float(FadcRx))
		return returnVar

	
	def get_FadcRx(self,afeId):
		FadcRx = c_float()
		
		returnVar = self.cafe.get_FadcRx(c_uint(int(afeId)), byref(FadcRx))
		return FadcRx.value

	
	def set_FadcFb(self,afeId, FadcFb):
		returnVar = self.cafe.set_FadcFb(c_uint(int(afeId)), c_float(FadcFb))
		return returnVar

	
	def get_FadcFb(self,afeId):
		FadcFb = c_float()
		
		returnVar = self.cafe.get_FadcFb(c_uint(int(afeId)), byref(FadcFb))
		return FadcFb.value

	
	def set_Fdac(self,afeId, Fdac):
		returnVar = self.cafe.set_Fdac(c_uint(int(afeId)), c_float(Fdac))
		return returnVar

	
	def get_Fdac(self,afeId):
		Fdac = c_float()
		
		returnVar = self.cafe.get_Fdac(c_uint(int(afeId)), byref(Fdac))
		return Fdac.value

	
	def set_useSpiSysref(self,afeId, useSpiSysref):
		returnVar = self.cafe.set_useSpiSysref(c_uint(int(afeId)), c_uint(int(useSpiSysref)))
		return returnVar

	
	def get_useSpiSysref(self,afeId):
		useSpiSysref = c_uint()
		
		returnVar = self.cafe.get_useSpiSysref(c_uint(int(afeId)), byref(useSpiSysref))
		return useSpiSysref.value

	
	def set_ncoFreqMode(self,afeId, ncoFreqMode):
		returnVar = self.cafe.set_ncoFreqMode(c_uint(int(afeId)), c_uint(int(ncoFreqMode)))
		return returnVar

	
	def get_ncoFreqMode(self,afeId):
		ncoFreqMode = c_uint()
		
		returnVar = self.cafe.get_ncoFreqMode(c_uint(int(afeId)), byref(ncoFreqMode))
		return ncoFreqMode.value

	
	def set_halfRateModeRx(self,afeId):
		halfRateModeRx = (c_uint8*2)()
		halfRateModeRx[0] = self.AFE.systemParams.halfRateModeRx[0]
		halfRateModeRx[1] = self.AFE.systemParams.halfRateModeRx[1]
		returnVar = self.cafe.set_halfRateModeRx(c_uint(int(afeId)), halfRateModeRx)
		return returnVar

	
	def get_halfRateModeRx(self,afeId):
		halfRateModeRx = (c_uint8*2)()
		
		returnVar = self.cafe.get_halfRateModeRx(c_uint(int(afeId)), halfRateModeRx)
		return list(halfRateModeRx)

	
	def set_halfRateModeFb(self,afeId):
		halfRateModeFb = (c_uint8*2)()
		halfRateModeFb[0] = self.AFE.systemParams.halfRateModeFb[0]
		halfRateModeFb[1] = self.AFE.systemParams.halfRateModeFb[1]
		
		returnVar = self.cafe.set_halfRateModeFb(c_uint(int(afeId)), halfRateModeFb)
		return returnVar

	
	def get_halfRateModeFb(self,afeId):
		halfRateModeRx = (c_uint8*2)()
		
		returnVar = self.cafe.get_halfRateModeFb(c_uint(int(afeId)), halfRateModeFb)
		return list(halfRateModeFb)

	
	def set_halfRateModeTx(self,afeId):
		halfRateModeTx = (c_uint8*2)()
		halfRateModeTx[0] = self.AFE.systemParams.halfRateModeTx[0]
		halfRateModeTx[1] = self.AFE.systemParams.halfRateModeTx[1]
		
	
		returnVar = self.cafe.set_halfRateModeTx(c_uint(int(afeId)), halfRateModeTx)
		return returnVar

	
	def get_halfRateModeTx(self,afeId):
		halfRateModeRx = (c_uint8*2)()
		
		returnVar = self.cafe.get_halfRateModeTx(c_uint(int(afeId)), halfRateModeTx)
		return list(halfRateModeTx)

	
	def set_syncLoopBack(self,afeId, syncLoopBack):
		returnVar = self.cafe.set_syncLoopBack(c_uint(int(afeId)), c_uint(int(syncLoopBack)))
		return returnVar

	
	def get_syncLoopBack(self,afeId):
		syncLoopBack = c_uint()
		
		returnVar = self.cafe.get_syncLoopBack(c_uint(int(afeId)), byref(syncLoopBack))
		return syncLoopBack.value

	
	def set_ddcFactorRx(self,afeId):
		ddcFactorRx = (c_uint8*2)()
		ddcFactorRx[0] = self.AFE.systemParams.ddcFactorRx[0]
		ddcFactorRx[1] = self.AFE.systemParams.ddcFactorRx[1]
		
		returnVar = self.cafe.set_ddcFactorRx(c_uint(int(afeId)), ddcFactorRx)
		return list(ddcFactorRx)

	
	def get_ddcFactorRx(self,afeId):
		ddcFactorRx = (c_uint8*2)()
		
		returnVar = self.cafe.get_ddcFactorRx(c_uint(int(afeId)), ddcFactorRx)
		return list(ddcFactorRx)

	
	def set_numBandsRx(self,afeId):
		numBandsRx = (c_uint8*2)()
		numBandsRx[0] = self.AFE.systemParams.numBandsRx[0]
		numBandsRx[1] = self.AFE.systemParams.numBandsRx[1]
		
		returnVar = self.cafe.set_numBandsRx(c_uint(int(afeId)), numBandsRx)
		return returnVar

	
	def get_numBandsRx(self,afeId):
		numBandsRx = (c_uint8*2)()
		numBandsRx[0] = self.AFE.systemParams.numBandsRx[0]
		numBandsRx[1] = self.AFE.systemParams.numBandsRx[1]
		
		returnVar = self.cafe.get_numBandsRx(c_uint(int(afeId)), numBandsRx)
		return list(numBandsRx)

	
	def set_ddcFactorFb(self,afeId):
		ddcFactorFb = (c_uint8*2)()
		ddcFactorFb[0] = self.AFE.systemParams.ddcFactorFb[0]
		ddcFactorFb[1] = self.AFE.systemParams.ddcFactorFb[1]
		
		returnVar = self.cafe.set_ddcFactorFb(c_uint(int(afeId)), ddcFactorFb)
		return returnVar

	
	def get_ddcFactorFb(self,afeId):
		ddcFactorFb = (c_uint8*2)()
		
		returnVar = self.cafe.get_ddcFactorFb(c_uint(int(afeId)), ddcFactorFb)
		return list(ddcFactorFb)

	
	def set_ducFactorTx(self,afeId):
		ducFactorTx = (c_uint8*2)()
		ducFactorTx[0] = self.AFE.systemParams.ducFactorTx[0]
		ducFactorTx[1] = self.AFE.systemParams.ducFactorTx[1]
		
		returnVar = self.cafe.set_ducFactorTx(c_uint(int(afeId)), ducFactorTx)
		return returnVar

	
	def get_ducFactorTx(self,afeId):
		ducFactorTx = (c_uint8*2)()
		
		returnVar = self.cafe.get_ducFactorTx(c_uint(int(afeId)), ducFactorTx)
		return list(ducFactorTx)

	
	def set_numBandsTx(self,afeId):
		numBandsTx = (c_uint8*2)()
		numBandsTx[0] = self.AFE.systemParams.numBandsTx[0]
		numBandsTx[1] = self.AFE.systemParams.numBandsTx[1]
		
		returnVar = self.cafe.set_numBandsTx(c_uint(int(afeId)), numBandsTx)
		return list(numBandsTx)

	
	def get_numBandsTx(self,afeId):
		numBandsTx = (c_uint8*2)()
		
		returnVar = self.cafe.get_numBandsTx(c_uint(int(afeId)), numBandsTx)
		return list(numBandsTx)

	
	def set_enableDacInterleavedMode(self,afeId, enableDacInterleavedMode):
		returnVar = self.cafe.set_enableDacInterleavedMode(c_uint(int(afeId)), c_uint(int(enableDacInterleavedMode)))
		return returnVar

	
	def get_enableDacInterleavedMode(self,afeId):
		enableDacInterleavedMode = c_uint()
		
		returnVar = self.cafe.get_enableDacInterleavedMode(c_uint(int(afeId)), byref(enableDacInterleavedMode))
		return enableDacInterleavedMode.value

	
	def set_txToFbMode(self,afeId, txToFbMode):
		returnVar = self.cafe.set_txToFbMode(c_uint(int(afeId)), c_uint(int(txToFbMode)))
		return returnVar

	
	def get_txToFbMode(self,afeId):
		txToFbMode = c_uint()
		
		returnVar = self.cafe.get_txToFbMode(c_uint(int(afeId)), byref(txToFbMode))
		return txToFbMode.value

	
	def set_chipId(self,afeId, chipId):
		returnVar = self.cafe.set_chipId(c_uint(int(afeId)), c_uint(int(chipId)))
		return returnVar

	
	def get_chipId(self,afeId):
		chipId = c_uint()
		
		returnVar = self.cafe.get_chipId(c_uint(int(afeId)), byref(chipId))
		return chipId.value

	
	def set_chipVersion(self,afeId, chipVersion):
		returnVar = self.cafe.set_chipVersion(c_uint(int(afeId)), c_uint(int(chipVersion)))
		return returnVar

	
	def get_chipVersion(self,afeId):
		chipVersion = c_uint()
		
		returnVar = self.cafe.get_chipVersion(c_uint(int(afeId)), byref(chipVersion))
		return chipVersion.value

	
	def set_agcMode(self,afeId, agcMode):
		returnVar = self.cafe.set_agcMode(c_uint(int(afeId)), c_uint(int(agcMode)))
		return returnVar

	
	def get_agcMode(self,afeId):
		agcMode = c_uint()
		
		returnVar = self.cafe.get_agcMode(c_uint(int(afeId)), byref(agcMode))
		return agcMode.value

	
	def set_bigStepAttkEn(self,afeId):
		bigStepAttkEn = (c_uint8*4)()
		bigStepAttkEn[0] = self.AFE.systemParams.agcParams[0].atken[0]
		bigStepAttkEn[1] = self.AFE.systemParams.agcParams[1].atken[0]
		bigStepAttkEn[2] = self.AFE.systemParams.agcParams[2].atken[0]
		bigStepAttkEn[3] = self.AFE.systemParams.agcParams[3].atken[0]
		
		returnVar = self.cafe.set_bigStepAttkEn(c_uint(int(afeId)), bigStepAttkEn)
		return returnVar

	
	def get_bigStepAttkEn(self,afeId):
		bigStepAttkEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_bigStepAttkEn(c_uint(int(afeId)), bigStepAttkEn)
		return list(bigStepAttkEn)

	
	def set_smallStepAttkEn(self,afeId):
		smallStepAttkEn = (c_uint8*4)()
		smallStepAttkEn[0] = self.AFE.systemParams.agcParams[0].atken[1]
		smallStepAttkEn[1] = self.AFE.systemParams.agcParams[1].atken[1]
		smallStepAttkEn[2] = self.AFE.systemParams.agcParams[2].atken[1]
		smallStepAttkEn[3] = self.AFE.systemParams.agcParams[3].atken[1]
		
		returnVar = self.cafe.set_smallStepAttkEn(c_uint(int(afeId)), smallStepAttkEn)
		return returnVar

	
	def get_smallStepAttkEn(self,afeId):
		smallStepAttkEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_smallStepAttkEn(c_uint(int(afeId)), smallStepAttkEn)
		return list(smallStepAttkEn)

	
	def set_powerAttkEn(self,afeId):
		powerAttkEn = (c_uint8*4)()
		powerAttkEn[0] = self.AFE.systemParams.agcParams[0].atken[2]
		powerAttkEn[1] = self.AFE.systemParams.agcParams[1].atken[2]
		powerAttkEn[2] = self.AFE.systemParams.agcParams[2].atken[2]
		powerAttkEn[3] = self.AFE.systemParams.agcParams[3].atken[2]
		
		returnVar = self.cafe.set_powerAttkEn(c_uint(int(afeId)), powerAttkEn)
		return returnVar

	
	def get_powerAttkEn(self,afeId):
		powerAttkEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_powerAttkEn(c_uint(int(afeId)), powerAttkEn)
		return list(powerAttkEn)

	
	def set_bigStepDecEn(self,afeId):
		bigStepDecEn = (c_uint8*4)()
		bigStepDecEn[0] = self.AFE.systemParams.agcParams[0].decayen[0]
		bigStepDecEn[1] = self.AFE.systemParams.agcParams[1].decayen[0]
		bigStepDecEn[2] = self.AFE.systemParams.agcParams[2].decayen[0]
		bigStepDecEn[3] = self.AFE.systemParams.agcParams[3].decayen[0]
		
		returnVar = self.cafe.set_bigStepDecEn(c_uint(int(afeId)), bigStepDecEn)
		return returnVar

	
	def get_bigStepDecEn(self,afeId):
		bigStepDecEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_bigStepDecEn(c_uint(int(afeId)), bigStepDecEn)
		return list(bigStepDecEn)

	
	def set_smallStepDecEn(self,afeId):
		smallStepDecEn = (c_uint8*4)()
		smallStepDecEn[0] = self.AFE.systemParams.agcParams[0].decayen[1]
		smallStepDecEn[1] = self.AFE.systemParams.agcParams[1].decayen[1]
		smallStepDecEn[2] = self.AFE.systemParams.agcParams[2].decayen[1]
		smallStepDecEn[3] = self.AFE.systemParams.agcParams[3].decayen[1]
		
		returnVar = self.cafe.set_smallStepDecEn(c_uint(int(afeId)), smallStepDecEn)
		return returnVar

	
	def get_smallStepDecEn(self,afeId):
		smallStepDecEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_smallStepDecEn(c_uint(int(afeId)), smallStepDecEn)
		return list(smallStepDecEn)

	
	def set_powerDecEn(self,afeId):
		powerDecEn = (c_uint8*4)()
		powerDecEn[0] = self.AFE.systemParams.agcParams[0].decayen[2]
		powerDecEn[1] = self.AFE.systemParams.agcParams[1].decayen[2]
		powerDecEn[2] = self.AFE.systemParams.agcParams[2].decayen[2]
		powerDecEn[3] = self.AFE.systemParams.agcParams[3].decayen[2]
		
		returnVar = self.cafe.set_powerDecEn(c_uint(int(afeId)), powerDecEn)
		return returnVar

	
	def get_powerDecEn(self,afeId):
		powerDecEn = (c_uint8*4)()
		
		returnVar = self.cafe.get_powerDecEn(c_uint(int(afeId)), powerDecEn)
		return list(powerDecEn)

	
	def set_bigStepAttkThresh(self,afeId):
		bigStepAttkThresh = (c_uint8*4)()
		bigStepAttkThresh[0] = self.AFE.systemParams.agcParams[0].atkthreshold[0]
		bigStepAttkThresh[1] = self.AFE.systemParams.agcParams[1].atkthreshold[0]
		bigStepAttkThresh[2] = self.AFE.systemParams.agcParams[2].atkthreshold[0]
		bigStepAttkThresh[3] = self.AFE.systemParams.agcParams[3].atkthreshold[0]
		
		returnVar = self.cafe.set_bigStepAttkThresh(c_uint(int(afeId)), bigStepAttkThresh)
		return returnVar

	
	def get_bigStepAttkThresh(self,afeId):
		bigStepAttkThresh = (c_uint8*4)()
		
		returnVar = self.cafe.get_bigStepAttkThresh(c_uint(int(afeId)), bigStepAttkThresh)
		return list(bigStepAttkThresh)

	
	def set_smallStepAttkThresh(self,afeId):
		smallStepAttkThresh = (c_uint8*4)()
		smallStepAttkThresh[0] = self.AFE.systemParams.agcParams[0].atkthreshold[1]
		smallStepAttkThresh[1] = self.AFE.systemParams.agcParams[1].atkthreshold[1]
		smallStepAttkThresh[2] = self.AFE.systemParams.agcParams[2].atkthreshold[1]
		smallStepAttkThresh[3] = self.AFE.systemParams.agcParams[3].atkthreshold[1]
		
		returnVar = self.cafe.set_smallStepAttkThresh(c_uint(int(afeId)), smallStepAttkThresh)
		return returnVar

	
	def get_smallStepAttkThresh(self,afeId):
		smallStepAttkThresh = (c_uint8*4)()
		
		returnVar = self.cafe.get_smallStepAttkThresh(c_uint(int(afeId)), smallStepAttkThresh)
		return list(smallStepAttkThresh)

	
	def set_powerAttkThresh(self,afeId):
		powerAttkThresh = (c_uint8*4)()
		powerAttkThresh[0] = self.AFE.systemParams.agcParams[0].atkthreshold[2]
		powerAttkThresh[1] = self.AFE.systemParams.agcParams[1].atkthreshold[2]
		powerAttkThresh[2] = self.AFE.systemParams.agcParams[2].atkthreshold[2]
		powerAttkThresh[3] = self.AFE.systemParams.agcParams[3].atkthreshold[2]
		
		returnVar = self.cafe.set_powerAttkThresh(c_uint(int(afeId)), powerAttkThresh)
		return returnVar

	
	def get_powerAttkThresh(self,afeId):
		powerAttkThresh = (c_uint8*4)()
		
		returnVar = self.cafe.get_powerAttkThresh(c_uint(int(afeId)), powerAttkThresh)
		return list(powerAttkThresh)

	
	def set_bigStepDecThresh(self,afeId):
		bigStepDecThresh = (c_uint8*4)()
		bigStepDecThresh[0] = self.AFE.systemParams.agcParams[0].decaythreshold[0]
		bigStepDecThresh[1] = self.AFE.systemParams.agcParams[1].decaythreshold[0]
		bigStepDecThresh[2] = self.AFE.systemParams.agcParams[2].decaythreshold[0]
		bigStepDecThresh[3] = self.AFE.systemParams.agcParams[3].decaythreshold[0]
		
		returnVar = self.cafe.set_bigStepDecThresh(c_uint(int(afeId)), bigStepDecThresh)
		return returnVar

	
	def get_bigStepDecThresh(self,afeId):
		bigStepDecThresh = (c_uint8*4)()
		
		returnVar = self.cafe.get_bigStepDecThresh(c_uint(int(afeId)), bigStepDecThresh)
		return list(bigStepDecThresh)

	
	def set_smallStepDecThresh(self,afeId):
		smallStepDecThresh = (c_uint8*4)()
		smallStepDecThresh[0] = self.AFE.systemParams.agcParams[0].decaythreshold[1]
		smallStepDecThresh[1] = self.AFE.systemParams.agcParams[1].decaythreshold[1]
		smallStepDecThresh[2] = self.AFE.systemParams.agcParams[2].decaythreshold[1]
		smallStepDecThresh[3] = self.AFE.systemParams.agcParams[3].decaythreshold[1]
		
		returnVar = self.cafe.set_smallStepDecThresh(c_uint(int(afeId)), smallStepDecThresh)
		return returnVar

	
	def get_smallStepDecThresh(self,afeId):
		smallStepDecThresh = c_uint()
		
		returnVar = self.cafe.get_smallStepDecThresh(c_uint(int(afeId)), byref(smallStepDecThresh))
		return smallStepDecThresh.value

	
	def set_powerDecThresh(self,afeId):
		powerDecThresh = (c_uint8*4)()
		powerDecThresh[0] = self.AFE.systemParams.agcParams[0].decaythreshold[2]
		powerDecThresh[1] = self.AFE.systemParams.agcParams[1].decaythreshold[2]
		powerDecThresh[2] = self.AFE.systemParams.agcParams[2].decaythreshold[2]
		powerDecThresh[3] = self.AFE.systemParams.agcParams[3].decaythreshold[2]
		
		returnVar = self.cafe.set_powerDecThresh(c_uint(int(afeId)), powerDecThresh)
		return returnVar

	
	def get_powerDecThresh(self,afeId):
		powerDecThresh = (c_uint8*4)()
		
		returnVar = self.cafe.get_powerDecThresh(c_uint(int(afeId)), powerDecThresh)
		return list(powerDecThresh)

	
	def set_bigStepAttkWinLen(self,afeId):
		bigStepAttkWinLen = (c_uint32*4)()
		bigStepAttkWinLen[0] = self.AFE.systemParams.agcParams[0].atkwinlength[0]
		bigStepAttkWinLen[1] = self.AFE.systemParams.agcParams[1].atkwinlength[0]
		bigStepAttkWinLen[2] = self.AFE.systemParams.agcParams[2].atkwinlength[0]
		bigStepAttkWinLen[3] = self.AFE.systemParams.agcParams[3].atkwinlength[0]
		
		returnVar = self.cafe.set_bigStepAttkWinLen(c_uint(int(afeId)), bigStepAttkWinLen)
		return returnVar

	
	def get_bigStepAttkWinLen(self,afeId):
		bigStepAttkWinLen = (c_uint32*4)()
		
		returnVar = self.cafe.get_bigStepAttkWinLen(c_uint(int(afeId)), bigStepAttkWinLen)
		return list(bigStepAttkWinLen)

	
	def set_miscStepAttkWinLen(self,afeId):
		miscStepAttkWinLen = (c_uint32*4)()
		miscStepAttkWinLen[0] = self.AFE.systemParams.agcParams[0].atkwinlength[1]
		miscStepAttkWinLen[1] = self.AFE.systemParams.agcParams[1].atkwinlength[1]
		miscStepAttkWinLen[2] = self.AFE.systemParams.agcParams[2].atkwinlength[1]
		miscStepAttkWinLen[3] = self.AFE.systemParams.agcParams[3].atkwinlength[1]
		
		returnVar = self.cafe.set_miscStepAttkWinLen(c_uint(int(afeId)), miscStepAttkWinLen)
		return returnVar

	
	def get_miscStepAttkWinLen(self,afeId):
		miscStepAttkWinLen = (c_uint32*4)()
		
		returnVar = self.cafe.get_miscStepAttkWinLen(c_uint(int(afeId)), miscStepAttkWinLen)
		return list(miscStepAttkWinLen)

	
	def set_decayWinLen(self,afeId):
		decayWinLen = (c_uint32*4)()
		decayWinLen[0] = self.AFE.systemParams.agcParams[0].decaywinlength
		decayWinLen[1] = self.AFE.systemParams.agcParams[1].decaywinlength
		decayWinLen[2] = self.AFE.systemParams.agcParams[2].decaywinlength
		decayWinLen[3] = self.AFE.systemParams.agcParams[3].decaywinlength
		
		returnVar = self.cafe.set_decayWinLen(c_uint(int(afeId)), decayWinLen)
		return returnVar

	
	def get_decayWinLen(self,afeId):
		decayWinLen = (c_uint32*4)()
		
		returnVar = self.cafe.get_decayWinLen(c_uint(int(afeId)), decayWinLen)
		return list(decayWinLen)

	
	def set_jesdProtocol(self,afeId, jesdProtocol):
		returnVar = self.cafe.set_jesdProtocol(c_uint(int(afeId)), c_uint(int(jesdProtocol)))
		return returnVar

	
	def get_jesdProtocol(self,afeId):
		jesdProtocol = c_uint()
		
		returnVar = self.cafe.get_jesdProtocol(c_uint(int(afeId)), byref(jesdProtocol))
		return jesdProtocol.value
	
	def set_spiInUseForPllAccess(self,afeId, spiInUseForPllAccess):
		returnVar = self.cafe.set_spiInUseForPllAccess(c_uint(int(afeId)), c_uint(int(spiInUseForPllAccess)))
		return returnVar

	
	def get_spiInUseForPllAccess(self,afeId):
		spiInUseForPllAccess = c_uint()
		
		returnVar = self.cafe.get_jesdProtocol(c_uint(int(afeId)), byref(spiInUseForPllAccess))
		return spiInUseForPllAccess.value


	def updateCSystemParams(self):
		if self.AFE==None:
			Globals.info("AFE library not passed.")
			return
		systemParams=self.AFE.systemParams
		systemStatus=self.AFE.systemStatus
		self.set_X(0, int(systemStatus.X*1000))
		self.set_numTxNCO(0, systemParams.numTxNCO)
		self.set_numRxNCO(0, systemParams.numRxNCO)
		self.set_numFbNCO(0, systemParams.numFbNCO)
		self.set_FRef(0, systemParams.FRef)
		self.set_FadcRx(0, systemParams.FadcRx)
		self.set_FadcFb(0, systemParams.FadcFb)
		self.set_Fdac(0, systemParams.Fdac)
		self.set_useSpiSysref(0, systemParams.useSpiSysref)
		self.set_ncoFreqMode(0, ('FCW','1KHz').index(systemParams.ncoFreqMode))
		self.set_halfRateModeRx(0)
		self.set_halfRateModeFb(0)
		self.set_halfRateModeTx(0)
		self.set_syncLoopBack(0, systemParams.syncLoopBack)
		self.set_ddcFactorRx(0)
		self.set_numBandsRx(0)
		self.set_ddcFactorFb(0)
		self.set_ducFactorTx(0)
		self.set_numBandsTx(0)
		self.set_enableDacInterleavedMode(0, systemParams.enableDacInterleavedMode)
		self.set_txToFbMode(0, systemParams.txToFbMode)
		self.set_chipId(0, systemParams.chipId)
		self.set_chipVersion(0, systemParams.chipVersion)
		self.set_agcMode(0, systemParams.agcParams[0].agcMode)
		self.set_bigStepAttkEn(0)
		self.set_smallStepAttkEn(0)
		self.set_powerAttkEn(0)
		self.set_bigStepDecEn(0)
		self.set_smallStepDecEn(0)
		self.set_powerDecEn(0)
		self.set_bigStepAttkThresh(0)
		self.set_smallStepAttkThresh(0)
		self.set_powerAttkThresh(0)
		self.set_bigStepDecThresh(0)
		self.set_smallStepDecThresh(0)
		self.set_powerDecThresh(0)
		self.set_bigStepAttkWinLen(0)
		self.set_miscStepAttkWinLen(0)
		self.set_decayWinLen(0)
		self.set_jesdProtocol(0, systemParams.jesdTxProtocol[0])


if __name__ == "__main__":
	import os
	import time
	os.chdir(r"C\Asterix5p6\code\core")
	CAFE=afeCTestWrapper(None,"AFE79xx HSC1386 B",None)
	print CAFE.cafe.getAfeLogLvl()
	print CAFE.cafe.setAfeLogLvl(3)
	print "Start"
	#time.sleep(0.1)
	#print CAFE.readReg(0x6)
	#time.sleep(0.1)
	#CAFE.writeReg(0x10,10)
	#time.sleep(0.1)
	#print CAFE.readReg(0x10)
	#CAFE.writeReg(0x10,0)
	#print CAFE.readReg(0x10)
	a=CAFE.cafe.overrideTdd(c_uint(0),c_uint(0xf), c_uint(0x0), c_uint(0x0), c_uint(1))
	#time.sleep(5)
	a=CAFE.cafe.updateRxNco(c_uint(0x0),c_uint(0x0),c_ulong(1800000),c_uint(0x0),c_uint(0x0))
	#CAFE.cafe.overrideTdd(c_uint(0),c_uint(0x0), c_uint(0x0), c_uint(0x0), c_uint(1))
	