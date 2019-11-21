#include "processor.h"

/*
bestItem/bestValue Array Index Assignment
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
12	1HMace			| 16	Bow
13	1HDagger		| 17	Crossbow

Ammunition
18	Arrow			| 19	Bolts

Clothing
20 ClothingBody		| 22 ClothingGloves
21 ClothingShoes	| 23 ClothingHat
*/

void Plugin_BestInClassPP_Proc::LogMessage(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char inputBuf[1024];
	vsprintf_s(inputBuf, fmt, args);
	va_end(args);

	std::string date = date::format("%F %T", std::chrono::system_clock::now());

	_MESSAGE("[%s] %s", date.c_str(), inputBuf);
}

void Plugin_BestInClassPP_Proc::ProcessInventory(BSTArray<StandardItemData*>& itemDataArray)
{
	LogMessage("The itemDataArray is at address %08X", &itemDataArray);

	std::fill_n(bestItemArray, arraySize, nullptr);
	std::fill_n(bestValueArray, arraySize, 0);

	if(!itemDataArray.empty()) {
		for(StandardItemData* itemData : itemDataArray) {
			TESForm* baseForm = itemData->objDesc->baseForm;

			if(baseForm) {
				int targetIndex = -1;
				if(baseForm->IsWeapon()) {
					LogMessage("Item %s has baseFormID %08X", itemData->GetName(), baseForm->GetFormID());
					TESObjectWEAP* objWEAP = DYNAMIC_CAST<TESObjectWEAP*>(baseForm);

					if(objWEAP) {
						LogMessage("Weapon %s has type %d", itemData->GetName(), objWEAP->gameData.type);
						switch(objWEAP->type()) {
							case TESObjectWEAP::GameData::kType_1HS:
							case TESObjectWEAP::GameData::kType_OneHandSword: // Sword
								targetIndex = 10;
								break;
							case TESObjectWEAP::GameData::kType_1HD:
							case TESObjectWEAP::GameData::kType_OneHandDagger: // Dagger
								targetIndex = 13;
								break;
							case TESObjectWEAP::GameData::kType_1HA:
							case TESObjectWEAP::GameData::kType_OneHandAxe: // Axe
								targetIndex = 11;
								break;
							case TESObjectWEAP::GameData::kType_1HM:
							case TESObjectWEAP::GameData::kType_OneHandMace: // Mace
								targetIndex = 12;
								break;
							case TESObjectWEAP::GameData::kType_2HS:
							case TESObjectWEAP::GameData::kType_TwoHandSword: // Greatsword
								targetIndex = 14;
								break;
							case TESObjectWEAP::GameData::kType_2HA:
							case TESObjectWEAP::GameData::kType_TwoHandAxe: // Battleaxe
								targetIndex = 15;
								break;
							case TESObjectWEAP::GameData::kType_Bow2:
							case TESObjectWEAP::GameData::kType_Bow: // Bow
								targetIndex = 16;
								break;
							case TESObjectWEAP::GameData::kType_CBow:
							case TESObjectWEAP::GameData::kType_CrossBow: // Crossbow
								targetIndex = 17;
								break;
							default: targetIndex = -1; break;
						}
						if(targetIndex != -1) {
							if(bestItemArray[targetIndex]) { LogMessage("		Last Item: %s with %d damage", bestItemArray[targetIndex]->GetName(), bestValueArray[targetIndex]); }
							if(objWEAP->attackDamage > bestValueArray[targetIndex]) {
								bestItemArray[targetIndex]	= itemData;
								bestValueArray[targetIndex] = objWEAP->attackDamage;
							}
						}
					}
				} else if(baseForm->IsArmor()) {
					LogMessage("Item %s has baseFormID %08X", itemData->GetName(), baseForm->GetFormID());
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
						} else {
							if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Body)) {
								// Body
								targetIndex = 20;
							} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Feet)) {
								// Shoes
								targetIndex = 21;
							} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hands)) {
								// Gloves
								targetIndex = 22;
							} else if(objARMO->HasPartOf(BGSBipedObjectForm::kPart_Hair)) {
								// Hat
								targetIndex = 23;
							} else {
								targetIndex = -1;
							}
						}
						if(targetIndex != -1) {
							if(objARMO->armorValTimes100 > bestValueArray[targetIndex]) {
								bestItemArray[targetIndex]	= itemData;
								bestValueArray[targetIndex] = objARMO->armorValTimes100;
							}
						}
					}
				} else if(baseForm->IsAmmo()) {
					LogMessage("Item %s has baseFormID %X", itemData->GetName(), baseForm->GetFormID());
					TESAmmo* tesAMMO = DYNAMIC_CAST<TESAmmo*>(baseForm);

					if(tesAMMO) {
						if(!tesAMMO->isBolt()) {
							targetIndex = 18;
						} else {
							targetIndex = 19;
						}

						if(targetIndex != -1) {
							if(tesAMMO->settings.damage > bestValueArray[targetIndex]) {
								bestItemArray[targetIndex]	= itemData;
								bestValueArray[targetIndex] = tesAMMO->settings.damage;
							}
						}
					}
				}
			}
		}
	}

	// By setting the member "bestInClass" to true,
	// we tell the UI to mark the item
	for(StandardItemData* itemData : bestItemArray) {
		if(itemData) {
			LogMessage("The best item of type is %s", itemData->GetName());

			LogMessage("The itemData is at address %08X", &itemData);
			LogMessage("The fxValue is at address %08X", &itemData->fxValue);
			itemData->fxValue.SetMember("bestInClass", true);
		}
	}

	LogMessage("The bestItemArray is at address %08X", &bestItemArray);
	LogMessage("Finished marking the best items");
};
