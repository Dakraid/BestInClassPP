#include "hook.h"

void Hook_MarkBestInClass()
{
	Plugin_BestInClassPP_Proc proc;

	UIStringHolder* holder = UIStringHolder::GetSingleton();
	MenuManager*	mm	   = MenuManager::GetSingleton();

	if(mm->IsMenuOpen(holder->inventoryMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->inventoryMenu);
		InventoryMenu*				 invMenu	   = dynamic_cast<InventoryMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = invMenu->inventoryData->items;

		proc.LogMessage("HOOK: InventoryMenu is at address %08X", invMenu);
		proc.ProcessInventory(itemDataArray);
	} else if(mm->IsMenuOpen(holder->barterMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->barterMenu);
		BarterMenu*					 barMenu	   = dynamic_cast<BarterMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = barMenu->barterInventoryData->items;

		proc.LogMessage("HOOK: BarterMenu is at address %08X", barMenu);
		proc.ProcessInventory(itemDataArray);
	} else if(mm->IsMenuOpen(holder->containerMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->containerMenu);
		ContainerMenu*				 conMenu	   = dynamic_cast<ContainerMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = conMenu->inventoryData->items;

		proc.LogMessage("HOOK: ContainerMenu is at address %08X", conMenu);
		proc.ProcessInventory(itemDataArray);
	}

	return;
}

void InstallHook()
{
	constexpr std::uintptr_t fnAddress = 0x008684A0;

	WriteRelJump(fnAddress, reinterpret_cast<UInt32>(&Hook_MarkBestInClass));
}
