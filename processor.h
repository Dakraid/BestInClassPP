#pragma once

#include <SKSE.h>

#include <SKSE/DebugLog.h>
#include <SKSE/GameData.h>
#include <SKSE/GameExtraData.h>
#include <SKSE/GameForms.h>
#include <SKSE/GameMenus.h>
#include <SKSE/GameObjects.h>
#include <SKSE/GameRTTI.h>
#include <SKSE/GameReferences.h>

#include <algorithm>
#include <string>
#include <vector>

#include "date.h"

class Plugin_BestInClassPP_Proc
{
	public:
	void LogMessage(const char* fmt, ...);
	void ProcessInventory(BSTArray<StandardItemData*>& itemDataArray);

	private:
	static const int  arraySize = 23;
	StandardItemData* bestItemArray[arraySize];
	float			  bestValueArray[arraySize];
};
