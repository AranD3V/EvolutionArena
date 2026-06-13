// Copyright Evolution Arena. All Rights Reserved.

#include "Breeding/BreedingLibrary.h"
#include "Breeding/MutationLibrary.h"
#include "Genetics/GeneticsLibrary.h"
#include "Genetics/GeneticsRows.h"
#include "Engine/DataTable.h"

namespace
{
	// Deterministic coin flip: pick one of a gene's two alleles.
	uint8 PickAllele(const FGene& Gene, FRandomStream& Stream)
	{
		return (Stream.RandRange(0, 1) == 0) ? Gene.AlleleA : Gene.AlleleB;
	}

	const FGene* FindGeneById(const FGenome& Genome, const FName Id)
	{
		for (const FGene& Gene : Genome.Genes)
		{
			if (Gene.GeneDefId == Id)
			{
				return &Gene;
			}
		}
		return nullptr;
	}

	const FPartAssignment* FindPartBySlot(const FGenome& Genome, const EBodyPartSlot Slot)
	{
		for (const FPartAssignment& Part : Genome.Parts)
		{
			if (Part.Slot == Slot)
			{
				return &Part;
			}
		}
		return nullptr;
	}
}

FGenome UBreedingLibrary::BreedGenomes(const FGenome& ParentA, const FGenome& ParentB, FRandomStream& Stream,
	const UDataTable* GeneTable, const UDataTable* PartTable, const UDataTable* MutationTable)
{
	FGenome Offspring;

	// --- Genes: Mendelian per locus. Iterate A's genes (stable order), then B-only
	//     genes, so the RNG draw sequence is identical every run. ---
	for (const FGene& GeneA : ParentA.Genes)
	{
		FGene Child;
		Child.GeneDefId = GeneA.GeneDefId;
		Child.AlleleA = PickAllele(GeneA, Stream); // one allele from parent A
		if (const FGene* GeneB = FindGeneById(ParentB, GeneA.GeneDefId))
		{
			Child.AlleleB = PickAllele(*GeneB, Stream); // one allele from parent B
		}
		else
		{
			Child.AlleleB = PickAllele(GeneA, Stream); // locus only in A → both from A
		}
		Offspring.Genes.Add(Child);
	}
	for (const FGene& GeneB : ParentB.Genes)
	{
		if (FindGeneById(ParentA, GeneB.GeneDefId) != nullptr)
		{
			continue; // shared locus already produced above
		}
		FGene Child;
		Child.GeneDefId = GeneB.GeneDefId;
		Child.AlleleA = PickAllele(GeneB, Stream);
		Child.AlleleB = PickAllele(GeneB, Stream);
		Offspring.Genes.Add(Child);
	}

	// --- Parts: each slot inherited from one parent by a seeded coin flip. ---
	for (const FPartAssignment& PartA : ParentA.Parts)
	{
		const FPartAssignment* PartB = FindPartBySlot(ParentB, PartA.Slot);
		// RandRange is always evaluated first, so the draw count is stable regardless
		// of whether parent B has this slot.
		const bool bTakeA = (Stream.RandRange(0, 1) == 0) || (PartB == nullptr);
		Offspring.Parts.Add(bTakeA ? PartA : *PartB);
	}
	for (const FPartAssignment& PartB : ParentB.Parts)
	{
		if (FindPartBySlot(ParentA, PartB.Slot) == nullptr)
		{
			Offspring.Parts.Add(PartB); // slot only in B
		}
	}

	// --- Mutations, then derived stats. ---
	UMutationLibrary::RollMutations(Offspring, Stream, MutationTable);
	UGeneticsLibrary::RecomputeDerivedStats(Offspring, PartTable, GeneTable);

	return Offspring;
}

