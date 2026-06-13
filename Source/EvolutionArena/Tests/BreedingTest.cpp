// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Math/RandomStream.h"
#include "Engine/DataTable.h"
#include "Breeding/BreedingLibrary.h"
#include "Breeding/MutationLibrary.h"
#include "Genetics/GeneticsRows.h"
#include "Genetics/Genome.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	UDataTable* MakeGeneTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FGeneDef::StaticStruct();
		const FString Json = TEXT("[")
			TEXT("{ \"Name\": \"Gene_Vigor\", \"Type\": \"Stat\", \"Dominance\": \"Dominant\", \"StatMapping\": { \"Health\": 4, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } }")
			TEXT("]");
		Table->CreateTableFromJSONString(Json);
		return Table;
	}

	// Part table with one row per rarity tier so CalculateRarity is easy to target.
	UDataTable* MakePartTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FBodyPartDef::StaticStruct();
		const FString Json = TEXT("[")
			TEXT("{ \"Name\": \"Part_Common\",    \"Slot\": \"Torso\", \"Rarity\": \"Common\",    \"StatModifiers\": { \"Health\": 10, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } },")
			TEXT("{ \"Name\": \"Part_Rare\",      \"Slot\": \"Torso\", \"Rarity\": \"Rare\",      \"StatModifiers\": { \"Health\": 20, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } },")
			TEXT("{ \"Name\": \"Part_Legendary\", \"Slot\": \"Head\",  \"Rarity\": \"Legendary\", \"StatModifiers\": { \"Health\": 40, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } }")
			TEXT("]");
		Table->CreateTableFromJSONString(Json);
		return Table;
	}

	UDataTable* MakeMutationTable(const int32 ChancePermille)
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FMutationDef::StaticStruct();
		const FString Json = FString::Printf(
			TEXT("[ { \"Name\": \"Mut_Test\", \"ChancePermille\": %d, \"Rarity\": \"Rare\", \"AffectedType\": \"Stat\", \"Effect\": { \"Health\": 0, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } } ]"),
			ChancePermille);
		Table->CreateTableFromJSONString(Json);
		return Table;
	}

	FGene MakeGene(const FName Id, const uint8 A, const uint8 B)
	{
		FGene Gene;
		Gene.GeneDefId = Id;
		Gene.AlleleA = A;
		Gene.AlleleB = B;
		return Gene;
	}

	FPartAssignment MakePart(const EBodyPartSlot Slot, const FName Id)
	{
		FPartAssignment Part;
		Part.Slot = Slot;
		Part.PartId = Id;
		return Part;
	}

	bool GenomesEqual(const FGenome& A, const FGenome& B)
	{
		return A.Genes == B.Genes
			&& A.Parts == B.Parts
			&& A.Mutations == B.Mutations
			&& A.DerivedStats == B.DerivedStats;
	}
}

// Offspring alleles come from the correct parent's pair; parts come from a parent.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingInheritanceTest,
	"EvolutionArena.Breeding.Inheritance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingInheritanceTest::RunTest(const FString&)
{
	UDataTable* Genes = MakeGeneTable();
	UDataTable* Parts = MakePartTable();
	UDataTable* Mutations = MakeMutationTable(0); // never mutate, to isolate inheritance

	// Homozygous parents make the expected alleles independent of the coin flips:
	// A always contributes 1, B always contributes 9.
	FGenome ParentA;
	ParentA.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 1, 1));
	ParentA.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_Common")));

	FGenome ParentB;
	ParentB.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 9, 9));
	ParentB.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_Rare")));

	FRandomStream Stream(12345);
	const FGenome Child = UBreedingLibrary::BreedGenomes(ParentA, ParentB, Stream, Genes, Parts, Mutations);

	if (!TestEqual(TEXT("One gene locus"), Child.Genes.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("AlleleA inherited from parent A"), (int32)Child.Genes[0].AlleleA, 1);
	TestEqual(TEXT("AlleleB inherited from parent B"), (int32)Child.Genes[0].AlleleB, 9);

	TestEqual(TEXT("One part slot"), Child.Parts.Num(), 1);
	const FName PartId = Child.Parts[0].PartId;
	TestTrue(TEXT("Part came from a parent"), PartId == TEXT("Part_Common") || PartId == TEXT("Part_Rare"));

	TestEqual(TEXT("No mutations at 0 chance"), Child.Mutations.Num(), 0);
	// Vigor expressed = max(1,9) = 9 -> Health 9*4 = 36, plus the part's Health.
	TestTrue(TEXT("Derived stats recomputed"), Child.DerivedStats.Health > 0);

	return true;
}

