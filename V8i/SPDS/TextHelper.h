#pragma once

#include <text/Textstyleapi.h> 
//
//#include <DgnFile.h>
#include <MicroStationAPI.h>
//#include <DgnModelRef.h>

#include "spds.h"

USING_NAMESPACE_BENTLEY;
USING_NAMESPACE_BENTLEY_USTN;
using namespace Bentley::Ustn::Text;

DgnFileP getActiveDgnFileP();
DgnFileR getActiveDgnFileR();

DgnModelRefP getActiveDgnModelP();

bool isTextStyleExists(MSWCharCP styleName);

DgnTextStylePtr getDefaultTextStylePtr();

DgnTextStylePtr getTextStylePtr(MSWCharCP styleName);

bool setActiveTextStyle(MSWCharCP styleName);
bool setActiveTextStyle(MSCharCP styleName);