
#include <stdint.h>
#include<stdio.h>


#include "afe79xxTypes.h"
#include "afe79xxLog.h"
#include "afeCommonMacros.h"
#include "baseFunc.h"
#include "afeParameters.h"
#include "basicFunctions.h"
#include "hMacro.h"
#include "controls.h"
#include "calibrations.h"
#include "dsaAndNco.h"
#include "jesd.h"
#include "serDes.h"
#include "pap.h"
#include "agc.h"

int main(void)
{
	setAfeLogLvl(4);
	overrideTdd(0,0,0,0,0);
}