// Same parents + same seed -> byte-identical genome.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingDeterminismTest,
	"EvolutionArena.Breeding.Determinism",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingDeterminismTest::RunTest(const FString&)
{
	UDataTable* Genes = MakeGeneTable();
	UDataTable* Parts = MakePartTable();
	UDataTable* Mutations = MakeMutationTable(500); // high chance so mutations participate

	FGenome ParentA;
	ParentA.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 2, 8));
	ParentA.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_Common")));
	ParentA.Parts.Add(MakePart(EBodyPartSlot::Head, TEXT("Part_Legendary")));

	FGenome ParentB;
	ParentB.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 3, 7));
	ParentB.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_Rare")));

	FRandomStream S1(777);
	const FGenome First = UBreedingLibrary::BreedGenomes(ParentA, ParentB, S1, Genes, Parts, Mutations);
	FRandomStream S2(777);
	const FGenome Second = UBreedingLibrary::BreedGenomes(ParentA, ParentB, S2, Genes, Parts, Mutations);

	TestTrue(TEXT("Same seed -> identical offspring genome"), GenomesEqual(First, Second));
	return true;
}

// Over many seeded trials the observed mutation rate matches the configured chance.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingMutationRateTest,
	"EvolutionArena.Breeding.MutationRate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingMutationRateTest::RunTest(const FString&)
{
	UDataTable* Mutations = MakeMutationTable(250); // 25%

	const int32 Trials = 4000;
	int32 Hits = 0;
	for (int32 i = 0; i < Trials; ++i)
	{
		FGenome Genome;
		FRandomStream Stream(i);
		UMutationLibrary::RollMutations(Genome, Stream, Mutations);
		if (Genome.Mutations.Num() > 0)
		{
			++Hits;
		}
	}

	const double Rate = static_cast<double>(Hits) / Trials;
	// Expected 0.25; band is very wide vs. the ~0.007 std error, so this is stable.
	TestTrue(FString::Printf(TEXT("Mutation rate %.3f within [0.20, 0.30]"), Rate), Rate >= 0.20 && Rate <= 0.30);
	return true;
}

// Constructed genomes map to the expected rarity tiers.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingRarityTest,
	"EvolutionArena.Breeding.Rarity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingRarityTest::RunTest(const FString&)
{
	UDataTable* Parts = MakePartTable();

	auto MakeGenome = [](const TArray<FName>& PartIds, const int32 MutationCount)
	{
		FGenome Genome;
		for (const FName& Id : PartIds)
		{
			Genome.Parts.Add(MakePart(EBodyPartSlot::Torso, Id));
		}
		for (int32 i = 0; i < MutationCount; ++i)
		{
			Genome.Mutations.Add(*FString::Printf(TEXT("Mut_%d"), i));
		}
		return Genome;
	};

	// Score = Σ part rarity (Common0/Rare2/Legendary4) + 2×mutations.
	TestTrue(TEXT("Common"),    UBreedingLibrary::CalculateRarity(MakeGenome({ TEXT("Part_Common") }, 0), Parts) == ERarity::Common);    // 0
	TestTrue(TEXT("Uncommon"),  UBreedingLibrary::CalculateRarity(MakeGenome({ TEXT("Part_Rare") }, 0), Parts) == ERarity::Uncommon);    // 2
	TestTrue(TEXT("Rare"),      UBreedingLibrary::CalculateRarity(MakeGenome({ TEXT("Part_Rare"), TEXT("Part_Rare") }, 0), Parts) == ERarity::Rare); // 4
	TestTrue(TEXT("Epic"),      UBreedingLibrary::CalculateRarity(MakeGenome({ TEXT("Part_Legendary"), TEXT("Part_Rare") }, 1), Parts) == ERarity::Epic);      // 4+2+2=8
	TestTrue(TEXT("Legendary"), UBreedingLibrary::CalculateRarity(MakeGenome({ TEXT("Part_Legendary"), TEXT("Part_Legendary") }, 1), Parts) == ERarity::Legendary); // 4+4+2=10
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
