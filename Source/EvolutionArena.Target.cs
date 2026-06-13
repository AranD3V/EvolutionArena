// Copyright Evolution Arena. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EvolutionArenaTarget : TargetRules
{
	public EvolutionArenaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("EvolutionArena");
	}
}
