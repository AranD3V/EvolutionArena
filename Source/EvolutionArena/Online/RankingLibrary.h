// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Battle/BattleTypes.h"
#include "RankingLibrary.generated.h"

/**
 * Ranked rating math. Integer, deterministic, Elo-like — no floats (consistent with
 * the rest of the sim). Beating a higher-rated opponent yields more. The constants
 * are MVP defaults and candidates to move to config.
 */
UCLASS()
class URankingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr int32 BaseRating = 1000;
	static constexpr int32 RatingKFactor = 32;
	static constexpr int32 RatingScale = 2;

	/**
	 * Rating change for the player (combatant A) given both ratings and the battle
	 * outcome. `Delta = K·(Score‰ − Expected‰)/1000`, Score‰ = 1000/500/0 for
	 * win/draw/loss, Expected‰ = clamp(500 − (Opp − Self)/RatingScale, 0, 1000).
	 */
	UFUNCTION(BlueprintPure, Category = "Arena")
	static int32 ComputeRatingDelta(int32 SelfRating, int32 OpponentRating, EBattleOutcome Outcome);
};
