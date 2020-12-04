#ifndef _PARAMAS_GET_SET_
#define _PARAMAS_GET_SET_
uint8_t set_X(uint8_t afeId, uint32_t X);
uint8_t get_X(uint8_t afeId, uint32_t *X);
uint8_t set_numTxNCO(uint8_t afeId, uint8_t numTxNCO);
uint8_t get_numTxNCO(uint8_t afeId, uint8_t *numTxNCO);
uint8_t set_numRxNCO(uint8_t afeId, uint8_t numRxNCO);
uint8_t get_numRxNCO(uint8_t afeId, uint8_t *numRxNCO);
uint8_t set_numFbNCO(uint8_t afeId, uint8_t numFbNCO);
uint8_t get_numFbNCO(uint8_t afeId, uint8_t *numFbNCO);
uint8_t set_FRef(uint8_t afeId, float FRef);
uint8_t get_FRef(uint8_t afeId, float *FRef);
uint8_t set_FadcRx(uint8_t afeId, float FadcRx);
uint8_t get_FadcRx(uint8_t afeId, float *FadcRx);
uint8_t set_FadcFb(uint8_t afeId, float FadcFb);
uint8_t get_FadcFb(uint8_t afeId, float *FadcFb);
uint8_t set_Fdac(uint8_t afeId, float Fdac);
uint8_t get_Fdac(uint8_t afeId, float *Fdac);
uint8_t set_useSpiSysref(uint8_t afeId, uint8_t useSpiSysref);
uint8_t get_useSpiSysref(uint8_t afeId, uint8_t *useSpiSysref);
uint8_t set_ncoFreqMode(uint8_t afeId, uint8_t ncoFreqMode);
uint8_t get_ncoFreqMode(uint8_t afeId, uint8_t *ncoFreqMode);
uint8_t set_halfRateModeRx(uint8_t afeId, uint8_t *halfRateModeRx);
uint8_t get_halfRateModeRx(uint8_t afeId, uint8_t *halfRateModeRx);
uint8_t set_halfRateModeFb(uint8_t afeId, uint8_t *halfRateModeFb);
uint8_t get_halfRateModeFb(uint8_t afeId, uint8_t *halfRateModeFb);
uint8_t set_halfRateModeTx(uint8_t afeId, uint8_t *halfRateModeTx);
uint8_t get_halfRateModeTx(uint8_t afeId, uint8_t *halfRateModeTx);
uint8_t set_syncLoopBack(uint8_t afeId, uint8_t syncLoopBack);
uint8_t get_syncLoopBack(uint8_t afeId, uint8_t *syncLoopBack);
uint8_t set_ddcFactorRx(uint8_t afeId, uint8_t *ddcFactorRx);
uint8_t get_ddcFactorRx(uint8_t afeId, uint8_t *ddcFactorRx);
uint8_t set_numBandsRx(uint8_t afeId, uint8_t *numBandsRx);
uint8_t get_numBandsRx(uint8_t afeId, uint8_t *numBandsRx);
uint8_t set_ddcFactorFb(uint8_t afeId, uint8_t *ddcFactorFb);
uint8_t get_ddcFactorFb(uint8_t afeId, uint8_t *ddcFactorFb);
uint8_t set_ducFactorTx(uint8_t afeId, uint8_t *ducFactorTx);
uint8_t get_ducFactorTx(uint8_t afeId, uint8_t *ducFactorTx);
uint8_t set_numBandsTx(uint8_t afeId, uint8_t *numBandsTx);
uint8_t get_numBandsTx(uint8_t afeId, uint8_t *numBandsTx);
uint8_t set_enableDacInterleavedMode(uint8_t afeId, uint8_t enableDacInterleavedMode);
uint8_t get_enableDacInterleavedMode(uint8_t afeId, uint8_t *enableDacInterleavedMode);
uint8_t set_txToFbMode(uint8_t afeId, uint8_t txToFbMode);
uint8_t get_txToFbMode(uint8_t afeId, uint8_t *txToFbMode);
uint8_t set_chipId(uint8_t afeId, uint32_t chipId);
uint8_t get_chipId(uint8_t afeId, uint32_t *chipId);
uint8_t set_chipVersion(uint8_t afeId, uint8_t chipVersion);
uint8_t get_chipVersion(uint8_t afeId, uint8_t *chipVersion);
uint8_t set_agcMode(uint8_t afeId, uint8_t agcMode);
uint8_t get_agcMode(uint8_t afeId, uint8_t *agcMode);
uint8_t set_bigStepAttkEn(uint8_t afeId, uint8_t *bigStepAttkEn);
uint8_t get_bigStepAttkEn(uint8_t afeId, uint8_t *bigStepAttkEn);
uint8_t set_smallStepAttkEn(uint8_t afeId, uint8_t *smallStepAttkEn);
uint8_t get_smallStepAttkEn(uint8_t afeId, uint8_t *smallStepAttkEn);
uint8_t set_powerAttkEn(uint8_t afeId, uint8_t *powerAttkEn);
uint8_t get_powerAttkEn(uint8_t afeId, uint8_t *powerAttkEn);
uint8_t set_bigStepDecEn(uint8_t afeId, uint8_t *bigStepDecEn);
uint8_t get_bigStepDecEn(uint8_t afeId, uint8_t *bigStepDecEn);
uint8_t set_smallStepDecEn(uint8_t afeId, uint8_t *smallStepDecEn);
uint8_t get_smallStepDecEn(uint8_t afeId, uint8_t *smallStepDecEn);
uint8_t set_powerDecEn(uint8_t afeId, uint8_t *powerDecEn);
uint8_t get_powerDecEn(uint8_t afeId, uint8_t *powerDecEn);
uint8_t set_bigStepAttkThresh(uint8_t afeId, uint8_t *bigStepAttkThresh);
uint8_t get_bigStepAttkThresh(uint8_t afeId, uint8_t *bigStepAttkThresh);
uint8_t set_smallStepAttkThresh(uint8_t afeId, uint8_t *smallStepAttkThresh);
uint8_t get_smallStepAttkThresh(uint8_t afeId, uint8_t *smallStepAttkThresh);
uint8_t set_powerAttkThresh(uint8_t afeId, uint8_t *powerAttkThresh);
uint8_t get_powerAttkThresh(uint8_t afeId, uint8_t *powerAttkThresh);
uint8_t set_bigStepDecThresh(uint8_t afeId, uint8_t *bigStepDecThresh);
uint8_t get_bigStepDecThresh(uint8_t afeId, uint8_t *bigStepDecThresh);
uint8_t set_smallStepDecThresh(uint8_t afeId, uint8_t *smallStepDecThresh);
uint8_t get_smallStepDecThresh(uint8_t afeId, uint8_t *smallStepDecThresh);
uint8_t set_powerDecThresh(uint8_t afeId, uint8_t *powerDecThresh);
uint8_t get_powerDecThresh(uint8_t afeId, uint8_t *powerDecThresh);
uint8_t set_bigStepAttkWinLen(uint8_t afeId, uint32_t *bigStepAttkWinLen);
uint8_t get_bigStepAttkWinLen(uint8_t afeId, uint32_t *bigStepAttkWinLen);
uint8_t set_miscStepAttkWinLen(uint8_t afeId, uint32_t *miscStepAttkWinLen);
uint8_t get_miscStepAttkWinLen(uint8_t afeId, uint32_t *miscStepAttkWinLen);
uint8_t set_decayWinLen(uint8_t afeId, uint32_t *decayWinLen);
uint8_t get_decayWinLen(uint8_t afeId, uint32_t *decayWinLen);
uint8_t set_jesdProtocol(uint8_t afeId, uint8_t jesdProtocol);
uint8_t get_jesdProtocol(uint8_t afeId, uint8_t *jesdProtocol);
uint8_t set_spiInUseForPllAccess(uint8_t afeId, uint8_t spiInUseForPllAccess);
uint8_t get_spiInUseForPllAccess(uint8_t afeId, uint8_t *spiInUseForPllAccess);

