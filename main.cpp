#include <SKSE.h>
#include <SKSE/DebugLog.h>
#include <SKSE/GameData.h>
#include <SKSE/GameExtraData.h>
#include <SKSE/GameForms.h>
#include <SKSE/GameMenus.h>
#include <SKSE/GameObjects.h>
#include <SKSE/GameRTTI.h>
#include <SKSE/GameReferences.h>
#include <SKSE/HookUtil.h>
#include <SKSE/PapyrusFunctions.h>
#include <SKSE/PluginAPI.h>
#include <SKSE/SafeWrite.h>
#include <SKSE/Version.h>

#include <algorithm>
#include <string>
#include <vector>

#include "date.h"

static const char* g_pluginName	   = "BestInClass++";
const UInt32	   g_pluginVersion = 0x00030400;

class Plugin_BestInClassPP_plugin : public SKSEPlugin, public BSTEventSink<MenuOpenCloseEvent>
{
	private:
	/*
		bestItem Structure
		The bestItem struct is part of an attempt at optimizing the code.
		Instead of accessing the stored item to get its value for comparison
		we save it separately with it.
	*/
	struct bestItem
	{
		StandardItemData* itemData;
		float			  comparisonValue;
	};

	/*
		bestItem Index Assignment
		The bestItemArray assigns an item type to an index

		Armors
		0	LightArmor		| 5	HeavyArmor
		1	LightBoots		| 6	HeavyBoots
		2	LightGauntlets	| 7	HeavyGauntlets
		3	LightHelmet		| 8	HeavyHelmet
		4	LightShield		| 9	HeavyShield

		Weapons
		10	1HSword			| 14	2HGreatsword
		11	1HWarAxe		| 15	2HBattleaxe
		12	1HMace			| 16	2HWarhammer ? No kType defined
		13	1HDagger
	*/
	bestItem bestItemArray[16];

	void LogMessage(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		char inputBuf[1024];
		vsprintf_s(inputBuf, fmt, args);
		va_end(args);

		std::string date = date::format("%F %T", std::chrono::system_clock::now());

		_MESSAGE("[%s] %s", date.c_str(), inputBuf);
	}

	public:
	Plugin_BestInClassPP_plugin() {}

	virtual bool InitInstance() override
	{
		LogMessage("Initializing %s", g_pluginName);
		if(!Requires(kSKSEVersion_1_7_1, SKSEPapyrusInterface::Version_1)) {
			LogMessage("ERROR: Your SKSE Version is too old");
			return false;
		}

		SetName(g_pluginName);
		SetVersion(g_pluginVersion);

		{
			auto  v		= g_pluginVersion;
			UInt8 main	= v >> 0x18;
			UInt8 major = v >> 0x10;
			UInt8 minor = v >> 0x08;
			LogMessage("Current version is %d.%d.%d", main, major, minor);
		}

		return true;
	}

	virtual bool OnLoad() override
	{
		LogMessage("Adding menu event sinks");
		MenuManager* mm = MenuManager::GetSingleton();
		mm->BSTEventSource<MenuOpenCloseEvent>::AddEventSink(this);

		LogMessage("Disabling vanilla bestInClass function at memory location %X", 0x008684A0);
		SafeWrite8(0x008684A0, 0xC3);

		return true;
	}

