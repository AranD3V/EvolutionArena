// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Engine/DataTable.h"
#include "Genetics/GeneticsLibrary.h"
#include "Genetics/GeneticsRows.h"
#include "Genetics/Genome.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	// Builds a transient DataTable from a JSON string — the same import path the
	// runtime game uses, so the tests need no .uasset and also exercise the JSON
	// format the seed files in Data/ use.
	UDataTable* MakePartTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FBodyPartDef::StaticStruct();
		const FString Json = TEXT("[")
			TEXT("{ \"Name\": \"Part_BasicTorso\", \"Slot\": \"Torso\", \"Rarity\": \"Common\", \"StatModifiers\": { \"Health\": 20, \"Attack\": 0, \"Defense\": 5, \"Speed\": 0 } },")
			TEXT("{ \"Name\": \"Part_BasicLimbs\", \"Slot\": \"Limbs\", \"Rarity\": \"Common\", \"StatModifiers\": { \"Health\": 0, \"Attack\": 4, \"Defense\": 2, \"Speed\": 6 } }")
			TEXT("]");
		Table->CreateTableFromJSONString(Json);
		return Table;
	}

	UDataTable* MakeGeneTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FGeneDef::StaticStruct();
		const FString Json = TEXT("[")
			TEXT("{ \"Name\": \"Gene_Vigor\",    \"Type\": \"Stat\",   \"Dominance\": \"Dominant\",  \"StatMapping\": { \"Health\": 4, \"Attack\": 0, \"Defense\": 0, \"Speed\": 0 } },")
			TEXT("{ \"Name\": \"Gene_Carapace\", \"Type\": \"Stat\",   \"Dominance\": \"Recessive\", \"StatMapping\": { \"Health\": 0, \"Attack\": 0, \"Defense\": 3, \"Speed\": 0 } },")
			TEXT("{ \"Name\": \"Gene_Decor\",    \"Type\": \"Visual\", \"Dominance\": \"Dominant\",  \"StatMapping\": { \"Health\": 0, \"Attack\": 99, \"Defense\": 0, \"Speed\": 0 } }")
			TEXT("]");
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

	void CheckStats(FAutomationTestBase& Test, const TCHAR* Label, const FStatBlock& Actual, const FStatBlock& Expected)
	{
		Test.TestEqual(FString::Printf(TEXT("%s Health"), Label), Actual.Health, Expected.Health);
		Test.TestEqual(FString::Printf(TEXT("%s Attack"), Label), Actual.Attack, Expected.Attack);
		Test.TestEqual(FString::Printf(TEXT("%s Defense"), Label), Actual.Defense, Expected.Defense);
		Test.TestEqual(FString::Printf(TEXT("%s Speed"), Label), Actual.Speed, Expected.Speed);
	}
}

// Derived stats = Σ part modifiers + Σ expressed Stat-gene contributions.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticsStatComputationTest,
	"EvolutionArena.Genetics.StatComputation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticsStatComputationTest::RunTest(const FString&)
{
	UDataTable* Parts = MakePartTable();
	UDataTable* Genes = MakeGeneTable();

	FGenome Genome;
	Genome.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_BasicTorso")));
	Genome.Parts.Add(MakePart(EBodyPartSlot::Limbs, TEXT("Part_BasicLimbs")));
	Genome.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 3, 7));    // Dominant  -> 7 -> Health  +28
	Genome.Genes.Add(MakeGene(TEXT("Gene_Carapace"), 2, 5)); // Recessive -> 2 -> Defense +6
	Genome.Genes.Add(MakeGene(TEXT("Gene_Decor"), 9, 9));    // Visual    -> ignored (not a Stat gene)

	const FStatBlock Stats = UGeneticsLibrary::ComputeDerivedStats(Genome, Parts, Genes);

	FStatBlock Expected;
	Expected.Health = 48;  // Torso 20 + Vigor 28
	Expected.Attack = 4;   // Limbs 4   (Decor's +99 must NOT apply)
	Expected.Defense = 13; // Torso 5 + Limbs 2 + Carapace 6
	Expected.Speed = 6;    // Limbs 6
	CheckStats(*this, TEXT("Derived"), Stats, Expected);

	return true;
}

// Dominant -> Max(A,B), Recessive -> Min(A,B), Codominant -> (A+B)/2.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticsDominanceTest,
	"EvolutionArena.Genetics.DominanceResolution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticsDominanceTest::RunTest(const FString&)
{
	const FGene Gene = MakeGene(TEXT("X"), 2, 7);
	TestEqual(TEXT("Dominant -> max"),   (int32)UGeneticsLibrary::ResolveExpressedAllele(Gene, EGeneDominance::Dominant), 7);
	TestEqual(TEXT("Recessive -> min"),  (int32)UGeneticsLibrary::ResolveExpressedAllele(Gene, EGeneDominance::Recessive), 2);
	TestEqual(TEXT("Codominant -> avg"), (int32)UGeneticsLibrary::ResolveExpressedAllele(Gene, EGeneDominance::Codominant), 4);
	return true;
}

// Same inputs -> identical stats; element order must not change the result.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticsDeterminismTest,
	"EvolutionArena.Genetics.Determinism",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticsDeterminismTest::RunTest(const FString&)
{
	UDataTable* Parts = MakePartTable();
	UDataTable* Genes = MakeGeneTable();

	FGenome A;
	A.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_BasicTorso")));
	A.Parts.Add(MakePart(EBodyPartSlot::Limbs, TEXT("Part_BasicLimbs")));
	A.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 3, 7));
	A.Genes.Add(MakeGene(TEXT("Gene_Carapace"), 2, 5));

	const FStatBlock First = UGeneticsLibrary::ComputeDerivedStats(A, Parts, Genes);
	const FStatBlock Second = UGeneticsLibrary::ComputeDerivedStats(A, Parts, Genes);
	TestTrue(TEXT("Repeated compute is identical"), First == Second);

	FGenome B; // same content, reversed order
	B.Parts.Add(MakePart(EBodyPartSlot::Limbs, TEXT("Part_BasicLimbs")));
	B.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_BasicTorso")));
	B.Genes.Add(MakeGene(TEXT("Gene_Carapace"), 2, 5));
	B.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 3, 7));
	const FStatBlock Reordered = UGeneticsLibrary::ComputeDerivedStats(B, Parts, Genes);
	TestTrue(TEXT("Order-independent"), Reordered == First);

	return true;
}

// Missing referenced rows are reported; a clean genome validates.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticsContentValidationTest,
	"EvolutionArena.Genetics.ContentValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticsContentValidationTest::RunTest(const FString&)
{
	UDataTable* Parts = MakePartTable();
	UDataTable* Genes = MakeGeneTable();

	FGenome Valid;
	Valid.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_BasicTorso")));
	Valid.Genes.Add(MakeGene(TEXT("Gene_Vigor"), 1, 1));
	TArray<FString> Errors;
	TestTrue(TEXT("Valid genome passes"), UGeneticsLibrary::ValidateGenome(Valid, Parts, Genes, Errors));
	TestEqual(TEXT("No errors reported"), Errors.Num(), 0);

	FGenome Invalid;
	Invalid.Parts.Add(MakePart(EBodyPartSlot::Torso, TEXT("Part_Missing")));
	Invalid.Genes.Add(MakeGene(TEXT("Gene_Missing"), 1, 1));
	Errors.Reset();
	TestFalse(TEXT("Invalid genome fails"), UGeneticsLibrary::ValidateGenome(Invalid, Parts, Genes, Errors));
	TestEqual(TEXT("Two missing-row errors"), Errors.Num(), 2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
