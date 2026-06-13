// Copyright Evolution Arena. All Rights Reserved.

#include "Progression/ProgressionLibrary.h"
#include "Progression/ProgressionRows.h"
#include "Engine/DataTable.h"

int32 UProgressionLibrary::LevelForXp(int32 Xp)
{
	return 1 + FMath::Max(0, Xp) / XpPerLevel;
}

void UProgressionLibrary::AwardRankedResult(FPlayerSave& Save, bool bWin)
{
	Save.Xp += bWin ? WinXp : LossXp;
	Save.SoftCurrency += bWin ? WinCoins : LossCoins;
	Save.Level = LevelForXp(Save.Xp);
}

void UProgressionLibrary::ApplyUnlocks(FPlayerSave& Save, const UDataTable* UnlockTable)
{
	if (!UnlockTable)
	{
		return;
	}

	// Stable row order keeps the result deterministic (consistent with mutation rolls).
	TArray<FName> RowNames = UnlockTable->GetRowNames();
	RowNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	static const FString Context(TEXT("ApplyUnlocks"));
	for (const FName& RowName : RowNames)
	{
		const FProgressionUnlockDef* Def = UnlockTable->FindRow<FProgressionUnlockDef>(RowName, Context, /*bWarnIfMissing=*/ false);
		if (Def && Save.Level >= Def->RequiredLevel)
		{
			Save.EvolutionUnlocks.AddUnique(RowName);
		}
	}
}

bool UProgressionLibrary::IsUnlocked(const FPlayerSave& Save, FName UnlockId)
{
	return Save.EvolutionUnlocks.Contains(UnlockId);
}
