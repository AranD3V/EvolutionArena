// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EvoGeneticsTypes.generated.h"

/**
 * Rarity tier of a creature (and, later, of parts/genes/mutations).
 * UENUM(BlueprintType) + uint8 makes it usable from Blueprint and DataTables.
 * See docs/schema.md §6.
 */
UENUM(BlueprintType)
enum class ERarity : uint8
{
	Common		UMETA(DisplayName = "Common"),
	Uncommon	UMETA(DisplayName = "Uncommon"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary")
};

/** A body region a part can occupy. One part per slot per creature. */
UENUM(BlueprintType)
enum class EBodyPartSlot : uint8
{
	Head,
	Torso,
	Limbs,
	Tail,
	Special
};

/** What a gene influences. */
UENUM(BlueprintType)
enum class EGeneType : uint8
{
	Stat,
	Visual,
	Special
};

/**
 * The combat-relevant stats every creature has. Named ECreatureStat (not the
 * obvious EStatType) because the engine already defines a global EStatType in
 * Core — game types must avoid the engine's reserved names.
 */
UENUM(BlueprintType)
enum class ECreatureStat : uint8
{
	Health,
	Attack,
	Defense,
	Speed
};

/**
 * The derived combat stats of a creature, computed from its genome
 * (genes + parts) by the Genetics System and cached on the genome so gameplay
 * and UI don't recompute on every read.
 *
 * The += / + operators exist because deriving stats means accumulating the
 * contributions of each part and expressed gene; == exists so save round-trips
 * and tests can compare blocks. See docs/schema.md §3 (FStatBlock).
 */
USTRUCT(BlueprintType)
struct FStatBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Stats")
	int32 Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Stats")
	int32 Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Stats")
	int32 Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Stats")
	int32 Speed = 0;

	FStatBlock& operator+=(const FStatBlock& Other)
	{
		Health  += Other.Health;
		Attack  += Other.Attack;
		Defense += Other.Defense;
		Speed   += Other.Speed;
		return *this;
	}

	friend FStatBlock operator+(FStatBlock A, const FStatBlock& B)
	{
		A += B;
		return A;
	}

	/**
	 * Component-wise scale. Used to scale a gene's per-unit StatMapping by its
	 * expressed allele value when deriving stats. Integer math only (determinism).
	 */
	FStatBlock operator*(int32 Scalar) const
	{
		FStatBlock Result;
		Result.Health  = Health  * Scalar;
		Result.Attack  = Attack  * Scalar;
		Result.Defense = Defense * Scalar;
		Result.Speed   = Speed   * Scalar;
		return Result;
	}

	bool operator==(const FStatBlock& Other) const
	{
		return Health == Other.Health
			&& Attack == Other.Attack
			&& Defense == Other.Defense
			&& Speed == Other.Speed;
	}

	bool operator!=(const FStatBlock& Other) const { return !(*this == Other); }
};
