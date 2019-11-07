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
const UInt32	   g_pluginVersion = 0x00030500;

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
		LogMessage("Registering for SKSE events");

		MenuManager* mm = MenuManager::GetSingleton();
		mm->BSTEventSource<MenuOpenCloseEvent>::AddEventSink(this);

		LogMessage("Disabling vanilla bestInClass function at memory location %X", 0x008684A0);
		SafeWrite8(0x008684A0, 0xC3);

		std::fill_n(bestItemArray, 16, bestItem{NULL, 0});

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

				if(!itemDataArray.empty()) {
					for(StandardItemData* itemData : itemDataArray) {
						// LogMessage("Item \"%s\" is being processed", itemData->GetName());
						TESForm* baseForm = itemData->objDesc->baseForm;

						if(baseForm) {
							LogMessage("Current item \"%s\" has baseFormID %X", itemData->GetName(), baseForm->GetFormID());
							int targetIndex = -1;
							if(baseForm->IsWeapon()) {
								TESObjectWEAP* objWEAP = DYNAMIC_CAST<TESObjectWEAP*>(baseForm);

								if(objWEAP) {
									LogMessage("Weapon %s has type %d", itemData->GetName(), objWEAP->gameData.type);
									switch(objWEAP->type()) {
										case TESObjectWEAP::GameData::kType_OneHandSword: // Sword
											targetIndex = 10;
										case TESObjectWEAP::GameData::kType_OneHandDagger: // Dagger
											targetIndex = 13;
											break;
										case TESObjectWEAP::GameData::kType_OneHandAxe: // Axe
											targetIndex = 11;
											break;
										case TESObjectWEAP::GameData::kType_OneHandMace: // Mace
											targetIndex = 12;
											break;
										case TESObjectWEAP::GameData::kType_TwoHandSword: // Greatsword
											targetIndex = 14;
											break;
										case TESObjectWEAP::GameData::kType_TwoHandAxe: // Battleaxe
											targetIndex = 15;
											break;
										default: targetIndex = -1; break;
									}
									if(targetIndex != -1) {
										if(bestItemArray[targetIndex].itemData) {
											if(objWEAP->attackDamage > bestItemArray[targetIndex].comparisonValue) {
												bestItemArray[targetIndex].itemData		   = itemData;
												bestItemArray[targetIndex].comparisonValue = objWEAP->attackDamage;
											}
										} else {
											bestItemArray[targetIndex].itemData		   = itemData;
											bestItemArray[targetIndex].comparisonValue = objWEAP->attackDamage;
										}
									}
								}
							} else if(baseForm->IsArmor()) {
								TESObjectARMO* objARMO = DYNAMIC_CAST<TESObjectARMO*>(baseForm);

								if(objARMO) {
									LogMessage("Armor piece %s occupies slot mask %d", itemData->GetName(), objARMO->GetSlotMask());
									if(objARMO->IsLightArmor()) {
										if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Body)) {
											// Armor
											targetIndex = 0;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Feet)) {
											// Boots
											targetIndex = 1;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hands)) {
											// Gauntlets
											targetIndex = 2;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hair)) {
											// Helmet
											targetIndex = 3;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Shield)) {
											// Shield
											targetIndex = 4;
										} else {
											targetIndex = -1;
										}
									} else if(objARMO->IsHeavyArmor()) {
										if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Body)) {
											// Armor
											targetIndex = 5;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Feet)) {
											// Boots
											targetIndex = 6;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hands)) {
											// Gauntlets
											targetIndex = 7;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hair)) {
											// Helmet
											targetIndex = 8;
										} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Shield)) {
											// Shield
											targetIndex = 9;
										} else {
											targetIndex = -1;
										}
									}
								}
								if(targetIndex != -1) {
									if(bestItemArray[targetIndex].itemData) {
										if(objARMO->armorValTimes100 > bestItemArray[targetIndex].comparisonValue) {
											bestItemArray[targetIndex].itemData		   = itemData;
											bestItemArray[targetIndex].comparisonValue = objARMO->armorValTimes100;
										}
									} else {
										bestItemArray[targetIndex].itemData		   = itemData;
										bestItemArray[targetIndex].comparisonValue = objARMO->armorValTimes100;
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
