// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

// Registers a single automation test. The string is its hierarchical name in the
// Session Frontend / `Automation RunTests` command. Flags say where it can run.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGenomeSerializationRoundTripTest,
	"EvolutionArena.Genetics.GenomeSerializationRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGenomeSerializationRoundTripTest::RunTest(const FString& /*Parameters*/)
{
	// 1) Build a representative creature with a populated genome.
	FCreature Original;
	Original.CreatureId = FGuid::NewGuid();
	Original.Name = TEXT("Test Specimen");
	Original.Rarity = ERarity::Epic;
	Original.EvolutionStage = 2;
	Original.CreatedUtc = FDateTime(2026, 6, 13, 12, 0, 0);
	Original.BreedCooldownUntil = FDateTime(2026, 6, 14, 12, 0, 0);

	FGene Gene;
	Gene.GeneDefId = TEXT("Gene_Vigor");
	Gene.AlleleA = 3;
	Gene.AlleleB = 7;
	Original.Genome.Genes.Add(Gene);

	FPartAssignment Part;
	Part.Slot = EBodyPartSlot::Torso;
	Part.PartId = TEXT("Part_ArmoredShell");
	Original.Genome.Parts.Add(Part);

	Original.Genome.Mutations.Add(TEXT("Mut_Bioluminescence"));

	FStatBlock Stats;
	Stats.Health = 120;
	Stats.Attack = 45;
	Stats.Defense = 30;
	Stats.Speed = 18;
	Original.Genome.DerivedStats = Stats;

	// 2) Serialize the struct's reflected properties to a byte buffer, then read
	//    them back into a fresh creature. This is the same machinery the save
	//    system will use, which is why the structs are USTRUCTs with UPROPERTYs.
	TArray<uint8> Bytes;
	FMemoryWriter Writer(Bytes, /*bIsPersistent=*/ true);
	FCreature::StaticStruct()->SerializeBin(Writer, &Original);

	FCreature Restored;
	FMemoryReader Reader(Bytes, /*bIsPersistent=*/ true);
	FCreature::StaticStruct()->SerializeBin(Reader, &Restored);

	// 3) Every field must survive the round-trip unchanged.
	TestTrue(TEXT("CreatureId preserved"), Restored.CreatureId == Original.CreatureId);
	TestEqual(TEXT("Name preserved"), Restored.Name, Original.Name);
	TestTrue(TEXT("Rarity preserved"), Restored.Rarity == Original.Rarity);
	TestEqual(TEXT("EvolutionStage preserved"), Restored.EvolutionStage, Original.EvolutionStage);
	TestTrue(TEXT("CreatedUtc preserved"), Restored.CreatedUtc == Original.CreatedUtc);
	TestTrue(TEXT("BreedCooldownUntil preserved"), Restored.BreedCooldownUntil == Original.BreedCooldownUntil);

	TestEqual(TEXT("Gene count preserved"), Restored.Genome.Genes.Num(), Original.Genome.Genes.Num());
	TestTrue(TEXT("Gene preserved"),
		Restored.Genome.Genes.Num() == 1 && Restored.Genome.Genes[0] == Original.Genome.Genes[0]);

	TestEqual(TEXT("Part count preserved"), Restored.Genome.Parts.Num(), Original.Genome.Parts.Num());
	TestTrue(TEXT("Part preserved"),
		Restored.Genome.Parts.Num() == 1 && Restored.Genome.Parts[0] == Original.Genome.Parts[0]);

	TestEqual(TEXT("Mutation count preserved"), Restored.Genome.Mutations.Num(), Original.Genome.Mutations.Num());
	TestTrue(TEXT("DerivedStats preserved"), Restored.Genome.DerivedStats == Original.Genome.DerivedStats);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
