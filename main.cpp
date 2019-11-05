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

#include "date.h"

#include <algorithm>
#include <vector>

static const char *g_pluginName = "BestInClass++";
const UInt32 g_pluginVersion = 100;

class Plugin_BestInClassPP_plugin : public SKSEPlugin,
                                    public BSTEventSink<MenuOpenCloseEvent> {
private:
  /* bestItem Structure
  The bestItem struct is part of an attempt at optimizing the code.
  Instead of accessing the stored item to get its value for comparison
  we save it separately with it.
  */
  struct bestItem {
    StandardItemData *itemData;
    float comparisonValue;
  };

  /* bestItem Index Assignment
  The bestItemArray assigns an item type to an index

  Armors
  0	LightArmor		| 5	HeavyArmor
  1	LightBoots		| 6	HeavyBoots
  2	LightGauntlets| 7	HeavyGauntlets
  3	LightHelmet		| 8	HeavyHelmet
  4	LightShield		| 9	HeavyShield

  Weapons
  10	1HSword			| 14	2HGreatsword
  11	1HWarAxe		| 15	2HBattleaxe
  12	1HMace			| 16	2HWarhammer
  13	1HDagger
  */
  bestItem bestItemArray[16];

  void LogMessage(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char formatBuf[1024];
    vsprintf_s(formatBuf, fmt, args);
    va_end(args);

    std::string currTime =
        date::format("%F %T", std::chrono::system_clock::now());

    _MESSAGE("[%s] %s", currTime, formatBuf);
  }

public:
  Plugin_BestInClassPP_plugin() {}

  virtual bool InitInstance() override {
    if (!Requires(kSKSEVersion_1_7_1, SKSEPapyrusInterface::Version_1)) {
      gLog << "ERROR: Your SKSE Version is too old." << std::endl;
      return false;
    }

    SetName(g_pluginName);
    SetVersion(g_pluginVersion);

    return true;
  }

  virtual bool OnLoad() override {
    SKSEPlugin::OnLoad();

    MenuManager *mm = MenuManager::GetSingleton();
    mm->BSTEventSource<MenuOpenCloseEvent>::AddEventSink(this);

    // TESDataHandler *dh = DataHandler::GetSingleton();

    return true;
  }

  virtual EventResult
  ReceiveEvent(MenuOpenCloseEvent *evn,
               BSTEventSource<MenuOpenCloseEvent> *src) override {
    UIStringHolder *holder = UIStringHolder::GetSingleton();
    if (evn->menuName != holder->console)
      return kEvent_Continue;

    if (evn->opening) {
      LogMessage("Menu \"%s\" has been opened", evn->menuName);

      MenuManager *mm = MenuManager::GetSingleton();
      IMenu *menu = mm->GetMenu(holder->inventoryMenu);

      if (menu) {
        InventoryMenu *invMenu = static_cast<InventoryMenu *>(menu);
        BSTArray<StandardItemData *> &itemDataArray =
            invMenu->inventoryData->items;

        LogMessage("Found %d items within the inventory array",
                   itemDataArray.GetSize());

        if (itemDataArray.GetSize() > 0) {
          for (StandardItemData *itemData : itemDataArray) {
            LogMessage("Current item: %s", itemData->GetName());

            if (itemData->objDesc) {
              TESForm *temp = itemData->objDesc->baseForm;
              if (temp->IsWeapon() || temp->IsArmor()) {
                LogMessage("Current type: %s", temp->GetTypeString());
              }
            }
          }
        }
      }
    }

    return kEvent_Continue;
  }

  virtual void OnModLoaded() override {}
} thePlugin;