uint8_t set_X(uint8_t afeId, uint32_t X)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].X = X;
    return RET_OK;
}
uint8_t get_X(uint8_t afeId, uint32_t *X)
{
    AFE_ID_VALIDITY();

    *X = systemParams[afeId].X;
    return RET_OK;
}
uint8_t set_numTxNCO(uint8_t afeId, uint8_t numTxNCO)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].numTxNCO = numTxNCO;
    return RET_OK;
}
uint8_t get_numTxNCO(uint8_t afeId, uint8_t *numTxNCO)
{
    AFE_ID_VALIDITY();

    *numTxNCO = systemParams[afeId].numTxNCO;
    return RET_OK;
}
uint8_t set_numRxNCO(uint8_t afeId, uint8_t numRxNCO)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].numRxNCO = numRxNCO;
    return RET_OK;
}
uint8_t get_numRxNCO(uint8_t afeId, uint8_t *numRxNCO)
{
    AFE_ID_VALIDITY();

    *numRxNCO = systemParams[afeId].numRxNCO;
    return RET_OK;
}
uint8_t set_numFbNCO(uint8_t afeId, uint8_t numFbNCO)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].numFbNCO = numFbNCO;
    return RET_OK;
}
uint8_t get_numFbNCO(uint8_t afeId, uint8_t *numFbNCO)
{
    AFE_ID_VALIDITY();

    *numFbNCO = systemParams[afeId].numFbNCO;
    return RET_OK;
}
uint8_t set_FRef(uint8_t afeId, float FRef)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].FRef = FRef;
    return RET_OK;
}
uint8_t get_FRef(uint8_t afeId, float *FRef)
{
    AFE_ID_VALIDITY();

    *FRef = systemParams[afeId].FRef;
    return RET_OK;
}
uint8_t set_FadcRx(uint8_t afeId, float FadcRx)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].FadcRx = FadcRx;
    return RET_OK;
}
uint8_t get_FadcRx(uint8_t afeId, float *FadcRx)
{
    AFE_ID_VALIDITY();

    *FadcRx = systemParams[afeId].FadcRx;
    return RET_OK;
}
uint8_t set_FadcFb(uint8_t afeId, float FadcFb)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].FadcFb = FadcFb;
    return RET_OK;
}
uint8_t get_FadcFb(uint8_t afeId, float *FadcFb)
{
    AFE_ID_VALIDITY();

    *FadcFb = systemParams[afeId].FadcFb;
    return RET_OK;
}
uint8_t set_Fdac(uint8_t afeId, float Fdac)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].Fdac = Fdac;
    return RET_OK;
}
uint8_t get_Fdac(uint8_t afeId, float *Fdac)
{
    AFE_ID_VALIDITY();

    *Fdac = systemParams[afeId].Fdac;
    return RET_OK;
}
uint8_t set_useSpiSysref(uint8_t afeId, uint8_t useSpiSysref)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].useSpiSysref = useSpiSysref;
    return RET_OK;
}
uint8_t get_useSpiSysref(uint8_t afeId, uint8_t *useSpiSysref)
{
    AFE_ID_VALIDITY();

    *useSpiSysref = systemParams[afeId].useSpiSysref;
    return RET_OK;
}
uint8_t set_ncoFreqMode(uint8_t afeId, uint8_t ncoFreqMode)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].ncoFreqMode = ncoFreqMode;
    return RET_OK;
}
uint8_t get_ncoFreqMode(uint8_t afeId, uint8_t *ncoFreqMode)
{
    AFE_ID_VALIDITY();

    *ncoFreqMode = systemParams[afeId].ncoFreqMode;
    return RET_OK;
}
uint8_t set_halfRateModeRx(uint8_t afeId, uint8_t *halfRateModeRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        systemParams[afeId].halfRateModeRx[i] = halfRateModeRx[i];
    }
    return RET_OK;
}
uint8_t get_halfRateModeRx(uint8_t afeId, uint8_t *halfRateModeRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        halfRateModeRx[i] = systemParams[afeId].halfRateModeRx[i];
    }
    return RET_OK;
}
uint8_t set_halfRateModeFb(uint8_t afeId, uint8_t *halfRateModeFb)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        systemParams[afeId].halfRateModeFb[i] = halfRateModeFb[i];
    }
    return RET_OK;
}
uint8_t get_halfRateModeFb(uint8_t afeId, uint8_t *halfRateModeFb)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        halfRateModeFb[i] = systemParams[afeId].halfRateModeFb[i];
    }
    return RET_OK;
}
uint8_t set_halfRateModeTx(uint8_t afeId, uint8_t *halfRateModeTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        systemParams[afeId].halfRateModeTx[i] = halfRateModeTx[i];
    }
    return RET_OK;
}
uint8_t get_halfRateModeTx(uint8_t afeId, uint8_t *halfRateModeTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        halfRateModeTx[i] = systemParams[afeId].halfRateModeTx[i];
    }
    return RET_OK;
}
uint8_t set_syncLoopBack(uint8_t afeId, uint8_t syncLoopBack)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].syncLoopBack = syncLoopBack;
    return RET_OK;
}
uint8_t get_syncLoopBack(uint8_t afeId, uint8_t *syncLoopBack)
{
    AFE_ID_VALIDITY();

    *syncLoopBack = systemParams[afeId].syncLoopBack;
    return RET_OK;
}
uint8_t set_ddcFactorRx(uint8_t afeId, uint8_t *ddcFactorRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].ddcFactorRx[i] = ddcFactorRx[i];
    }
    return RET_OK;
}
uint8_t get_ddcFactorRx(uint8_t afeId, uint8_t *ddcFactorRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        ddcFactorRx[i] = systemParams[afeId].ddcFactorRx[i];
    }
    return RET_OK;
}
uint8_t set_numBandsRx(uint8_t afeId, uint8_t *numBandsRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].numBandsRx[i] = numBandsRx[i];
    }
    return RET_OK;
}
uint8_t get_numBandsRx(uint8_t afeId, uint8_t *numBandsRx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        numBandsRx[i] = systemParams[afeId].numBandsRx[i];
    }
    return RET_OK;
}
uint8_t set_ddcFactorFb(uint8_t afeId, uint8_t *ddcFactorFb)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        systemParams[afeId].ddcFactorFb[i] = ddcFactorFb[i];
    }
    return RET_OK;
}
uint8_t get_ddcFactorFb(uint8_t afeId, uint8_t *ddcFactorFb)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 2; i++)
    {
        ddcFactorFb[i] = systemParams[afeId].ddcFactorFb[i];
    }
    return RET_OK;
}
uint8_t set_ducFactorTx(uint8_t afeId, uint8_t *ducFactorTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].ducFactorTx[i] = ducFactorTx[i];
    }
    return RET_OK;
}
uint8_t get_ducFactorTx(uint8_t afeId, uint8_t *ducFactorTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        ducFactorTx[i] = systemParams[afeId].ducFactorTx[i];
    }
    return RET_OK;
}
uint8_t set_numBandsTx(uint8_t afeId, uint8_t *numBandsTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].numBandsTx[i] = numBandsTx[i];
    }
    return RET_OK;
}
uint8_t get_numBandsTx(uint8_t afeId, uint8_t *numBandsTx)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        numBandsTx[i] = systemParams[afeId].numBandsTx[i];
    }
    return RET_OK;
}
uint8_t set_enableDacInterleavedMode(uint8_t afeId, uint8_t enableDacInterleavedMode)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].enableDacInterleavedMode = enableDacInterleavedMode;
    return RET_OK;
}
uint8_t get_enableDacInterleavedMode(uint8_t afeId, uint8_t *enableDacInterleavedMode)
{
    AFE_ID_VALIDITY();

    *enableDacInterleavedMode = systemParams[afeId].enableDacInterleavedMode;
    return RET_OK;
}
uint8_t set_txToFbMode(uint8_t afeId, uint8_t txToFbMode)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].txToFbMode = txToFbMode;
    return RET_OK;
}
uint8_t get_txToFbMode(uint8_t afeId, uint8_t *txToFbMode)
{
    AFE_ID_VALIDITY();

    *txToFbMode = systemParams[afeId].txToFbMode;
    return RET_OK;
}
uint8_t set_chipId(uint8_t afeId, uint32_t chipId)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].chipId = chipId;
    return RET_OK;
}
uint8_t get_chipId(uint8_t afeId, uint32_t *chipId)
{
    AFE_ID_VALIDITY();

    *chipId = systemParams[afeId].chipId;
    return RET_OK;
}
uint8_t set_chipVersion(uint8_t afeId, uint8_t chipVersion)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].chipVersion = chipVersion;
    return RET_OK;
}
uint8_t get_chipVersion(uint8_t afeId, uint8_t *chipVersion)
{
    AFE_ID_VALIDITY();

    *chipVersion = systemParams[afeId].chipVersion;
    return RET_OK;
}
uint8_t set_agcMode(uint8_t afeId, uint8_t agcMode)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].agcMode = agcMode;
    return RET_OK;
}
uint8_t get_agcMode(uint8_t afeId, uint8_t *agcMode)
{
    AFE_ID_VALIDITY();

    *agcMode = systemParams[afeId].agcMode;
    return RET_OK;
}
uint8_t set_bigStepAttkEn(uint8_t afeId, uint8_t *bigStepAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].bigStepAttkEn[i] = bigStepAttkEn[i];
    }
    return RET_OK;
}
uint8_t get_bigStepAttkEn(uint8_t afeId, uint8_t *bigStepAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        bigStepAttkEn[i] = systemParams[afeId].bigStepAttkEn[i];
    }
    return RET_OK;
}
uint8_t set_smallStepAttkEn(uint8_t afeId, uint8_t *smallStepAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].smallStepAttkEn[i] = smallStepAttkEn[i];
    }
    return RET_OK;
}
uint8_t get_smallStepAttkEn(uint8_t afeId, uint8_t *smallStepAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        smallStepAttkEn[i] = systemParams[afeId].smallStepAttkEn[i];
    }
    return RET_OK;
}
uint8_t set_powerAttkEn(uint8_t afeId, uint8_t *powerAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].powerAttkEn[i] = powerAttkEn[i];
    }
    return RET_OK;
}
uint8_t get_powerAttkEn(uint8_t afeId, uint8_t *powerAttkEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        powerAttkEn[i] = systemParams[afeId].powerAttkEn[i];
    }
    return RET_OK;
}
uint8_t set_bigStepDecEn(uint8_t afeId, uint8_t *bigStepDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].bigStepDecEn[i] = bigStepDecEn[i];
    }
    return RET_OK;
}
uint8_t get_bigStepDecEn(uint8_t afeId, uint8_t *bigStepDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        bigStepDecEn[i] = systemParams[afeId].bigStepDecEn[i];
    }
    return RET_OK;
}
uint8_t set_smallStepDecEn(uint8_t afeId, uint8_t *smallStepDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].smallStepDecEn[i] = smallStepDecEn[i];
    }
    return RET_OK;
}
uint8_t get_smallStepDecEn(uint8_t afeId, uint8_t *smallStepDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        smallStepDecEn[i] = systemParams[afeId].smallStepDecEn[i];
    }
    return RET_OK;
}
uint8_t set_powerDecEn(uint8_t afeId, uint8_t *powerDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].powerDecEn[i] = powerDecEn[i];
    }
    return RET_OK;
}
uint8_t get_powerDecEn(uint8_t afeId, uint8_t *powerDecEn)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        powerDecEn[i] = systemParams[afeId].powerDecEn[i];
    }
    return RET_OK;
}
uint8_t set_bigStepAttkThresh(uint8_t afeId, uint8_t *bigStepAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].bigStepAttkThresh[i] = bigStepAttkThresh[i];
    }
    return RET_OK;
}
uint8_t get_bigStepAttkThresh(uint8_t afeId, uint8_t *bigStepAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        bigStepAttkThresh[i] = systemParams[afeId].bigStepAttkThresh[i];
    }
    return RET_OK;
}
uint8_t set_smallStepAttkThresh(uint8_t afeId, uint8_t *smallStepAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].smallStepAttkThresh[i] = smallStepAttkThresh[i];
    }
    return RET_OK;
}
uint8_t get_smallStepAttkThresh(uint8_t afeId, uint8_t *smallStepAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        smallStepAttkThresh[i] = systemParams[afeId].smallStepAttkThresh[i];
    }
    return RET_OK;
}
uint8_t set_powerAttkThresh(uint8_t afeId, uint8_t *powerAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].powerAttkThresh[i] = powerAttkThresh[i];
    }
    return RET_OK;
}
uint8_t get_powerAttkThresh(uint8_t afeId, uint8_t *powerAttkThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        powerAttkThresh[i] = systemParams[afeId].powerAttkThresh[i];
    }
    return RET_OK;
}
uint8_t set_bigStepDecThresh(uint8_t afeId, uint8_t *bigStepDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].bigStepDecThresh[i] = bigStepDecThresh[i];
    }
    return RET_OK;
}
uint8_t get_bigStepDecThresh(uint8_t afeId, uint8_t *bigStepDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        bigStepDecThresh[i] = systemParams[afeId].bigStepDecThresh[i];
    }
    return RET_OK;
}
uint8_t set_smallStepDecThresh(uint8_t afeId, uint8_t *smallStepDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].smallStepDecThresh[i] = smallStepDecThresh[i];
    }
    return RET_OK;
}
uint8_t get_smallStepDecThresh(uint8_t afeId, uint8_t *smallStepDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        smallStepDecThresh[i] = systemParams[afeId].smallStepDecThresh[i];
    }
    return RET_OK;
}
uint8_t set_powerDecThresh(uint8_t afeId, uint8_t *powerDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].powerDecThresh[i] = powerDecThresh[i];
    }
    return RET_OK;
}
uint8_t get_powerDecThresh(uint8_t afeId, uint8_t *powerDecThresh)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        powerDecThresh[i] = systemParams[afeId].powerDecThresh[i];
    }
    return RET_OK;
}
uint8_t set_bigStepAttkWinLen(uint8_t afeId, uint32_t *bigStepAttkWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].bigStepAttkWinLen[i] = bigStepAttkWinLen[i];
    }
    return RET_OK;
}
uint8_t get_bigStepAttkWinLen(uint8_t afeId, uint32_t *bigStepAttkWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        bigStepAttkWinLen[i] = systemParams[afeId].bigStepAttkWinLen[i];
    }
    return RET_OK;
}
uint8_t set_miscStepAttkWinLen(uint8_t afeId, uint32_t *miscStepAttkWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].miscStepAttkWinLen[i] = miscStepAttkWinLen[i];
    }
    return RET_OK;
}
uint8_t get_miscStepAttkWinLen(uint8_t afeId, uint32_t *miscStepAttkWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        miscStepAttkWinLen[i] = systemParams[afeId].miscStepAttkWinLen[i];
    }
    return RET_OK;
}
uint8_t set_decayWinLen(uint8_t afeId, uint32_t *decayWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        systemParams[afeId].decayWinLen[i] = decayWinLen[i];
    }
    return RET_OK;
}
uint8_t get_decayWinLen(uint8_t afeId, uint32_t *decayWinLen)
{
    AFE_ID_VALIDITY();

    for (uint8_t i = 0; i < 4; i++)
    {
        decayWinLen[i] = systemParams[afeId].decayWinLen[i];
    }
    return RET_OK;
}
uint8_t set_jesdProtocol(uint8_t afeId, uint8_t jesdProtocol)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].jesdProtocol = jesdProtocol;
    return RET_OK;
}
uint8_t get_jesdProtocol(uint8_t afeId, uint8_t *jesdProtocol)
{
    AFE_ID_VALIDITY();

    *jesdProtocol = systemParams[afeId].jesdProtocol;
    return RET_OK;
}

uint8_t set_spiInUseForPllAccess(uint8_t afeId, uint8_t spiInUseForPllAccess)
{
    AFE_ID_VALIDITY();

    systemParams[afeId].spiInUseForPllAccess = spiInUseForPllAccess;
    return RET_OK;
}
uint8_t get_spiInUseForPllAccess(uint8_t afeId, uint8_t *spiInUseForPllAccess)
{
    AFE_ID_VALIDITY();

    *spiInUseForPllAccess = systemParams[afeId].spiInUseForPllAccess;
    return RET_OK;
}

#endif