ERarity UBreedingLibrary::CalculateRarity(const FGenome& Genome, const UDataTable* PartTable)
{
	int32 Score = 0;

	if (PartTable)
	{
		static const FString Context(TEXT("CalculateRarity"));
		for (const FPartAssignment& Part : Genome.Parts)
		{
			if (const FBodyPartDef* Def = PartTable->FindRow<FBodyPartDef>(Part.PartId, Context, /*bWarnIfMissing=*/ false))
			{
				Score += static_cast<int32>(Def->Rarity);
			}
		}
	}

	// Mutations skew rarity upward.
	Score += 2 * Genome.Mutations.Num();

	// MVP thresholds — candidates to move to config (see docs/TechSpec.md §7).
	if (Score >= 10) { return ERarity::Legendary; }
	if (Score >= 7)  { return ERarity::Epic; }
	if (Score >= 4)  { return ERarity::Rare; }
	if (Score >= 2)  { return ERarity::Uncommon; }
	return ERarity::Common;
}

FCreature UBreedingLibrary::BreedCreatures(const FCreature& ParentA, const FCreature& ParentB, int32 Seed,
	const UDataTable* GeneTable, const UDataTable* PartTable, const UDataTable* MutationTable)
{
	FRandomStream Stream(Seed);

	FCreature Child;
	Child.CreatureId = FGuid::NewGuid();        // unique identity — not part of determinism
	Child.ParentA = ParentA.CreatureId;
	Child.ParentB = ParentB.CreatureId;
	Child.Genome = BreedGenomes(ParentA.Genome, ParentB.Genome, Stream, GeneTable, PartTable, MutationTable);
	Child.Rarity = CalculateRarity(Child.Genome, PartTable);
	Child.CreatedUtc = FDateTime::UtcNow();      // metadata — not part of determinism
	Child.Name = TEXT("Offspring");
	return Child;
}

bool UBreedingLibrary::CanBreed(const FCreature& ParentA, const FCreature& ParentB, const FPlayerSave& Save,
	const FDateTime NowUtc, FString& OutReason)
{
	if (ParentA.CreatureId == ParentB.CreatureId)
	{
		OutReason = TEXT("A creature cannot breed with itself.");
		return false;
	}
	if (!ParentA.IsOffCooldown(NowUtc) || !ParentB.IsOffCooldown(NowUtc))
	{
		OutReason = TEXT("A parent is still on breeding cooldown.");
		return false;
	}
	if (Save.SoftCurrency < BreedCost)
	{
		OutReason = FString::Printf(TEXT("Not enough currency (need %d)."), BreedCost);
		return false;
	}

	OutReason.Reset();
	return true;
}

bool UBreedingLibrary::CommitBreed(FPlayerSave& Save, const FGuid& ParentAId, const FGuid& ParentBId, int32 Seed,
	const UDataTable* GeneTable, const UDataTable* PartTable, const UDataTable* MutationTable,
	const FDateTime NowUtc, FCreature& OutOffspring)
{
	const int32 IndexA = Save.Creatures.IndexOfByPredicate([&](const FCreature& C) { return C.CreatureId == ParentAId; });
	const int32 IndexB = Save.Creatures.IndexOfByPredicate([&](const FCreature& C) { return C.CreatureId == ParentBId; });
	if (IndexA == INDEX_NONE || IndexB == INDEX_NONE)
	{
		return false; // a parent isn't in the collection
	}

	FString Reason;
	if (!CanBreed(Save.Creatures[IndexA], Save.Creatures[IndexB], Save, NowUtc, Reason))
	{
		return false; // gated — no state changes
	}

	// Produce the offspring first (reads the parent genomes), then mutate state.
	OutOffspring = BreedCreatures(Save.Creatures[IndexA], Save.Creatures[IndexB], Seed, GeneTable, PartTable, MutationTable);

	Save.SoftCurrency -= BreedCost;
	const FDateTime CooldownUntil = NowUtc + FTimespan::FromHours(BreedCooldownHours);
	Save.Creatures[IndexA].BreedCooldownUntil = CooldownUntil;
	Save.Creatures[IndexB].BreedCooldownUntil = CooldownUntil;

	Save.Creatures.Add(OutOffspring);
	return true;
}
