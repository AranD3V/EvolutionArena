// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "BattleTypes.generated.h"

/** An action a combatant can take on its turn. */
UENUM(BlueprintType)
enum class EBattleAction : uint8
{
	Attack,
	Heavy
};

/** Result of a 1v1 battle from combatant A's frame of reference. */
UENUM(BlueprintType)
enum class EBattleOutcome : uint8
{
	CombatantAWon,
	CombatantBWon,
	Draw
};

/** Runtime combat state for one creature (built from its derived stats; not saved). */
USTRUCT(BlueprintType)
struct FBattleCombatant
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	FGuid CreatureId;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	FStatBlock Stats;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 CurrentHealth = 0;
};

/**
 * One recorded event in the battle timeline. The simulation has no rendering
 * dependency — it produces this ordered list, and the (later) playback UI just
 * animates it. See docs/TechSpec.md §4.
 */
USTRUCT(BlueprintType)
struct FBattleEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 Round = 0;

	/** 0 = combatant A, 1 = combatant B. */
	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 ActorIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	EBattleAction Action = EBattleAction::Attack;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	bool bMissed = false;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 Damage = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 TargetHealthAfter = 0;

	bool operator==(const FBattleEvent& Other) const
	{
		return Round == Other.Round
			&& ActorIndex == Other.ActorIndex
			&& Action == Other.Action
			&& bMissed == Other.bMissed
			&& Damage == Other.Damage
			&& TargetHealthAfter == Other.TargetHealthAfter;
	}
};

/** The full, deterministic result of a battle (for a given pair of creatures + seed). */
USTRUCT(BlueprintType)
struct FBattleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	EBattleOutcome Outcome = EBattleOutcome::Draw;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 Rounds = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 FinalHealthA = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 FinalHealthB = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	int32 Seed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
	TArray<FBattleEvent> Timeline;
};
