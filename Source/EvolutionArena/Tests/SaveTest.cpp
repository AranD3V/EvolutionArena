// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Save/EvoSaveGame.h"
#include "Save/SaveSubsystem.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	FCreature MakeCreature(const FString& Name, const ERarity Rarity)
	{
		FCreature Creature;
		Creature.CreatureId = FGuid::NewGuid();
		Creature.Name = Name;
		Creature.Rarity = Rarity;

		FGene Gene;
		Gene.GeneDefId = TEXT("Gene_Vigor");
		Gene.AlleleA = 2;
		Gene.AlleleB = 5;
		Creature.Genome.Genes.Add(Gene);

		return Creature;
	}
}

// Save -> bytes -> load reproduces the whole collection. Load-after-save is the
// same path a fresh app launch takes, so this is the "survives restart" proof.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSaveRoundTripTest,
	"EvolutionArena.Save.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSaveRoundTripTest::RunTest(const FString&)
{
	UEvoSaveGame* Save = Cast<UEvoSaveGame>(UGameplayStatics::CreateSaveGameObject(UEvoSaveGame::StaticClass()));
	if (!TestNotNull(TEXT("Created save object"), Save))
	{
		return false;
	}

	Save->Data.PlayerId = TEXT("player-1");
	Save->Data.DisplayName = TEXT("Tester");
	Save->Data.Level = 7;
	Save->Data.SoftCurrency = 1234;
	Save->Data.Creatures.Add(MakeCreature(TEXT("Alpha"), ERarity::Rare));
	Save->Data.Creatures.Add(MakeCreature(TEXT("Beta"), ERarity::Epic));

	TArray<uint8> Bytes;
	TestTrue(TEXT("SaveGameToMemory succeeds"), UGameplayStatics::SaveGameToMemory(Save, Bytes));
	TestTrue(TEXT("Produced non-empty bytes"), Bytes.Num() > 0);

	UEvoSaveGame* Loaded = Cast<UEvoSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
	if (!TestNotNull(TEXT("Loaded save object"), Loaded))
	{
		return false;
	}

	TestEqual(TEXT("PlayerId preserved"), Loaded->Data.PlayerId, Save->Data.PlayerId);
	TestEqual(TEXT("Level preserved"), Loaded->Data.Level, 7);
	TestEqual(TEXT("Currency preserved"), Loaded->Data.SoftCurrency, 1234);
	TestEqual(TEXT("Collection size preserved"), Loaded->Data.Creatures.Num(), 2);
	TestEqual(TEXT("Creature name preserved"), Loaded->Data.Creatures[0].Name, FString(TEXT("Alpha")));
	TestTrue(TEXT("Creature rarity preserved"), Loaded->Data.Creatures[1].Rarity == ERarity::Epic);
	TestEqual(TEXT("Nested genome preserved"), Loaded->Data.Creatures[0].Genome.Genes.Num(), 1);

	return true;
}

// Atomic write commits the latest bytes, overwrites prior content, and leaves no
// temp file behind.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSaveAtomicWriteTest,
	"EvolutionArena.Save.AtomicWrite",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSaveAtomicWriteTest::RunTest(const FString&)
{
	const FString Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("AtomicTest.bin"));
	IFileManager::Get().Delete(*Path, false, true, true);

	const TArray<uint8> First = { 1, 2, 3, 4 };
	TestTrue(TEXT("First write"), USaveSubsystem::AtomicWriteFile(Path, First));

	const TArray<uint8> Second = { 9, 8, 7 };
	TestTrue(TEXT("Second write"), USaveSubsystem::AtomicWriteFile(Path, Second));

	TArray<uint8> ReadBack;
	TestTrue(TEXT("Read back"), FFileHelper::LoadFileToArray(ReadBack, *Path));
	TestEqual(TEXT("Latest content committed"), ReadBack.Num(), 3);
	TestTrue(TEXT("No temp file left behind"), !IFileManager::Get().FileExists(*(Path + TEXT(".tmp"))));

	IFileManager::Get().Delete(*Path, false, true, true);
	return true;
}

// The migration hook upgrades an older save's version while preserving its data.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSaveMigrationTest,
	"EvolutionArena.Save.Migration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSaveMigrationTest::RunTest(const FString&)
{
	FPlayerSave Data;
	Data.SaveVersion = 0; // simulate a save written by an older build
	Data.Creatures.Add(MakeCreature(TEXT("Legacy"), ERarity::Common));

	USaveSubsystem::MigrateIfNeeded(Data);

	TestEqual(TEXT("Version bumped to current"), Data.SaveVersion, USaveSubsystem::CurrentSaveVersion);
	TestEqual(TEXT("Data preserved through migration"), Data.Creatures.Num(), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