	virtual EventResult ReceiveEvent(MenuOpenCloseEvent* evn, BSTEventSource<MenuOpenCloseEvent>* src) override
	{
		UIStringHolder* holder = UIStringHolder::GetSingleton();

		if(evn->opening && (evn->menuName == holder->inventoryMenu || evn->menuName == holder->barterMenu || evn->menuName == holder->containerMenu)) {
			LogMessage("Menu \"%s\" has been opened", evn->menuName);

			MenuManager* mm	  = MenuManager::GetSingleton();
			IMenu*		 menu = mm->GetMenu(holder->inventoryMenu);

			if(menu && holder->inventoryMenu) {
				InventoryMenu*				 invMenu	   = static_cast<InventoryMenu*>(menu);
				BSTArray<StandardItemData*>& itemDataArray = invMenu->inventoryData->items;

				/*
				for(bestItem item : bestItemArray) {
					if(item.itemData) {
						item.itemData		 = NULL;
						item.comparisonValue = 0;
					}
				}
				*/

				if(!itemDataArray.empty()) {
					for(StandardItemData* itemData : itemDataArray) {
						LogMessage("Item \"%s\" is being processed", itemData->GetName());
						TESForm* baseForm = itemData->objDesc->baseForm;

						if(baseForm) {
							LogMessage("Current item \"%s\" has baseFormID %X", itemData->GetName(), baseForm->GetFormID());
							if(baseForm->IsWeapon()) {
								TESObjectWEAP* objWEAP = DYNAMIC_CAST<TESObjectWEAP*>(baseForm);

								if(objWEAP) {
									LogMessage("Weapon %s has type %d", itemData->GetName(), objWEAP->gameData.type);
									switch(objWEAP->gameData.type) {
										case 1: // Sword
											if(bestItemArray[10].itemData) {
												if(objWEAP->attackDamage > bestItemArray[10].comparisonValue) {
													bestItemArray[10].itemData		  = itemData;
													bestItemArray[10].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[10].itemData		  = itemData;
												bestItemArray[10].comparisonValue = objWEAP->attackDamage;
											}
											break;
										case 2: // Dagger
											if(bestItemArray[13].itemData) {
												if(objWEAP->attackDamage > bestItemArray[10].comparisonValue) {
													bestItemArray[13].itemData		  = itemData;
													bestItemArray[13].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[13].itemData		  = itemData;
												bestItemArray[13].comparisonValue = objWEAP->attackDamage;
											}
											break;
										case 3: // Axe
											if(bestItemArray[11].itemData) {
												if(objWEAP->attackDamage > bestItemArray[11].comparisonValue) {
													bestItemArray[11].itemData		  = itemData;
													bestItemArray[11].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[11].itemData		  = itemData;
												bestItemArray[11].comparisonValue = objWEAP->attackDamage;
											}
											break;
										case 4: // Mace
											if(bestItemArray[12].itemData) {
												if(objWEAP->attackDamage > bestItemArray[12].comparisonValue) {
													bestItemArray[12].itemData		  = itemData;
													bestItemArray[12].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[12].itemData		  = itemData;
												bestItemArray[12].comparisonValue = objWEAP->attackDamage;
											}
											break;
										case 5: // Greatsword
											if(bestItemArray[14].itemData) {
												if(objWEAP->attackDamage > bestItemArray[14].comparisonValue) {
													bestItemArray[14].itemData		  = itemData;
													bestItemArray[14].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[14].itemData		  = itemData;
												bestItemArray[14].comparisonValue = objWEAP->attackDamage;
											}
											break;
										case 6: // Battleaxe
											if(bestItemArray[15].itemData) {
												if(objWEAP->attackDamage > bestItemArray[15].comparisonValue) {
													bestItemArray[15].itemData		  = itemData;
													bestItemArray[15].comparisonValue = objWEAP->attackDamage;
												}
											} else {
												bestItemArray[15].itemData		  = itemData;
												bestItemArray[15].comparisonValue = objWEAP->attackDamage;
											}
											break;
										default: break;
									}
								}
							} else if(baseForm->IsArmor()) {
								TESObjectARMO* objARMO = DYNAMIC_CAST<TESObjectARMO*>(baseForm);

								if(objARMO) {
									LogMessage("Armor piece %s occupies slot mask %d", itemData->GetName(), objARMO->GetSlotMask());
									if(objARMO->IsLightArmor()) {
										if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Body)) {
											// Armor
											if(bestItemArray[0].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[0].comparisonValue) {
													bestItemArray[0].itemData		 = itemData;
													bestItemArray[0].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[0].itemData		 = itemData;
												bestItemArray[0].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Feet)) {
											// Boots
											if(bestItemArray[1].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[1].comparisonValue) {
													bestItemArray[1].itemData		 = itemData;
													bestItemArray[1].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[1].itemData		 = itemData;
												bestItemArray[1].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hands)) {
											// Gauntlets
											if(bestItemArray[2].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[2].comparisonValue) {
													bestItemArray[2].itemData		 = itemData;
													bestItemArray[2].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[2].itemData		 = itemData;
												bestItemArray[2].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hair)) {
											// Helmet
											if(bestItemArray[3].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[3].comparisonValue) {
													bestItemArray[3].itemData		 = itemData;
													bestItemArray[3].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[3].itemData		 = itemData;
												bestItemArray[3].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Shield)) {
											// Shield
											if(bestItemArray[4].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[4].comparisonValue) {
													bestItemArray[4].itemData		 = itemData;
													bestItemArray[4].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[4].itemData		 = itemData;
												bestItemArray[4].comparisonValue = objARMO->armorValTimes100;
											}
										}
									} else if(objARMO->IsHeavyArmor()) {
										if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Body)) {
											// Armor
											if(bestItemArray[5].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[5].comparisonValue) {
													bestItemArray[5].itemData		 = itemData;
													bestItemArray[5].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[5].itemData		 = itemData;
												bestItemArray[5].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Feet)) {
											// Boots
											if(bestItemArray[6].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[6].comparisonValue) {
													bestItemArray[6].itemData		 = itemData;
													bestItemArray[6].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[6].itemData		 = itemData;
												bestItemArray[6].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hands)) {
											// Gauntlets
											if(bestItemArray[7].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[7].comparisonValue) {
													bestItemArray[7].itemData		 = itemData;
													bestItemArray[7].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[7].itemData		 = itemData;
												bestItemArray[7].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hair)) {
											// Helmet
											if(bestItemArray[8].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[8].comparisonValue) {
													bestItemArray[8].itemData		 = itemData;
													bestItemArray[8].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[8].itemData		 = itemData;
												bestItemArray[8].comparisonValue = objARMO->armorValTimes100;
											}
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Shield)) {
											// Shield
											if(bestItemArray[9].itemData) {
												if(objARMO->armorValTimes100 > bestItemArray[9].comparisonValue) {
													bestItemArray[9].itemData		 = itemData;
													bestItemArray[9].comparisonValue = objARMO->armorValTimes100;
												}
											} else {
												bestItemArray[9].itemData		 = itemData;
												bestItemArray[9].comparisonValue = objARMO->armorValTimes100;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		} else {
			return kEvent_Continue;
		}

		// By setting the member "bestInClass" to true,
		// we tell the UI to mark the item
		for(bestItem item : bestItemArray) {
			if(item.itemData) {
				LogMessage("The best item of type is %s", item.itemData->GetName());

				GFxValue* gfxVal = static_cast<GFxValue*>(&item.itemData->fxValue);
				gfxVal->SetMember("bestInClass", true);
			}
		}

		return kEvent_Continue;
	}

	virtual void OnModLoaded() override {}
} thePlugin;
