// Copyright Evolution Arena. All Rights Reserved.

#include "Battle/BattleAI.h"
#include "Battle/BattleSimulator.h"

EBattleAction UBattleAI::ChooseAction(const FBattleCombatant& Actor, const FBattleCombatant& Opponent)
{
	// Go for the kill: if a Heavy would be lethal on hit, take the risk. Otherwise
	// play it safe with a guaranteed Attack. (Uses the simulator's damage formula so
	// the AI's reasoning matches what actually gets applied.)
	const int32 HeavyDamage = UBattleSimulator::ComputeHeavyDamage(Actor.Stats, Opponent.Stats);
	if (Opponent.CurrentHealth <= HeavyDamage)
	{
		return EBattleAction::Heavy;
	}
	return EBattleAction::Attack;
}
