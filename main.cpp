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
#include <vector>

#include "date.h"

static const char* g_pluginName = "BestInClass++";
const UInt32 g_pluginVersion = 108;

class Plugin_BestInClassPP_plugin : public SKSEPlugin,
                                    public BSTEventSink<MenuOpenCloseEvent> {
private:
    /* bestItem Structure
  The bestItem struct is part of an attempt at optimizing the code.
  Instead of accessing the stored item to get its value for comparison
  we save it separately with it.
  */
    struct bestItem {
        StandardItemData* itemData;
        float comparisonValue;
    };

    /* bestItem Index Assignment
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
  12	1HMace			| 16	2HWarhammer
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

        std::string timestamp = date::format("%F %T", std::chrono::system_clock::now());
        std::string message = inputBuf;
        std::string output = "[" + timestamp + "] " + message;

        _MESSAGE("%s", output);
    }

public:
    Plugin_BestInClassPP_plugin() { }

    virtual bool InitInstance() override
    {
        if (!Requires(kSKSEVersion_1_7_1, SKSEPapyrusInterface::Version_1)) {
            gLog << "ERROR: Your SKSE Version is too old." << std::endl;
            return false;
        }

        SetName(g_pluginName);
        SetVersion(g_pluginVersion);

        return true;
    }

    virtual bool OnLoad() override
    {
        SKSEPlugin::OnLoad();

        MenuManager* mm = MenuManager::GetSingleton();
        mm->BSTEventSource<MenuOpenCloseEvent>::AddEventSink(this);

        return true;
    }

    virtual EventResult ReceiveEvent(
        MenuOpenCloseEvent* evn,
        BSTEventSource<MenuOpenCloseEvent>* src) override
    {
        UIStringHolder* holder = UIStringHolder::GetSingleton();
        if (evn->opening && (evn->menuName == holder->inventoryMenu || evn->menuName == holder->barterMenu || evn->menuName == holder->containerMenu)) {
            _MESSAGE("Menu \"%s\" has been opened", evn->menuName);

            MenuManager* mm = MenuManager::GetSingleton();
            IMenu* menu = mm->GetMenu(holder->inventoryMenu);

            if (menu) {
                InventoryMenu* invMenu = static_cast<InventoryMenu*>(menu);
                BSTArray<StandardItemData*>& itemDataArray = invMenu->inventoryData->items;

                if (!itemDataArray.empty()) {
                    for (StandardItemData* itemData : itemDataArray) {
                        _MESSAGE("Current Item: %s", itemData->GetName());
                        TESForm* baseForm = itemData->objDesc->baseForm;
                        if (baseForm->IsWeapon()) {
                            _MESSAGE("Current Weapon: %s", itemData->GetName());
                            TESObjectWEAP* objWEAP = DYNAMIC_CAST<TESObjectWEAP*>(baseForm);

                            if (objWEAP) {
                                switch (objWEAP->gameData.type) {
                                case 1:
                                    if (bestItemArray[10].itemData) {
                                        if (objWEAP->attackDamage > bestItemArray[10].comparisonValue) {
                                            bestItemArray[10].itemData = itemData;
                                            bestItemArray[10].comparisonValue = objWEAP->attackDamage;
                                        }
                                    } else {
                                        bestItemArray[10].itemData = itemData;
                                        bestItemArray[10].comparisonValue = objWEAP->attackDamage;
                                    }
                                    break;
                                default:
                                    break;
                                }
                            }
                        } else if (baseForm->IsArmor()) {
                            _MESSAGE("Current Armor: %s", itemData->GetName());
                            // TODO
                        }
                    }
                }
            }
        } else {
            return kEvent_Continue;
        }

        if (bestItemArray[10].itemData) {
            _MESSAGE("The best 1H Sword is %s",
                bestItemArray[10].itemData->GetName());
        }

        return kEvent_Continue;
    }

    virtual void
    OnModLoaded() override
    {
    }
} thePlugin;
