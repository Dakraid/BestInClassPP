#include "main.h"
#include "constants.h"

class Plugin_BestInClassPP_OpenHandle : public BSTEventSink<MenuOpenCloseEvent>, public Plugin_BestInClassPP_Proc
{
	public:
	Plugin_BestInClassPP_OpenHandle() {}

	virtual EventResult ReceiveEvent(MenuOpenCloseEvent* evn, BSTEventSource<MenuOpenCloseEvent>* src) override
	{
		UIStringHolder* holder = UIStringHolder::GetSingleton();

		if(evn->opening && (evn->menuName == holder->inventoryMenu || evn->menuName == holder->barterMenu || evn->menuName == holder->containerMenu)) {
			LogMessage("Menu \"%s\" has been opened", evn->menuName);

			MenuManager* mm = MenuManager::GetSingleton();

			if(evn->menuName == holder->inventoryMenu) {
				IMenu*						 menu		   = mm->GetMenu(holder->inventoryMenu);
				InventoryMenu*				 invMenu	   = dynamic_cast<InventoryMenu*>(menu);
				BSTArray<StandardItemData*>& itemDataArray = invMenu->inventoryData->items;

				LogMessage("EVENT: InventoryMenu is at address %08X", invMenu);
				ProcessInventory(itemDataArray);

			} else if(evn->menuName == holder->barterMenu) {
				IMenu*						 menu		   = mm->GetMenu(holder->barterMenu);
				BarterMenu*					 barMenu	   = dynamic_cast<BarterMenu*>(menu);
				BSTArray<StandardItemData*>& itemDataArray = barMenu->barterInventoryData->items;

				LogMessage("EVENT: BarterMenu is at address %08X", barMenu);
				ProcessInventory(itemDataArray);
			} else if(evn->menuName == holder->containerMenu) {
				IMenu*						 menu		   = mm->GetMenu(holder->containerMenu);
				ContainerMenu*				 conMenu	   = dynamic_cast<ContainerMenu*>(menu);
				BSTArray<StandardItemData*>& itemDataArray = conMenu->inventoryData->items;

				LogMessage("EVENT: ContainerMenu is at address %08X", conMenu);
				ProcessInventory(itemDataArray);
			}

			return kEvent_Continue;
		} else {
			return kEvent_Continue;
		}
	}
};

class Plugin_BestInClassPP_SKSE : public SKSEPlugin, public Plugin_BestInClassPP_Proc
{
	Plugin_BestInClassPP_OpenHandle OpenHandler;

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

			char* rType;

			switch(g_pluginRelease) {
				case Devel: rType = "Development"; break;
				case Testing: rType = "Testing"; break;
				case Candidate: rType = "Candidate"; break;
				case Release: rType = "Release"; break;
			}
			LogMessage("Current version is %d.%d.%d (%s)", main, major, minor, rType);
		}

		return true;
	}

	virtual bool OnLoad() override
	{
		LogMessage("Registering for SKSE events");

		MenuManager* mm = MenuManager::GetSingleton();
		mm->BSTEventSource<MenuOpenCloseEvent>::AddEventSink(&OpenHandler);

		// LogMessage("Disabling vanilla bestInClass function at memory location %08X", 0x008684A0);
		// SafeWrite8(0x008684A0, 0xC3);

		LogMessage("Hooking the vanilla function at %08X", 0x008684A0);
		InstallHook();

		return true;
	}

	virtual void OnModLoaded() override {}
} thePlugin;
