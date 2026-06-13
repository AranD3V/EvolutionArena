// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Battle/BattleTypes.h"
#include "BattleAI.generated.h"

/**
 * Creature combat AI. The decision is pure and deterministic — there is no RNG in
 * the choice itself (RNG only governs whether a Heavy connects, inside the
 * simulator). MVP rule: go for the kill with Heavy when it would be lethal on hit,
 * otherwise Attack. This keeps outcomes varying sensibly with state (PRD).
 */
UCLASS()
class UBattleAI : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Battle")
	static EBattleAction ChooseAction(const FBattleCombatant& Actor, const FBattleCombatant& Opponent);
};
