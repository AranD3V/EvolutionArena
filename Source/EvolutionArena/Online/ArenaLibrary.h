// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Battle/BattleTypes.h"
#include "Creature/Creature.h"
#include "ArenaLibrary.generated.h"

class ULeaderboardService;

/** Outcome of one ranked match: the battle plus the rating change applied. */
USTRUCT(BlueprintType)
struct FRankedMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	FBattleResult Battle;

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	int32 RatingBefore = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	int32 RatingAfter = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	int32 RatingDelta = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	bool bWin = false;

	UPROPERTY(BlueprintReadOnly, Category = "Arena")
	FString OpponentId;
};

/**
 * Ranked arena orchestration: ties opponent selection, the deterministic battle sim,
 * the rating update, and result submission together — all through the
 * `ULeaderboardService` interface (never EOS directly).
 */
UCLASS()
class URankedArenaLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Run one ranked match: select an opponent from the service, simulate the battle
	 * (player = combatant A), update rating, submit the result. Returns false if no
	 * opponent was available. Deterministic for a given seed + service state.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arena")
	static bool RunRankedMatch(const FCreature& PlayerLoadout, int32 PlayerRating, const FString& PlayerId,
		ULeaderboardService* Service, int32 Seed, FRankedMatchResult& OutResult);
};
