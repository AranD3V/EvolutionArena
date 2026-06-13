// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Battle/BattleTypes.h"
#include "Creature/Creature.h"
#include "BattleSimulator.generated.h"

/**
 * The Battle System: resolves a 1v1 auto-battle deterministically from two creatures
 * and a seed, producing an outcome plus a replayable event timeline. No rendering
 * dependency. Same creatures + seed → identical result (the determinism release gate;
 * see docs/rules.md and docs/TechSpec.md §7). Integer-only math, single seeded stream.
 */
UCLASS()
class UBattleSimulator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Per-mille chance a Heavy attack misses (integer roll — no floats). */
	static constexpr int32 HeavyMissChancePermille = 250;

	/** Hard round cap so the sim always terminates (then decided by remaining health). */
	static constexpr int32 MaxRounds = 200;

	/** Resolve a full battle. Deterministic for a given pair of creatures + seed. */
	UFUNCTION(BlueprintCallable, Category = "Battle")
	static FBattleResult SimulateBattle(const FCreature& A, const FCreature& B, int32 Seed);

	/** Damage formulas — shared by the sim and the AI's lethality check so they can't drift. */
	static int32 ComputeAttackDamage(const FStatBlock& Attacker, const FStatBlock& Defender);
	static int32 ComputeHeavyDamage(const FStatBlock& Attacker, const FStatBlock& Defender);
};
