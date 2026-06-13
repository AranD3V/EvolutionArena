// Copyright Evolution Arena. All Rights Reserved.

#include "Genetics/GeneticsLibrary.h"
#include "Engine/DataTable.h"

uint8 UGeneticsLibrary::ResolveExpressedAllele(const FGene& Gene, EGeneDominance Dominance)
{
	switch (Dominance)
	{
	case EGeneDominance::Recessive:
		return FMath::Min(Gene.AlleleA, Gene.AlleleB);
	case EGeneDominance::Codominant:
		// Widen to int32 before adding so the sum can't overflow uint8.
		return static_cast<uint8>((static_cast<int32>(Gene.AlleleA) + static_cast<int32>(Gene.AlleleB)) / 2);
	case EGeneDominance::Dominant:
	default:
		return FMath::Max(Gene.AlleleA, Gene.AlleleB);
	}
}

FStatBlock UGeneticsLibrary::ComputeDerivedStats(const FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable)
{
	FStatBlock Result;

	// Parts contribute their flat stat modifiers.
	if (PartTable)
	{
		static const FString Context(TEXT("ComputeDerivedStats:Part"));
		for (const FPartAssignment& Part : Genome.Parts)
		{
			if (const FBodyPartDef* Def = PartTable->FindRow<FBodyPartDef>(Part.PartId, Context, /*bWarnIfMissing=*/ false))
			{
				Result += Def->StatModifiers;
			}
		}
	}

	// Stat genes contribute StatMapping scaled by their expressed allele value.
	if (GeneTable)
	{
		static const FString Context(TEXT("ComputeDerivedStats:Gene"));
		for (const FGene& Gene : Genome.Genes)
		{
			const FGeneDef* Def = GeneTable->FindRow<FGeneDef>(Gene.GeneDefId, Context, /*bWarnIfMissing=*/ false);
			if (Def && Def->Type == EGeneType::Stat)
			{
				const int32 Expressed = static_cast<int32>(ResolveExpressedAllele(Gene, Def->Dominance));
				Result += Def->StatMapping * Expressed;
			}
		}
	}

	return Result;
}

void UGeneticsLibrary::RecomputeDerivedStats(FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable)
{
	Genome.DerivedStats = ComputeDerivedStats(Genome, PartTable, GeneTable);
}

bool UGeneticsLibrary::ValidateGenome(const FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable, TArray<FString>& OutErrors)
{
	static const FString Context(TEXT("ValidateGenome"));

	for (const FPartAssignment& Part : Genome.Parts)
	{
		const FBodyPartDef* Def = PartTable ? PartTable->FindRow<FBodyPartDef>(Part.PartId, Context, /*bWarnIfMissing=*/ false) : nullptr;
		if (!Def)
		{
			OutErrors.Add(FString::Printf(TEXT("Part row '%s' not found"), *Part.PartId.ToString()));
		}
		else if (Def->Slot != Part.Slot)
		{
			OutErrors.Add(FString::Printf(TEXT("Part '%s' assigned to slot %d but its definition is slot %d"),
				*Part.PartId.ToString(), static_cast<int32>(Part.Slot), static_cast<int32>(Def->Slot)));
		}
	}

	for (const FGene& Gene : Genome.Genes)
	{
		const bool bFound = GeneTable && GeneTable->FindRow<FGeneDef>(Gene.GeneDefId, Context, /*bWarnIfMissing=*/ false) != nullptr;
		if (!bFound)
		{
			OutErrors.Add(FString::Printf(TEXT("Gene row '%s' not found"), *Gene.GeneDefId.ToString()));
		}
	}

	return OutErrors.Num() == 0;
}
