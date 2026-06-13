// Copyright Evolution Arena. All Rights Reserved.

#include "Battle/BattleSimulator.h"
#include "Battle/BattleAI.h"
#include "Math/RandomStream.h"

int32 UBattleSimulator::ComputeAttackDamage(const FStatBlock& Attacker, const FStatBlock& Defender)
{
	return FMath::Max(1, Attacker.Attack - Defender.Defense / 2);
}

int32 UBattleSimulator::ComputeHeavyDamage(const FStatBlock& Attacker, const FStatBlock& Defender)
{
	return FMath::Max(1, (Attacker.Attack * 3) / 2 - Defender.Defense / 2);
}

FBattleResult UBattleSimulator::SimulateBattle(const FCreature& A, const FCreature& B, int32 Seed)
{
	FRandomStream Stream(Seed);

	FBattleCombatant Combatants[2];
	Combatants[0].CreatureId = A.CreatureId;
	Combatants[0].Stats = A.Genome.DerivedStats;
	Combatants[0].CurrentHealth = FMath::Max(1, A.Genome.DerivedStats.Health);
	Combatants[1].CreatureId = B.CreatureId;
	Combatants[1].Stats = B.Genome.DerivedStats;
	Combatants[1].CurrentHealth = FMath::Max(1, B.Genome.DerivedStats.Health);

	FBattleResult Result;
	Result.Seed = Seed;

	int32 Round = 0;
	bool bDecided = false;
	while (Round < MaxRounds && !bDecided)
	{
		++Round; // 1-based round number

		// Act order: higher Speed first; tie -> combatant A (index 0) first. Stable.
		int32 Order[2] = { 0, 1 };
		if (Combatants[1].Stats.Speed > Combatants[0].Stats.Speed)
		{
			Order[0] = 1;
			Order[1] = 0;
		}

		for (int32 Slot = 0; Slot < 2; ++Slot)
		{
			const int32 ActorIndex = Order[Slot];
			const int32 TargetIndex = 1 - ActorIndex;
			FBattleCombatant& Actor = Combatants[ActorIndex];
			FBattleCombatant& Target = Combatants[TargetIndex];

			if (Actor.CurrentHealth <= 0)
			{
				continue; // a combatant felled earlier this round doesn't act
			}

			const EBattleAction Action = UBattleAI::ChooseAction(Actor, Target);

			int32 Damage = 0;
			bool bMissed = false;
			if (Action == EBattleAction::Heavy)
			{
				// Integer per-mille hit roll. RNG is drawn only for Heavy, at a
				// deterministic point in the action sequence.
				const bool bHit = Stream.RandRange(0, 999) >= HeavyMissChancePermille;
				if (bHit)
				{
					Damage = ComputeHeavyDamage(Actor.Stats, Target.Stats);
				}
				else
				{
					bMissed = true;
				}
			}
			else
			{
				Damage = ComputeAttackDamage(Actor.Stats, Target.Stats);
			}

			Target.CurrentHealth = FMath::Max(0, Target.CurrentHealth - Damage);

			FBattleEvent Event;
			Event.Round = Round;
			Event.ActorIndex = ActorIndex;
			Event.Action = Action;
			Event.bMissed = bMissed;
			Event.Damage = Damage;
			Event.TargetHealthAfter = Target.CurrentHealth;
			Result.Timeline.Add(Event);

			if (Target.CurrentHealth <= 0)
			{
				bDecided = true;
				break; // round (and battle) ends on a death
			}
		}
	}

	Result.Rounds = Round;
	Result.FinalHealthA = Combatants[0].CurrentHealth;
	Result.FinalHealthB = Combatants[1].CurrentHealth;

	if (Combatants[1].CurrentHealth <= 0 && Combatants[0].CurrentHealth > 0)
	{
		Result.Outcome = EBattleOutcome::CombatantAWon;
	}
	else if (Combatants[0].CurrentHealth <= 0 && Combatants[1].CurrentHealth > 0)
	{
		Result.Outcome = EBattleOutcome::CombatantBWon;
	}
	else
	{
		// Hit the round cap with both alive (or both somehow at 0): decide by health.
		if (Combatants[0].CurrentHealth > Combatants[1].CurrentHealth)
		{
			Result.Outcome = EBattleOutcome::CombatantAWon;
		}
		else if (Combatants[1].CurrentHealth > Combatants[0].CurrentHealth)
		{
			Result.Outcome = EBattleOutcome::CombatantBWon;
		}
		else
		{
			Result.Outcome = EBattleOutcome::Draw;
		}
	}

	return Result;
}
