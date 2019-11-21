#include "hook.h"

void Hook_MarkBestInClass()
{
	Plugin_BestInClassPP_Proc Proc;

	UIStringHolder* holder = UIStringHolder::GetSingleton();
	MenuManager*	mm	   = MenuManager::GetSingleton();

	if(mm->IsMenuOpen(holder->inventoryMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->inventoryMenu);
		InventoryMenu*				 invMenu	   = dynamic_cast<InventoryMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = invMenu->inventoryData->items;

		Proc.LogMessage("HOOK: InventoryMenu is at address %08X", invMenu);
		Proc.ProcessInventory(itemDataArray);
	} else if(mm->IsMenuOpen(holder->barterMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->barterMenu);
		BarterMenu*					 barMenu	   = dynamic_cast<BarterMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = barMenu->barterInventoryData->items;

		Proc.LogMessage("HOOK: BarterMenu is at address %08X", barMenu);
		Proc.ProcessInventory(itemDataArray);
	} else if(mm->IsMenuOpen(holder->containerMenu)) {
		IMenu*						 menu		   = mm->GetMenu(holder->containerMenu);
		ContainerMenu*				 conMenu	   = dynamic_cast<ContainerMenu*>(menu);
		BSTArray<StandardItemData*>& itemDataArray = conMenu->inventoryData->items;

		Proc.LogMessage("HOOK: ContainerMenu is at address %08X", conMenu);
		Proc.ProcessInventory(itemDataArray);
	}
}

void InstallHook()
{
	constexpr std::uintptr_t fnAddress = 0x008684A0;

	WriteRelJump(fnAddress, reinterpret_cast<UInt32>(&Hook_MarkBestInClass));
}
