// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Breeding/BreedingLibrary.h"
#include "Save/PlayerSave.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	FCreature MakeParent(const FString& Name)
	{
		FCreature Creature;
		Creature.CreatureId = FGuid::NewGuid();
		Creature.Name = Name;

		FGene Gene;
		Gene.GeneDefId = TEXT("Gene_X");
		Gene.AlleleA = 1;
		Gene.AlleleB = 2;
		Creature.Genome.Genes.Add(Gene);

		FPartAssignment Part;
		Part.Slot = EBodyPartSlot::Torso;
		Part.PartId = TEXT("Part_X");
		Creature.Genome.Parts.Add(Part);

		return Creature;
	}

	const FDateTime TestNow = FDateTime(2026, 6, 14, 12, 0, 0);
}

// Cooldown / cost / distinct-parent gates.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingTransactionCanBreedTest,
	"EvolutionArena.Breeding.Transaction.CanBreed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingTransactionCanBreedTest::RunTest(const FString&)
{
	FPlayerSave Save;
	Save.SoftCurrency = 100;
	const FCreature A = MakeParent(TEXT("A"));
	const FCreature B = MakeParent(TEXT("B"));
	FString Reason;

	TestTrue(TEXT("Two ready parents with funds can breed"),
		UBreedingLibrary::CanBreed(A, B, Save, TestNow, Reason));
	TestFalse(TEXT("A creature cannot breed with itself"),
		UBreedingLibrary::CanBreed(A, A, Save, TestNow, Reason));

	FCreature OnCooldown = MakeParent(TEXT("C"));
	OnCooldown.BreedCooldownUntil = TestNow + FTimespan::FromHours(1);
	TestFalse(TEXT("On-cooldown parent blocks breeding"),
		UBreedingLibrary::CanBreed(A, OnCooldown, Save, TestNow, Reason));

	FPlayerSave Poor;
	Poor.SoftCurrency = 10; // below BreedCost
	TestFalse(TEXT("Insufficient currency blocks breeding"),
		UBreedingLibrary::CanBreed(A, B, Poor, TestNow, Reason));
	return true;
}

// CommitBreed deducts cost, starts cooldowns, appends one offspring; gated breeds change nothing.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBreedingTransactionCommitTest,
	"EvolutionArena.Breeding.Transaction.Commit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBreedingTransactionCommitTest::RunTest(const FString&)
{
	FPlayerSave Save;
	Save.SoftCurrency = 100;
	const FCreature A = MakeParent(TEXT("A"));
	const FCreature B = MakeParent(TEXT("B"));
	const FGuid IdA = A.CreatureId;
	const FGuid IdB = B.CreatureId;
	Save.Creatures.Add(A);
	Save.Creatures.Add(B);

	FCreature Offspring;
	TestTrue(TEXT("Breed committed"),
		UBreedingLibrary::CommitBreed(Save, IdA, IdB, 123, nullptr, nullptr, nullptr, TestNow, Offspring));
	TestEqual(TEXT("Cost deducted"), Save.SoftCurrency, 50);
	TestEqual(TEXT("Offspring added to collection"), Save.Creatures.Num(), 3);
	TestTrue(TEXT("Lineage recorded"), Offspring.ParentA == IdA && Offspring.ParentB == IdB);

	const FCreature* ParentA = Save.Creatures.FindByPredicate([&](const FCreature& C) { return C.CreatureId == IdA; });
	TestTrue(TEXT("Parent A is now on cooldown"), ParentA && !ParentA->IsOffCooldown(TestNow));

	// Immediate re-breed is blocked by cooldown; nothing changes.
	FCreature Second;
	TestFalse(TEXT("Second breed blocked by cooldown"),
		UBreedingLibrary::CommitBreed(Save, IdA, IdB, 124, nullptr, nullptr, nullptr, TestNow, Second));
	TestEqual(TEXT("No extra spend on a gated breed"), Save.SoftCurrency, 50);
	TestEqual(TEXT("No extra creature on a gated breed"), Save.Creatures.Num(), 3);

	// Unknown parent id fails cleanly.
	FCreature Third;
	TestFalse(TEXT("Unknown parent id fails"),
		UBreedingLibrary::CommitBreed(Save, FGuid::NewGuid(), IdB, 125, nullptr, nullptr, nullptr, TestNow, Third));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
