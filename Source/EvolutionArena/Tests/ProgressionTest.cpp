// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Engine/DataTable.h"
#include "Progression/ProgressionLibrary.h"
#include "Progression/ProgressionRows.h"
#include "Save/PlayerSave.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	UDataTable* MakeUnlockTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FProgressionUnlockDef::StaticStruct();
		const FString Json = TEXT("[")
			TEXT("{ \"Name\": \"Unlock_L2\", \"RequiredLevel\": 2 },")
			TEXT("{ \"Name\": \"Unlock_L5\", \"RequiredLevel\": 5 }")
			TEXT("]");
		Table->CreateTableFromJSONString(Json);
		return Table;
	}
}

// XP curve and ranked rewards.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProgressionXpAndLevelTest,
	"EvolutionArena.Progression.XpAndLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProgressionXpAndLevelTest::RunTest(const FString&)
{
	TestEqual(TEXT("Level 1 at 0 XP"), UProgressionLibrary::LevelForXp(0), 1);
	TestEqual(TEXT("Level 1 at 99 XP"), UProgressionLibrary::LevelForXp(99), 1);
	TestEqual(TEXT("Level 2 at 100 XP"), UProgressionLibrary::LevelForXp(100), 2);
	TestEqual(TEXT("Level 6 at 500 XP"), UProgressionLibrary::LevelForXp(500), 6);

	FPlayerSave WinSave;
	UProgressionLibrary::AwardRankedResult(WinSave, /*bWin*/ true);
	TestTrue(TEXT("Win grants XP"), WinSave.Xp > 0);
	TestTrue(TEXT("Win grants coins"), WinSave.SoftCurrency > 0);
	TestEqual(TEXT("Level recomputed from XP"), WinSave.Level, UProgressionLibrary::LevelForXp(WinSave.Xp));

	FPlayerSave LossSave;
	UProgressionLibrary::AwardRankedResult(LossSave, /*bWin*/ false);
	TestTrue(TEXT("A win is worth more XP than a loss"), WinSave.Xp > LossSave.Xp);
	return true;
}

// Level-gated, data-driven unlocks accumulate without duplicates.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProgressionUnlocksTest,
	"EvolutionArena.Progression.Unlocks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProgressionUnlocksTest::RunTest(const FString&)
{
	UDataTable* Unlocks = MakeUnlockTable();

	FPlayerSave Save;
	Save.Level = 1;
	UProgressionLibrary::ApplyUnlocks(Save, Unlocks);
	TestFalse(TEXT("L2 unlock locked at level 1"), UProgressionLibrary::IsUnlocked(Save, TEXT("Unlock_L2")));

	Save.Level = 3;
	UProgressionLibrary::ApplyUnlocks(Save, Unlocks);
	TestTrue(TEXT("L2 unlock granted at level 3"), UProgressionLibrary::IsUnlocked(Save, TEXT("Unlock_L2")));
	TestFalse(TEXT("L5 unlock still locked"), UProgressionLibrary::IsUnlocked(Save, TEXT("Unlock_L5")));

	Save.Level = 5;
	UProgressionLibrary::ApplyUnlocks(Save, Unlocks);
	TestTrue(TEXT("L5 unlock granted at level 5"), UProgressionLibrary::IsUnlocked(Save, TEXT("Unlock_L5")));

	// Idempotent — re-applying doesn't duplicate.
	UProgressionLibrary::ApplyUnlocks(Save, Unlocks);
	int32 Count = 0;
	for (const FName& Id : Save.EvolutionUnlocks)
	{
		if (Id == TEXT("Unlock_L2"))
		{
			++Count;
		}
	}
	TestEqual(TEXT("No duplicate unlocks"), Count, 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
